#!/usr/bin/env python3
"""
MLP Normalizer - Multi-language to Base Syntax Converter
Converts user code (Turkish, English, etc.) to MLP base syntax
"""

import json
import sys
import re
from pathlib import Path

class MLPNormalizer:
    def __init__(self, config_dir="config"):
        self.config_dir = Path(config_dir)
        self.load_configs()
    
    def load_configs(self):
        """Load language and syntax configurations"""
        # Load language mappings
        diller_path = self.config_dir / "diller.json"
        with open(diller_path, 'r', encoding='utf-8') as f:
            self.languages = json.load(f)
        
        # Load syntax rules
        syntax_path = self.config_dir / "syntax.json"
        with open(syntax_path, 'r', encoding='utf-8') as f:
            self.syntax = json.load(f)
    
    def detect_language(self, code):
        """Auto-detect source language"""
        # Check for Turkish keywords
        tr_keywords = ["işlev", "eğer", "döngü", "değilse", "yap"]
        for kw in tr_keywords:
            if kw in code:
                return "tr"
        
        # Default to English
        return "en"
    
    def normalize(self, code, source_lang=None):
        """
        Normalize code from source language to base syntax
        
        Args:
            code: Source code string
            source_lang: Source language ('tr', 'en', etc.) or None for auto-detect
        
        Returns:
            Normalized code string (English base syntax)
        """
        if source_lang is None:
            source_lang = self.detect_language(code)
        
        # Get keyword mapping for source language
        if source_lang not in self.languages:
            raise ValueError(f"Unsupported language: {source_lang}")
        
        lang_keywords = self.languages[source_lang]
        base_keywords = self.languages["en"]  # Base syntax is English
        
        # Build reverse mapping: source_lang → base
        keyword_map = {}
        for base_key, base_value in base_keywords.items():
            source_value = lang_keywords.get(base_key)
            if source_value and source_value != base_value:
                keyword_map[source_value] = base_value
        
        # Normalize: Replace source keywords with base keywords
        normalized = code
        
        # Use word boundary regex to avoid partial matches
        for source_kw, base_kw in keyword_map.items():
            # Pattern: match whole word (not part of identifier)
            pattern = r'\b' + re.escape(source_kw) + r'\b'
            normalized = re.sub(pattern, base_kw, normalized)
        
        return normalized
    
    def normalize_file(self, input_path, output_path, source_lang=None):
        """Normalize entire file"""
        with open(input_path, 'r', encoding='utf-8') as f:
            code = f.read()
        
        normalized = self.normalize(code, source_lang)
        
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(normalized)
        
        print(f"Normalized: {input_path} → {output_path}")
        print(f"  Language: {source_lang or 'auto-detected'}")
        print(f"  Size: {len(code)} → {len(normalized)} bytes")


def main():
    """Command-line interface"""
    if len(sys.argv) < 3:
        print("Usage: python mlp_normalize.py <input.mlp> <output.mlp> [language]")
        print("  language: 'tr' (Turkish), 'en' (English), or omit for auto-detect")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    lang = sys.argv[3] if len(sys.argv) > 3 else None
    
    # Find config directory
    script_dir = Path(__file__).parent
    config_dir = script_dir.parent.parent / "config"
    
    if not config_dir.exists():
        # Try relative path
        config_dir = Path("config")
    
    if not config_dir.exists():
        print(f"Error: Config directory not found: {config_dir}")
        sys.exit(1)
    
    normalizer = MLPNormalizer(config_dir)
    normalizer.normalize_file(input_file, output_file, lang)


if __name__ == "__main__":
    main()
