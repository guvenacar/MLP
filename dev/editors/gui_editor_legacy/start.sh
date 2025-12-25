#!/bin/bash
# MLP Visual Editor Launcher

echo "🎨 MLP Visual Block Editor"
echo "=========================="
echo ""

# Check Python
if ! command -v python3 &> /dev/null; then
    echo "❌ Python 3 not found. Please install Python 3."
    exit 1
fi

# Check Flask
if ! python3 -c "import flask" 2>/dev/null; then
    echo "📦 Installing Flask..."
    pip3 install -r requirements.txt
fi

# Check compiler
if [ ! -f "../c_compiler/mlp_compiler" ]; then
    echo "⚠️  Compiler not found. Building..."
    cd ../c_compiler
    gcc -c c_lexer.c -o c_lexer.o
    gcc -c c_parser.c -o c_parser.o
    gcc -c c_generator.c -o c_generator.o
    gcc -c main.c -o main.o
    gcc c_lexer.o c_parser.o c_generator.o main.o -o mlp_compiler
    cd ../gui_editor
    echo "✅ Compiler built successfully"
fi

# Start server
echo ""
echo "🚀 Starting MLP Visual Editor..."
echo "📖 Open: http://localhost:5000"
echo ""
echo "Press Ctrl+C to stop"
echo ""

python3 server.py
