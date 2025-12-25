#!/bin/bash
# Toplu Test Düzeltme Script'i
# Yaygın syntax hatalarını otomatik düzeltir

set -e

ROOT_DIR="/home/pardus/projeler/mlp-original"
FAILING_DIR="$ROOT_DIR/tests/unit/failing"
COMPILER="$ROOT_DIR/build/bin/mlpc"
RUNTIME="$ROOT_DIR/compiler/runtime/src/minimal_runtime.c"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "╔═══════════════════════════════════════════════════════════╗"
echo "║        MLP Toplu Test Düzeltme - Automatic Fixer         ║"
echo "╚═══════════════════════════════════════════════════════════╝"
echo ""

FIXED=0
FAILED=0
TOTAL=0

# Pattern 1: int -> numeric dönüşümü
echo "🔍 Pattern 1: int -> numeric dönüşümü..."
echo ""

for file in "$FAILING_DIR"/*.mlp; do
    [ -f "$file" ] || continue
    
    # int keyword içeren dosyaları bul
    if grep -q '\bint\s\+[a-zA-Z_][a-zA-Z0-9_]*\s*=' "$file"; then
        TOTAL=$((TOTAL + 1))
        basename_file=$(basename "$file")
        
        echo -n "  📝 $basename_file ... "
        
        # Yedek al
        cp "$file" "$file.bak"
        
        # Değişiklikleri uygula
        sed -i 's/\bint\s\+\([a-zA-Z_][a-zA-Z0-9_]*\)\s*=\s*\([^;]*\);/numeric \1 = \2/g' "$file"
        sed -i 's/\bint\s\+/numeric /g' "$file"
        
        # Test: Derleme
        if ! "$COMPILER" "$file" /tmp/test_auto.asm >/dev/null 2>&1; then
            echo -e "${RED}❌ Derleme hatası${NC}"
            mv "$file.bak" "$file"
            FAILED=$((FAILED + 1))
            continue
        fi
        
        # Test: Assembly
        if ! nasm -f elf64 /tmp/test_auto.asm -o /tmp/test_auto.o 2>/dev/null; then
            echo -e "${RED}❌ Assembly hatası${NC}"
            mv "$file.bak" "$file"
            FAILED=$((FAILED + 1))
            continue
        fi
        
        # Test: Linking
        if ! gcc -no-pie /tmp/test_auto.o "$RUNTIME" -o /tmp/test_auto_exe -lm 2>/dev/null; then
            echo -e "${RED}❌ Linking hatası${NC}"
            mv "$file.bak" "$file"
            FAILED=$((FAILED + 1))
            continue
        fi
        
        # Test: Execution
        if timeout 2s /tmp/test_auto_exe >/dev/null 2>&1; then
            echo -e "${GREEN}✅ BAŞARILI${NC}"
            mv "$file" "$ROOT_DIR/tests/unit/"
            rm "$file.bak"
            FIXED=$((FIXED + 1))
        else
            echo -e "${YELLOW}⚠️  Runtime hatası${NC}"
            mv "$file.bak" "$file"
            FAILED=$((FAILED + 1))
        fi
        
        # Cleanup
        rm -f /tmp/test_auto.*
    fi
done

echo ""
echo "╔═══════════════════════════════════════════════════════════╗"
echo "║                         Özet                              ║"
echo "╚═══════════════════════════════════════════════════════════╝"
echo ""
echo -e "📊 İşlenen Test:     $TOTAL"
echo -e "${GREEN}✅ Başarılı:        $FIXED${NC}"
echo -e "${RED}❌ Başarısız:       $FAILED${NC}"
echo ""

if [ $FIXED -gt 0 ]; then
    echo "🎉 $FIXED test düzeltildi ve tests/unit/ klasörüne taşındı!"
    echo ""
    echo "Güncel durum:"
    WORKING=$(ls "$ROOT_DIR/tests/unit/"*.mlp 2>/dev/null | wc -l)
    REMAINING=$(find "$FAILING_DIR" -name "*.mlp" | wc -l)
    echo "  ✅ Çalışan: $WORKING / 91"
    echo "  ❌ Kalan: $REMAINING"
fi

echo ""
exit 0
