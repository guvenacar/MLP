#!/usr/bin/env python3
"""
MLP Language Preprocessor (dil_cevirici.py)
Translates multi-language source code to English keywords

Pipeline: Multi-lang Source → English → Compiler
"""

import json
import sys
import re
from pathlib import Path


class LanguagePreprocessor:
    def __init__(self, languages_file="diller.json"):
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
    """CLI interface for preprocessor"""
    if len(sys.argv) < 2:
        print("Usage: dil_cevirici.py <input_file> [output_file]")
        print("       dil_cevirici.py <input_file> --lang=tr-TR [output_file]")
        sys.exit(1)

    input_file = sys.argv[1]
    lang_id = None
    output_file = None

    # Parse arguments
    for arg in sys.argv[2:]:
        if arg.startswith('--lang='):
            lang_id = arg.split('=')[1]
        elif not arg.startswith('--'):
            output_file = arg

    # Default output filename
    if output_file is None:
        output_file = str(Path(input_file).with_suffix('')) + '.preprocessed.mlp'

    # Read input
    with open(input_file, 'r', encoding='utf-8') as f:
        source_code = f.read()

    # Preprocess
    preprocessor = LanguagePreprocessor()

    detected_lang = preprocessor.detect_language(source_code)
    if lang_id is None:
        lang_id = detected_lang

    print(f"=== MLP Language Preprocessor ===")
    print(f"Input:    {input_file}")
    print(f"Output:   {output_file}")
    print(f"Language: {lang_id}")
    print()

    preprocessed = preprocessor.preprocess(source_code, lang_id)

    # Write output
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(preprocessed)

    print(f"Preprocessing complete!")
    print(f"Translated {len(source_code)} → {len(preprocessed)} bytes")


if __name__ == '__main__':
    main()
