#!/usr/bin/env python3
"""
MLP Multi-Language & Multi-Syntax Preprocessor (dil_cevirici.py)

Pipeline:
1. Syntax Transform (C/Python/JS → MLP base)
2. Keyword Translation (Turkish/Russian/etc → English)

Usage:
  dil_cevirici.py input.mlp [output.mlp]
  dil_cevirici.py input.mlp --lang=tr-TR --syntax=c [output.mlp]
"""

import json
import sys
import re
from pathlib import Path


class SyntaxTransformer:
    """Transforms various programming syntax styles to MLP base syntax"""

    def __init__(self, syntax_file="syntax_comprehensive.json"):
        """Initialize syntax transformer"""
        with open(syntax_file, 'r', encoding='utf-8') as f:
            data = json.load(f)

        self.syntax_styles = {}
        for style in data['syntax_styles']:
            self.syntax_styles[style['id']] = style

        self.detection_rules = data.get('syntax_detection', {}).get('detection_rules', [])
        self.default_syntax = data.get('default_syntax', 'mlp')

    def detect_syntax(self, source_code):
        """Auto-detect syntax style from code patterns

        Looks for:
        1. Header directive: -- syntax: c
        2. Pattern matching: if (x > 0) { → c-style

        Returns: syntax_id (e.g., 'c', 'python', 'mlp')
        """
        # Check for explicit header
        match = re.search(r'--\s*syntax:\s*(\S+)', source_code)
        if match:
            return match.group(1)

        # Auto-detect from patterns
        scores = {rule['syntax_id']: 0 for rule in self.detection_rules}

        for rule in self.detection_rules:
            for indicator in rule['indicators']:
                if indicator in source_code:
                    scores[rule['syntax_id']] += 1

        # Find syntax with highest score above threshold
        for rule in self.detection_rules:
            syntax_id = rule['syntax_id']
            threshold = rule.get('confidence_threshold', 2)

            if scores[syntax_id] >= threshold:
                return syntax_id

        # Return default
        for rule in self.detection_rules:
            if rule.get('is_default', False):
                return rule['syntax_id']

        return self.default_syntax

    def transform(self, source_code, syntax_id=None):
        """Transform source code from given syntax to MLP base syntax

        Args:
            source_code: Source code string
            syntax_id: Syntax style ID (e.g., 'c'), auto-detected if None

        Returns:
            Transformed code in MLP base syntax
        """
        # Auto-detect if not specified
        if syntax_id is None:
            syntax_id = self.detect_syntax(source_code)

        # If already in MLP syntax, return as-is
        if syntax_id == 'mlp':
            return source_code

        # Get syntax style
        if syntax_id not in self.syntax_styles:
            print(f"Warning: Unknown syntax '{syntax_id}', using mlp")
            return source_code

        style = self.syntax_styles[syntax_id]
        transformations = style.get('transformations', {})

        result = source_code

        # Apply transformations using rules array from comprehensive JSON
        rules = transformations.get('rules', [])
        result = self._apply_transformation_rules(result, rules, syntax_id)

        return result

    def _apply_transformation_rules(self, code, rules, syntax_id):
        """Apply transformation rules from comprehensive JSON

        Args:
            code: Source code string
            rules: List of transformation rules from JSON
            syntax_id: Syntax style ID for special handling

        Returns:
            Transformed code
        """
        result = code

        # Apply each rule in order
        for rule in rules:
            pattern = rule.get('pattern', '')
            replacement = rule.get('replacement', '')
            name = rule.get('name', '')

            if not pattern:
                continue

            # Apply regex replacement
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Special handling for Python - add 'end' keywords at dedent
        if syntax_id == 'python':
            result = self._add_end_keywords_python(result)

        return result

    def _transform_c_style(self, code, transformations):
        """Transform C-style syntax to MLP"""
        result = code

        # IMPORTANT: Do transformations in correct order!

        # 1. Transform: } else { → else (before removing braces)
        result = re.sub(r'\}\s*else\s*\{', 'else', result, flags=re.MULTILINE)

        # 2. Transform: if (condition) { → if condition then
        # Remove parentheses around condition and replace { with then
        result = re.sub(r'\bif\s*\(([^)]+)\)\s*\{', r'if \1 then', result, flags=re.MULTILINE)

        # 3. Also handle if without opening brace (multiline)
        # if (condition) \n → if condition then \n
        result = re.sub(r'\bif\s*\(([^)]+)\)\s*$', r'if \1 then', result, flags=re.MULTILINE)

        # 4. Transform: while (condition) { → while
        # MLP while doesn't have condition in syntax, it's infinite loop with break
        result = re.sub(r'\bwhile\s*\(([^)]+)\)\s*\{', r'while', result, flags=re.MULTILINE)
        result = re.sub(r'\bwhile\s*\{', 'while', result, flags=re.MULTILINE)

        # 5. Transform: for loops → while loops (MLP doesn't have for)
        result = re.sub(r'\bfor\s*\([^)]+\)\s*\{', 'while', result, flags=re.MULTILINE)

        # 6. Transform: function name(params) { → function name(params) then
        result = re.sub(r'\bfunction\s+(\w+)\s*\(([^)]*)\)\s*\{', r'function \1(\2) then', result, flags=re.MULTILINE)

        # 7. Transform: print(expr) → print expr
        # Handle multiple forms: print(...), printf(...), println(...)
        result = re.sub(r'\bprint\w*\s*\(([^)]+)\)', r'print \1', result, flags=re.MULTILINE)

        # 8. Remove semicolons at end of lines
        result = re.sub(r';\s*$', '', result, flags=re.MULTILINE)

        # 9. NOW transform all closing braces to 'end'
        # Match standalone } on a line (with optional whitespace)
        result = re.sub(r'^\s*\}\s*$', 'end', result, flags=re.MULTILINE)

        # 10. Remove any remaining standalone { or }
        result = re.sub(r'^\s*\{\s*$', '', result, flags=re.MULTILINE)
        result = re.sub(r'\{\s*$', '', result, flags=re.MULTILINE)

        return result

    def _transform_python_style(self, code, transformations):
        """Transform Python-style syntax to MLP"""
        result = code

        # Transform: if condition: → if condition then
        if 'if' in transformations:
            pattern = transformations['if']['from']
            replacement = transformations['if']['to']
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Transform: else: → else
        if 'else' in transformations:
            pattern = transformations['else']['from']
            replacement = transformations['else']['to']
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Transform: while condition: → while
        if 'while' in transformations:
            pattern = transformations['while']['from']
            replacement = transformations['while']['to']
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Transform: def name(params): → function name(params) then
        if 'function' in transformations:
            pattern = transformations['function']['from']
            replacement = transformations['function']['to']
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Transform: print(expr) → print expr
        if 'print' in transformations:
            pattern = transformations['print']['from']
            replacement = transformations['print']['to']
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Add 'end' keywords at dedent points
        # This is simplified - real implementation needs proper indentation tracking
        result = self._add_end_keywords_python(result)

        return result

    def _transform_javascript_style(self, code, transformations):
        """Transform JavaScript-style syntax to MLP"""
        result = code

        # Transform: console.log(expr) → print expr
        if 'console_log' in transformations:
            pattern = transformations['console_log']['from']
            replacement = transformations['console_log']['to']
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Rest is similar to C-style
        result = self._transform_c_style(result, transformations)

        return result

    def _transform_ruby_style(self, code, transformations):
        """Transform Ruby-style syntax to MLP"""
        result = code

        # Transform: puts expr → print expr
        if 'puts' in transformations:
            pattern = transformations['puts']['from']
            replacement = transformations['puts']['to']
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Transform: def name(params) → function name(params) then
        if 'def' in transformations:
            pattern = transformations['def']['from']
            replacement = transformations['def']['to']
            result = re.sub(pattern, replacement, result, flags=re.MULTILINE)

        # Ruby already uses 'end', so no closing transformation needed

        return result

    def _add_end_keywords_python(self, code):
        """Add 'end' keywords for Python indentation-based blocks

        Simplified implementation - tracks indentation levels
        """
        lines = code.split('\n')
        result_lines = []
        indent_stack = [0]  # Stack of indentation levels

        for i, line in enumerate(lines):
            # Skip empty lines
            if not line.strip():
                result_lines.append(line)
                continue

            # Calculate indentation
            stripped = line.lstrip()
            if not stripped:
                result_lines.append(line)
                continue

            indent = len(line) - len(stripped)

            # Check if this is a dedent
            while indent < indent_stack[-1]:
                indent_stack.pop()
                # Add 'end' keyword at previous indentation level
                result_lines.append(' ' * indent_stack[-1] + 'end')

            # Check if line ends with colon (starts new block)
            if stripped.rstrip().endswith(':'):
                # Remove colon, it's already transformed
                indent_stack.append(indent + 4)  # Assume 4-space indent

            result_lines.append(line)

        # Close any remaining blocks
        while len(indent_stack) > 1:
            indent_stack.pop()
            result_lines.append(' ' * indent_stack[-1] + 'end')

        return '\n'.join(result_lines)


class LanguagePreprocessor:
    """Translates keywords from native language to English"""

    def __init__(self, languages_file="diller_comprehensive.json"):
        """Initialize preprocessor with language definitions"""
        with open(languages_file, 'r', encoding='utf-8') as f:
            data = json.load(f)

        self.languages = {}
        for lang in data['languages']:
            self.languages[lang['id']] = lang

        self.default_lang = data.get('default_language', 'en-US')

    def detect_language(self, source_code):
        """Detect language from header comment

        Looks for: -- lang: tr-TR
        """
        match = re.search(r'--\s*lang:\s*(\S+)', source_code)
        if match:
            return match.group(1)
        return self.default_lang

    def build_translation_map(self, lang_id):
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

    def preprocess(self, source_code, lang_id=None):
        """Translate source code from native language to English

        Args:
            source_code: Source code string
            lang_id: Language ID (e.g., 'tr-TR'), auto-detected if None

        Returns:
            Preprocessed source code with English keywords
        """
        # Auto-detect language if not specified
        if lang_id is None:
            lang_id = self.detect_language(source_code)

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

            # Handle comments
            if c == '-' and i + 1 < len(source_code) and source_code[i+1] == '-':
                if state == STATE_CODE:
                    if current_word:
                        word = ''.join(current_word)
                        result.append(translation_map.get(word, word))
                        current_word = []
                    state = STATE_COMMENT
                    result.append('--')
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
            if c in ' \t\n;(){}[]<>=!+-*/,':
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


def main():
    """CLI interface for preprocessor with syntax and language support"""
    if len(sys.argv) < 2:
        print("MLP Multi-Language & Multi-Syntax Preprocessor")
        print()
        print("Usage:")
        print("  dil_cevirici.py <input_file> [output_file]")
        print("  dil_cevirici.py <input_file> --lang=tr-TR [output_file]")
        print("  dil_cevirici.py <input_file> --syntax=c [output_file]")
        print("  dil_cevirici.py <input_file> --lang=ru-RU --syntax=python [output_file]")
        print()
        print("Supported Languages: tr-TR, ru-RU, zh-CN, hi-IN, en-US")
        print("Supported Syntax: mlp, c, python, javascript, ruby")
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
        output_file = str(Path(input_file).with_suffix('')) + '.preprocessed.mlp'

    # Read input
    with open(input_file, 'r', encoding='utf-8') as f:
        source_code = f.read()

    # Initialize transformers
    syntax_transformer = SyntaxTransformer()
    language_preprocessor = LanguagePreprocessor()

    # Auto-detect
    detected_syntax = syntax_transformer.detect_syntax(source_code)
    detected_lang = language_preprocessor.detect_language(source_code)

    if syntax_id is None:
        syntax_id = detected_syntax
    if lang_id is None:
        lang_id = detected_lang

    print(f"=== MLP Multi-Language & Multi-Syntax Preprocessor ===")
    print(f"Input:    {input_file}")
    print(f"Output:   {output_file}")
    print(f"Language: {lang_id}")
    print(f"Syntax:   {syntax_id}")
    print()

    # STAGE 1: Keyword Translation (multi-lang → English)
    print(f"[1/2] Keyword Translation ({lang_id} → English)...")
    translated = language_preprocessor.preprocess(source_code, lang_id)

    # STAGE 2: Syntax Transform (C/Python/etc → MLP base)
    print(f"[2/2] Syntax Transform ({syntax_id} → MLP base)...")
    preprocessed = syntax_transformer.transform(translated, syntax_id)

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(preprocessed)

    print()
    print(f"Preprocessing complete!")
    print(f"Original:    {len(source_code)} bytes")
    print(f"Translated:  {len(translated)} bytes")
    print(f"Final:       {len(preprocessed)} bytes")


if __name__ == '__main__':
    main()
