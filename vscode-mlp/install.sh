#!/bin/bash

# MLP VSCode Extension Installation Script v1.1.0
# This script installs the extension via VSIX or direct copy

echo "🚀 MLP VSCode Extension Installer v1.1.0"
echo "========================================"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Determine VSCode extensions directory
if [[ "$OSTYPE" == "linux-gnu"* ]] || [[ "$OSTYPE" == "darwin"* ]]; then
    EXTENSIONS_DIR="$HOME/.vscode/extensions"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    EXTENSIONS_DIR="$USERPROFILE/.vscode/extensions"
else
    echo -e "${RED}❌ Unsupported operating system: $OSTYPE${NC}"
    exit 1
fi

# Check if VSCode is installed
if ! command -v code &> /dev/null; then
    echo -e "${RED}❌ VSCode is not installed or not in PATH${NC}"
    echo "Please install VSCode first: https://code.visualstudio.com/"
    exit 1
fi

echo -e "${GREEN}✅ VSCode found${NC}"

# Method selection
echo ""
echo "Choose installation method:"
echo "1) Install via VSIX package (Recommended)"
echo "2) Install by copying extension files"
read -p "Enter choice (1 or 2): " choice

case $choice in
    1)
        echo ""
        echo "📦 Creating VSIX package..."
        
        # Check if vsce is installed
        if ! command -v vsce &> /dev/null; then
            echo -e "${YELLOW}⚠️  vsce not found, installing...${NC}"
            npm install -g vsce
            
            if [ $? -ne 0 ]; then
                echo -e "${RED}❌ Failed to install vsce${NC}"
                exit 1
            fi
        fi
        
        # Create VSIX package
        vsce package
        
        if [ $? -ne 0 ]; then
            echo -e "${RED}❌ Failed to create VSIX package${NC}"
            exit 1
        fi
        
        # Install VSIX
        echo "📥 Installing extension..."
        code --install-extension mlp-language-1.1.0.vsix
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✅ Extension installed successfully!${NC}"
        else
            echo -e "${RED}❌ Failed to install extension${NC}"
            exit 1
        fi
        ;;
    2)
        echo ""
        echo "📂 Copying extension files..."
        
        # Create extensions directory if it doesn't exist
        mkdir -p "$EXTENSIONS_DIR"
        
        # Copy extension
        TARGET_DIR="$EXTENSIONS_DIR/mlp-language-1.1.0"
        echo "   Source: $(pwd)"
        echo "   Target: $TARGET_DIR"
        
        # Remove old version if exists
        if [ -d "$TARGET_DIR" ]; then
            echo "🗑️  Removing old version..."
            rm -rf "$TARGET_DIR"
        fi
        
        # Copy files
        cp -r "$(pwd)" "$TARGET_DIR"
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✅ Extension files copied successfully!${NC}"
            echo "📍 Installed to: $TARGET_DIR"
        else
            echo -e "${RED}❌ Failed to copy extension files${NC}"
            exit 1
        fi
        ;;
    *)
        echo -e "${RED}❌ Invalid choice${NC}"
        exit 1
        ;;
esac

# Summary
echo ""
echo "🎉 Installation Complete!"
echo "========================"
echo ""
echo "📝 Next steps:"
echo "  1. Restart VSCode"
echo "  2. Open any .mlp file"
echo "  3. Try typing 'mlp-hello' and press Tab"
echo "  4. Check templates in: $PWD/templates/"
echo ""
echo "📚 Quick Start Guide: QUICKSTART.md"
echo "📖 Documentation: README.md"
echo ""
echo -e "${GREEN}Happy coding in MLP! 🚀${NC}"
