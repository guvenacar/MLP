#!/usr/bin/env python3
"""
MLP Syntax Preprocessor
Converts alternative syntax styles (C-style, Python-style) to native MLP syntax.

Pipeline: 
    source.mlp → syntax_preprocessor.py → normalized.mlp → language_preprocessor.py → english.mlp → lexer
"""

import sys
import json
import re
from pathlib import Path

class SyntaxPreprocessor:
    def __init__(self, syntax_config_path="syntax.json"):
        """Load syntax configuration"""
        with open(syntax_config_path, 'r', encoding='utf-8') as f:
            self.config = json.load(f)
        
        self.syntaxes = self.config['syntaxes']
        self.detection = self.config['detection']
    
    def detect_syntax(self, source_code):
        """Detect syntax style from source code"""
        # Check for explicit header directive
        header_pattern = r'//\s*syntax:\s*(\S+)'
        match = re.search(header_pattern, source_code, re.IGNORECASE)
        if match:
            syntax_id = match.group(1)
            if syntax_id in self.syntaxes:
                return syntax_id
        
        # Auto-detection
        if self.detection['auto_detect']['enabled']:
            scores = {}
            for rule in self.detection['auto_detect']['rules']:
                pattern = rule['pattern']
                if re.search(pattern, source_code, re.MULTILINE):
                    syntax = rule['syntax']
                    confidence = rule['confidence']
                    scores[syntax] = max(scores.get(syntax, 0), confidence)
            
            if scores:
                best_syntax = max(scores.items(), key=lambda x: x[1])
                if best_syntax[1] > 0.5:  # Minimum confidence threshold
                    return best_syntax[0]
        
        return self.detection['auto_detect']['fallback']
    
    def normalize_c_style(self, source_code):
        """Convert C-style braces to MLP keywords"""
        # Detect source language to use appropriate keywords
        lang_match = re.search(r'(--|//)\s*(lang|dil):\s*(\S+)', source_code, re.IGNORECASE)
        source_lang = lang_match.group(3) if lang_match else 'en-US'
        
        # Language-specific keywords (base language is VB.NET-like English)
        if source_lang.startswith('tr'):
            # Turkish
            block_start_if = 'İSE'
            block_start_while = 'İSE'
            block_end = 'SON'
        elif source_lang.startswith('ru'):
            # Russian
            block_start_if = 'ТО'
            block_start_while = 'ТО'
            block_end = 'КОНЕЦ'
        else:
            # English (VB.NET-like base language)
            block_start_if = 'then'
            block_start_while = 'then'
            block_end = 'endif'  # Will use context-aware: endif, endwhile, endfor
        
        lines = source_code.split('\n')
        output = []
        context_stack = []  # Track what each brace belongs to (IF, WHILE, PROGRAM, etc.)
        
        # Detect source language from the code itself
        source_lang = 'en'  # default
        source_lower = source_code.lower()
        if any(kw in source_lower for kw in ['eğer', 'eger', 'döngü', 'dongu', 'yazdir', 'sayisal', 'sözel', 'metin']):
            source_lang = 'tr'
        
        for line in lines:
            stripped = line.strip()
            
            # Skip only syntax header directive (preserve language header)
            if stripped.startswith('// syntax:'):
                continue
            
            # Remove inline comments (but preserve strings with // and language header)
            if '// dil:' not in line and '// lang:' not in line:
                in_string = False
                clean_line = []
                i = 0
                while i < len(line):
                    if line[i] == '"':
                        in_string = not in_string
                        clean_line.append(line[i])
                    elif line[i:i+2] == '//' and not in_string:
                        # Rest is comment, stop here
                        break
                    else:
                        clean_line.append(line[i])
                    i += 1
                line = ''.join(clean_line)
            
            # Detect context for opening brace (support multiple languages)
            line_upper = line.upper()
            if any(kw in line_upper for kw in ['İSE', 'IF', 'EĞER', 'EGER']):
                context_stack.append('IF')
                # Transform operators (C-style → MLP)
                line = line.replace('==', '=')     # Equality
                line = line.replace('!=', '=/=')   # Not equal
                line = line.replace('&&', ' ve ')  # Logical AND
                line = line.replace('||', ' veya ') # Logical OR
                # Remove parentheses around condition
                line = re.sub(r'\(([^)]+)\)', r'\1', line)
            elif any(kw in line_upper for kw in ['DEĞİLSE', 'DEGILSE', 'ELSE']):
                context_stack.append('ELSE')
            elif any(kw in line_upper for kw in ['DÖNGÜ', 'DONGU', 'WHILE']):
                context_stack.append('WHILE')
                # Transform operators (C-style → MLP)
                line = line.replace('==', '=')
                line = line.replace('!=', '=/=')
                line = line.replace('&&', ' ve ')
                line = line.replace('||', ' veya ')
                # Remove parentheses around condition
                line = re.sub(r'\(([^)]+)\)', r'\1', line)
            elif any(kw in line_upper for kw in ['İÇİN', 'ICIN', 'FOR']):
                context_stack.append('FOR')
            elif 'PROGRAM' in line:
                context_stack.append('PROGRAM')
            elif 'FONKSİYON' in line or 'FUNCTION' in line:
                context_stack.append('FUNCTION')
            elif 'SINIF' in line or 'CLASS' in line:
                context_stack.append('CLASS')
            
            # Replace opening brace with MLP block starter
            # Important: Use keywords in the SAME LANGUAGE as the source code
            # IF blocks need "then/İSE", WHILE blocks don't need anything
            if '{' in line:
                context = context_stack[-1] if context_stack else 'UNKNOWN'
                
                if context == 'IF':
                    # Detect language from the line itself
                    line_lower = line.lower()
                    if any(kw in line_lower for kw in ['eğer', 'eger']):
                        # Turkish keywords → use Turkish block starter
                        line = line.replace('{', 'İSE')
                    else:
                        # English keywords → use English block starter
                        line = line.replace('{', 'then')
                elif context == 'WHILE':
                    # WHILE blocks don't need "then" keyword, just remove the brace
                    line = line.replace('{', '')
                else:
                    # For other contexts (PROGRAM, FUNCTION, CLASS), just remove brace
                    line = line.replace('{', '')
            
            # Replace closing brace with block terminator (language-aware)
            # Replace closing brace with language-appropriate block end keyword
            if '}' in line:
                context = context_stack[-1] if context_stack else 'UNKNOWN'
                
                # Context-aware block end keywords (VB.NET-like for English)
                if source_lang.startswith('tr'):
                    # Turkish: always SON
                    block_end_keyword = 'SON'
                elif source_lang.startswith('ru'):
                    # Russian: context-aware
                    if context == 'IF':
                        block_end_keyword = 'КОНЕЦ_ЕСЛИ'
                    elif context == 'WHILE':
                        block_end_keyword = 'КОНЕЦ_ПОКА'
                    elif context == 'FOR':
                        block_end_keyword = 'КОНЕЦ_ДЛЯ'
                    else:
                        block_end_keyword = 'КОНЕЦ'
                else:
                    # English: context-aware (VB.NET-like)
                    if context == 'IF':
                        block_end_keyword = 'endif'
                    elif context == 'WHILE':
                        block_end_keyword = 'endwhile'
                    elif context == 'FOR':
                        block_end_keyword = 'endfor'
                    else:
                        block_end_keyword = 'end'
                
                if context_stack:
                    context_stack.pop()
                
                # Remove semicolon if it's on the same line as closing brace
                line = line.replace('};', block_end_keyword).replace('}', block_end_keyword)            # MLP syntax rules for semicolons:
            # - REQUIRED after variable declarations: int x = 42;
            # - FORBIDDEN after function calls: print("hello"); ← remove ;
            # - FORBIDDEN after assignments inside blocks: y = y + 1; ← remove ;
            if ';' in line:
                line_stripped = line.strip()
                
                # Check if this is a variable declaration (starts with type keyword)
                is_var_declaration = any(
                    line_stripped.startswith(kw) for kw in 
                    ['int', 'sayisal', 'SAYISAL', 'string', 'sözel', 'SÖZEL', 'metin', 'METIN', 
                     'bool', 'zitlik', 'ZITLIK']
                )
                
                # Check if this is a function call (contains function name followed by parenthesis)
                is_function_call = any(
                    fn in line_stripped.lower() for fn in 
                    ['yazdir', 'print', 'oku', 'read', 'dosya_ac', 'dosya_oku', 'dosya_kapat']
                )
                
                # Check if this is an assignment (but NOT a declaration)
                is_assignment = '=' in line and not is_var_declaration
                
                # Remove semicolons from function calls and assignments
                if is_function_call or is_assignment:
                    line = line.replace(';', '')
                # Keep semicolons in variable declarations (MLP requires them)
            
            output.append(line)
        
        return '\n'.join(output)
    
    def normalize_python_style(self, source_code):
        """Convert Python-style indentation to MLP keywords"""
        lines = source_code.split('\n')
        output = []
        indent_stack = [0]  # Track indentation levels
        context_stack = []  # Track what each indent belongs to
        
        for i, line in enumerate(lines):
            # Skip only syntax header directive (preserve language header)
            if line.strip().startswith('// syntax:'):
                continue
            
            # Remove inline comments (but preserve strings with // and language header)
            if '// dil:' not in line and '// lang:' not in line:
                in_string = False
                clean_line = []
                j = 0
                while j < len(line):
                    if line[j] == '"':
                        in_string = not in_string
                        clean_line.append(line[j])
                    elif line[j:j+2] == '//' and not in_string:
                        # Rest is comment, stop here
                        break
                    else:
                        clean_line.append(line[j])
                    j += 1
                line = ''.join(clean_line)
            
            # Calculate indentation
            indent = len(line) - len(line.lstrip())
            stripped = line.strip()
            
            # Detect context for colon
            if stripped.endswith(':'):
                line = line.rstrip(':')
                
                if 'İSE' in line or 'IF' in line:
                    context_stack.append('IF')
                    # No additional marker needed - just İSE
                elif 'DEĞİLSE' in line or 'ELSE' in line:
                    context_stack.append('ELSE')
                elif 'DÖNGÜ' in line or 'WHILE' in line:
                    context_stack.append('WHILE')
                    # No additional marker needed - just DÖNGÜ
                elif 'İÇİN' in line or 'FOR' in line:
                    context_stack.append('FOR')
                elif 'PROGRAM' in line:
                    context_stack.append('PROGRAM')
                elif 'FONKSİYON' in line or 'FUNCTION' in line:
                    context_stack.append('FUNCTION')
                elif 'SINIF' in line or 'CLASS' in line:
                    context_stack.append('CLASS')
                
                indent_stack.append(indent)
            
            output.append(line)
            
            # Check for dedent on next line
            if i + 1 < len(lines):
                next_line = lines[i + 1]
                next_indent = len(next_line) - len(next_line.lstrip())
                
                # If dedent, add SON
                while indent_stack and next_indent < indent_stack[-1]:
                    indent_stack.pop()
                    if context_stack:
                        context_stack.pop()
                    output.append(' ' * next_indent + 'SON')
        
        # Close any remaining open blocks
        while len(indent_stack) > 1:
            indent_stack.pop()
            if context_stack:
                context_stack.pop()
            output.append('SON')
        
        return '\n'.join(output)
    
    def normalize(self, source_code):
        """Convert source code to normalized MLP syntax"""
        syntax_id = self.detect_syntax(source_code)
        
        print(f"[Syntax Preprocessor] Detected syntax: {syntax_id}", file=sys.stderr)
        
        if syntax_id == 'mlp-default':
            # Already in MLP syntax, just remove header if present
            return re.sub(r'//\s*syntax:.*\n', '', source_code)
        elif syntax_id == 'c-style':
            return self.normalize_c_style(source_code)
        elif syntax_id == 'python-style':
            return self.normalize_python_style(source_code)
        else:
            raise ValueError(f"Unknown syntax style: {syntax_id}")
    
    def process_file(self, input_path, output_path):
        """Process a single file"""
        with open(input_path, 'r', encoding='utf-8') as f:
            source_code = f.read()
        
        normalized = self.normalize(source_code)
        
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(normalized)
        
        print(f"[Syntax Preprocessor] {input_path} → {output_path}", file=sys.stderr)

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 syntax_preprocessor.py <input.mlp> [output.mlp]")
        print("   or: python3 syntax_preprocessor.py <input.mlp> (prints to stdout)")
        sys.exit(1)
    
    input_path = sys.argv[1]
    output_path = sys.argv[2] if len(sys.argv) > 2 else None
    
    # Find syntax.json
    syntax_json = Path(__file__).parent.parent / "syntax.json"
    
    preprocessor = SyntaxPreprocessor(syntax_json)
    
    if output_path:
        preprocessor.process_file(input_path, output_path)
    else:
        # Print to stdout
        with open(input_path, 'r', encoding='utf-8') as f:
            source_code = f.read()
        print(preprocessor.normalize(source_code))

if __name__ == "__main__":
    main()
