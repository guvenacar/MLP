#!/bin/bash

# MLP VSCode Extension Installation Script
# This script copies the extension to the VSCode extensions directory

echo "=== MLP Language Extension Installer ==="
echo ""

# Determine VSCode extensions directory
if [[ "$OSTYPE" == "linux-gnu"* ]] || [[ "$OSTYPE" == "darwin"* ]]; then
    EXTENSIONS_DIR="$HOME/.vscode/extensions"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    EXTENSIONS_DIR="$USERPROFILE/.vscode/extensions"
else
    echo "❌ Unsupported operating system: $OSTYPE"
    exit 1
fi

# Create extensions directory if it doesn't exist
mkdir -p "$EXTENSIONS_DIR"

# Copy extension
TARGET_DIR="$EXTENSIONS_DIR/mlp-language-1.0.0"
echo "📦 Installing MLP Language extension..."
echo "   Source: $(pwd)"
echo "   Target: $TARGET_DIR"

# Remove old version if exists
if [ -d "$TARGET_DIR" ]; then
    echo "🗑️  Removing old version..."
    rm -rf "$TARGET_DIR"
fi

# Copy files
cp -r "$(pwd)" "$TARGET_DIR"
echo "✅ Extension copied successfully!"

echo ""
echo "=== Installation Complete! ==="
echo ""
echo "Next steps:"
echo "1. Restart VS Code"
echo "2. Open any .mlp file"
echo "3. Enjoy syntax highlighting in 6 languages! 🎉"
echo ""
echo "Test with:"
echo "  echo 'SAYISAL x = 42; YAZDIR x;' > test.mlp"
echo "  code test.mlp"
