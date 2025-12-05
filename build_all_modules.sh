#!/bin/bash
# Build all missing modules

cd /home/pardus/projeler/MLP/MLP/melp/C/stage0/modules

MODULES=(
    "result_option"
    "smart_pointers"
    "regex_pattern"
    "test_framework"
    "attributes"
    "decorator_system"
    "macro_system"
    "const_generics"
    "ownership_system"
    "union_types"
    "unsafe_blocks"
    "ffi"
    "documentation"
    "network_io"
    "package_management"
    "performance"
)

SUCCESS=0
FAILED=0

for module in "${MODULES[@]}"; do
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "🔨 Building: $module"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    
    cd "$module" || continue
    
    # Create proper Makefile with tabs
    cat > Makefile <<'EOF'
CC = gcc
CFLAGS = -Wall -g -O2
TARGET = MODULE_standalone
SOURCES = MODULE_standalone.c MODULE_parser.c MODULE_codegen.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c MODULE.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Testing MODULE..."
	@./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) MODULE_test *.s

.PHONY: all test clean
EOF
    
    # Replace MODULE with actual module name
    sed -i "s/MODULE/${module}/g" Makefile
    
    # Clean and build
    make clean >/dev/null 2>&1
    
    if make 2>&1; then
        if [ -f "${module}_standalone" ]; then
            SIZE=$(ls -lh "${module}_standalone" | awk '{print $5}')
            echo "✅ SUCCESS - Binary: $SIZE"
            ((SUCCESS++))
        else
            echo "❌ FAILED - No binary produced"
            ((FAILED++))
        fi
    else
        echo "❌ FAILED - Compilation error"
        ((FAILED++))
    fi
    
    cd ..
    echo ""
done

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📊 RESULTS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "✅ Success: $SUCCESS"
echo "❌ Failed: $FAILED"
echo "Total: $((SUCCESS + FAILED))"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
