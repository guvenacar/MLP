#!/usr/bin/env python3
"""
MLP Multi-Language/Multi-Syntax Preprocessor
Two-stage pipeline: Keyword Translation → Syntax Transformation → Base IR

Architecture:
1. Stage 1: Natural language keywords → English (using diller_comprehensive.json)
2. Stage 2: Programming syntax → MLP Base IR (using syntax_comprehensive.json)
3. Block Type Tracking: Explicit terminators (end if, end while, end for)

Author: MLP Team
Version: 2.0
"""

import json
import sys
import re
from pathlib import Path
from typing import Dict, List, Tuple, Optional


class MLPPreprocessor:
    """Two-stage preprocessor with block type tracking"""
    
    def __init__(self, 
                 languages_file="diller_comprehensive.json",
                 syntax_file="syntax_comprehensive.json"):
        """Initialize preprocessor with language and syntax definitions"""
        
        # Load language definitions
        with open(languages_file, 'r', encoding='utf-8') as f:
            lang_data = json.load(f)
        
        self.languages = {}
        for lang in lang_data['languages']:
            self.languages[lang['id']] = lang
        
        self.default_lang = lang_data.get('default_language', 'en-US')
        
        # Load syntax definitions
        with open(syntax_file, 'r', encoding='utf-8') as f:
            syntax_data = json.load(f)
        
        self.syntax_styles = {}
        for style in syntax_data['syntax_styles']:
            self.syntax_styles[style['id']] = style
        
        self.default_syntax = syntax_data.get('default_syntax', 'mlp')
    
    def detect_language(self, source_code: str) -> str:
        """Detect language from header comment
        
        Looks for: -- lang: tr-TR or // lang: tr-TR
        """
        match = re.search(r'(--|//)\s*(lang|dil):\s*(\S+)', source_code, re.IGNORECASE)
        if match:
            return match.group(3)
        return self.default_lang
    
    def detect_syntax(self, source_code: str) -> str:
        """Detect syntax style from header comment
        
        Looks for: -- syntax: c or // syntax: python
        """
        match = re.search(r'(--|//)\s*syntax:\s*(\S+)', source_code, re.IGNORECASE)
        if match:
            return match.group(2)
        return self.default_syntax
    
    def build_translation_map(self, lang_id: str) -> Dict[str, str]:
        """Build keyword translation map for given language
        
        Returns: dict mapping native keywords to English
        """
        if lang_id not in self.languages:
            print(f"Warning: Language '{lang_id}' not found, using {self.default_lang}")
            lang_id = self.default_lang
        
        lang = self.languages[lang_id]
        translation_map = {}
        
        for english_kw, native_variants in lang['keywords'].items():
            for variant in native_variants:
                translation_map[variant] = english_kw
        
        return translation_map
    
    def stage1_translate_keywords(self, source_code: str, lang_id: str) -> str:
        """Stage 1: Translate natural language keywords to English
        
        Args:
            source_code: Source code with native keywords
            lang_id: Language ID (e.g., 'tr-TR')
        
        Returns:
            Code with English keywords
        """
        # Build translation map
        translation_map = self.build_translation_map(lang_id)
        
        # State machine for tokenization
        STATE_CODE = 0
        STATE_STRING = 1
        STATE_COMMENT = 2
        
        state = STATE_CODE
        result = []
        current_word = []
        i = 0
        
        while i < len(source_code):
            c = source_code[i]
            
            # Handle string literals
            if c == '"':
                if current_word:
                    result.append(''.join(current_word))
                    current_word = []
                
                if state == STATE_CODE:
                    state = STATE_STRING
                    result.append(c)
                elif state == STATE_STRING:
                    state = STATE_CODE
                    result.append(c)
                i += 1
                continue
            
            # Handle comments (both -- and //)
            if state == STATE_CODE:
                if c == '-' and i + 1 < len(source_code) and source_code[i+1] == '-':
                    if current_word:
                        word = ''.join(current_word)
                        result.append(translation_map.get(word, word))
                        current_word = []
                    state = STATE_COMMENT
                    result.append('--')
                    i += 2
                    continue
                elif c == '/' and i + 1 < len(source_code) and source_code[i+1] == '/':
                    if current_word:
                        word = ''.join(current_word)
                        result.append(translation_map.get(word, word))
                        current_word = []
                    state = STATE_COMMENT
                    result.append('--')  # Convert // to --
                    i += 2
                    continue
            
            # Handle newline (exit comment)
            if c == '\n':
                if state == STATE_COMMENT:
                    state = STATE_CODE
                if current_word:
                    word = ''.join(current_word)
                    result.append(translation_map.get(word, word))
                    current_word = []
                result.append(c)
                i += 1
                continue
            
            # Pass through strings and comments unchanged
            if state in (STATE_STRING, STATE_COMMENT):
                result.append(c)
                i += 1
                continue
            
            # CODE state: check for word boundaries
            # IMPORTANT: Include ':' for Python-style syntax (if x:, else:, while x:)
            if c in ' \t\n;(){}[]<>=!+-*/,.:' :
                if current_word:
                    word = ''.join(current_word)
                    # Translate if it's a keyword
                    result.append(translation_map.get(word, word))
                    current_word = []
                result.append(c)
                i += 1
            else:
                # Build word
                current_word.append(c)
                i += 1
        
        # Process final word
        if current_word:
            word = ''.join(current_word)
            result.append(translation_map.get(word, word))
        
        return ''.join(result)
    
    def stage2_transform_syntax(self, source_code: str, syntax_id: str) -> str:
        """Stage 2: Transform programming syntax to MLP Base IR
        
        Args:
            source_code: Code with English keywords
            syntax_id: Syntax style ID (e.g., 'c', 'python')
        
        Returns:
            Code in MLP Base IR format with explicit block terminators
        """
        if syntax_id not in self.syntax_styles:
            print(f"Warning: Syntax '{syntax_id}' not found, using {self.default_syntax}")
            syntax_id = self.default_syntax
        
        style = self.syntax_styles[syntax_id]
        
        # If it's already MLP base syntax, no transformation needed
        if style.get('no_transformation', False):
            return source_code
        
        # Get transformation rules
        transformations = style.get('transformations', {})
        rules = transformations.get('rules', [])

        # Apply transformations with block type tracking
        # VB.NET already has explicit terminators, skip block tracking for it
        result = self._apply_transformations_with_tracking(source_code, rules, syntax_id)

        return result

    def _apply_transformations_with_tracking(self, code: str, rules: List[dict], syntax_id: str) -> str:
        """Apply syntax transformations with block type tracking

        This ensures explicit block terminators: end if, end while, end for

        Args:
            code: Source code
            rules: Transformation rules
            syntax_id: Syntax style ID (needed to skip block tracking for VB.NET)
        """
        result = code
        
        # Sort rules by priority if available
        rules_sorted = sorted(rules, key=lambda r: r.get('priority', 999))
        
        for rule in rules_sorted:
            name = rule.get('name', '')
            pattern = rule.get('pattern', '')
            replacement = rule.get('replacement', '')
            
            if not pattern:
                continue
            
            # FIX: while_loop should preserve condition (NO 'then' keyword)
            if 'while_loop' in name and '\\1' not in replacement:
                # Override the rule to preserve condition
                # while (condition) { → while condition
                pattern = r'\bwhile\s*\(([^)]+)\)\s*\{'
                replacement = r'while \1'
            
            # Apply the transformation
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Add explicit block terminators
        # SKIP for VB.NET, Pascal, BASIC - they already have explicit terminators
        if syntax_id not in ['vbnet', 'pascal', 'basic']:
            result = self._add_explicit_terminators(result)

        return result
    
    def _add_explicit_terminators(self, code: str) -> str:
        """Add explicit block terminators: end if, end while, end for
        
        This is the key improvement: instead of generic 'end',
        we explicitly track what type of block we're closing.
        
        Also removes trailing colons from Python-style syntax.
        """
        lines = code.split('\n')
        result_lines = []
        block_stack = []  # Stack of (block_type, indent_level)
        
        for line in lines:
            stripped = line.strip()
            
            # Skip empty lines
            if not stripped:
                result_lines.append(line)
                continue
            
            # Calculate indentation level
            indent_level = len(line) - len(line.lstrip())

            # Remove trailing colons (Python style)
            if stripped.endswith(':'):
                stripped = stripped[:-1].rstrip()
                line = line[:len(line) - len(line.lstrip())] + stripped

            # CRITICAL: Check for else/else if BEFORE dedent control
            # These keywords continue the if block, don't close it!
            is_else_clause = (stripped == 'else' or stripped.startswith('else if'))

            # Check if we need to close blocks due to dedent
            # EXCEPT when we're at an else clause (which continues the if block)
            if not is_else_clause:
                while block_stack and block_stack[-1][1] >= indent_level:
                    block_type, block_indent = block_stack.pop()
                    # Add end statement with proper indentation
                    result_lines.append(' ' * block_indent + f'end {block_type}')

            # Detect block openings
            if re.match(r'\bif\b.*\bthen\b', stripped):
                result_lines.append(line)
                block_stack.append(('if', indent_level))
            elif re.match(r'\bfor\b.*\bto\b', stripped):
                result_lines.append(line)
                block_stack.append(('for', indent_level))
            elif re.match(r'\bwhile\b', stripped) and not stripped.startswith('end'):
                result_lines.append(line)
                block_stack.append(('while', indent_level))
            elif stripped.startswith('else if'):
                # else if: DON'T close previous if, it continues
                result_lines.append(line)
                # No stack change - else if is part of the same if block
            elif stripped == 'else':
                # else: DON'T close previous if, it continues
                result_lines.append(line)
                # No stack change - else is part of the same if block
            elif stripped == '}' or stripped == 'end':
                # Generic closing - replace with explicit terminator
                if block_stack:
                    block_type, block_indent = block_stack.pop()
                    # Use block's indentation, not current line's indentation
                    result_lines.append(' ' * block_indent + f'end {block_type}')
                # else: skip orphaned } (already handled by previous block ending)
            else:
                result_lines.append(line)
        
        # Close any remaining open blocks
        while block_stack:
            block_type, block_indent = block_stack.pop()
            result_lines.append(' ' * block_indent + f'end {block_type}')
        
        return '\n'.join(result_lines)
    
    def preprocess(self, source_code: str, 
                   lang_id: Optional[str] = None,
                   syntax_id: Optional[str] = None) -> str:
        """Full two-stage preprocessing pipeline
        
        Args:
            source_code: Original source code
            lang_id: Language ID (auto-detected if None)
            syntax_id: Syntax style ID (auto-detected if None)
        
        Returns:
            MLP Base IR code ready for compilation
        """
        # Auto-detect language and syntax if not specified
        if lang_id is None:
            lang_id = self.detect_language(source_code)
        if syntax_id is None:
            syntax_id = self.detect_syntax(source_code)
        
        # Remove header comments
        source_code = re.sub(r'^\s*(--|//)\s*(lang|dil|syntax):.*$', '', 
                            source_code, flags=re.MULTILINE)
        
        # Stage 1: Keyword translation
        after_stage1 = self.stage1_translate_keywords(source_code, lang_id)
        
        # Stage 2: Syntax transformation
        after_stage2 = self.stage2_transform_syntax(after_stage1, syntax_id)
        
        return after_stage2


def main():
    """CLI interface for preprocessor"""
    if len(sys.argv) < 2:
        print("Usage: mlp_preprocessor.py <input_file> [output_file]")
        print("       mlp_preprocessor.py <input_file> --lang=tr-TR --syntax=c [output_file]")
        print()
        print("Options:")
        print("  --lang=ID      Language ID (e.g., tr-TR, ru-RU, en-US)")
        print("  --syntax=ID    Syntax style (e.g., c, python, rust)")
        print()
        sys.exit(1)
    
    input_file = sys.argv[1]
    lang_id = None
    syntax_id = None
    output_file = None
    
    # Parse arguments
    for arg in sys.argv[2:]:
        if arg.startswith('--lang='):
            lang_id = arg.split('=')[1]
        elif arg.startswith('--syntax='):
            syntax_id = arg.split('=')[1]
        elif not arg.startswith('--'):
            output_file = arg
    
    # Default output filename
    if output_file is None:
        output_file = str(Path(input_file).with_suffix('')) + '.ir.mlp'
    
    # Read input
    with open(input_file, 'r', encoding='utf-8') as f:
        source_code = f.read()
    
    # Preprocess
    preprocessor = MLPPreprocessor()
    
    detected_lang = preprocessor.detect_language(source_code)
    detected_syntax = preprocessor.detect_syntax(source_code)
    
    if lang_id is None:
        lang_id = detected_lang
    if syntax_id is None:
        syntax_id = detected_syntax
    
    print(f"=== MLP Multi-Language/Multi-Syntax Preprocessor ===")
    print(f"Input:    {input_file}")
    print(f"Output:   {output_file}")
    print(f"Language: {lang_id}")
    print(f"Syntax:   {syntax_id}")
    print()
    
    preprocessed = preprocessor.preprocess(source_code, lang_id, syntax_id)
    
    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(preprocessed)
    
    print(f"✅ Preprocessing complete!")
    print(f"   Stage 1: Keyword translation ({lang_id} → English)")
    print(f"   Stage 2: Syntax transformation ({syntax_id} → MLP Base IR)")
    print(f"   Output: {len(preprocessed)} bytes")


if __name__ == '__main__':
    main()
