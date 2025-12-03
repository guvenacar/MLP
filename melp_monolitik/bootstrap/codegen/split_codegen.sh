#!/bin/bash
# ============================================================================
# MELP Codegen Modularization Script
# Bu script ana codegen.c dosyasını modüllere ayırır
# ============================================================================
# KULLANIM: ./split_codegen.sh
# ============================================================================

set -e

CODEGEN_C="../codegen.c"
OUTPUT_DIR="."

echo "=== MELP Codegen Modularization ==="
echo ""

# Her modül için başlangıç ve bitiş satırlarını bul
extract_module() {
    local module_name=$1
    local output_file=$2
    
    echo "Extracting: $module_name -> $output_file"
    
    # Başlangıç pattern'i: "// MODÜL: $module_name başlangıcı" veya "// MODÜL: $module_name - Başlangıç"
    # Bitiş pattern'i: "// MODÜL: $module_name sonu" veya "// MODÜL: $module_name - Bitiş"
    
    local start_line=$(grep -n "MODÜL: $module_name" "$CODEGEN_C" | grep -i "başlangıcı\|Başlangıç" | head -1 | cut -d: -f1)
    local end_line=$(grep -n "MODÜL: $module_name" "$CODEGEN_C" | grep -i "sonu\|Bitiş" | head -1 | cut -d: -f1)
    
    if [ -z "$start_line" ] || [ -z "$end_line" ]; then
        echo "  WARNING: Could not find boundaries for $module_name"
        return 1
    fi
    
    echo "  Lines: $start_line - $end_line"
    
    # Header ekle
    cat > "$output_file" << EOF
// ============================================================================
// MELP Modular Codegen - $module_name
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

EOF
    
    # Kodu çıkar (yorum satırları hariç)
    sed -n "${start_line},${end_line}p" "$CODEGEN_C" | \
        grep -v "^// MODÜL:" | \
        grep -v "^// ====" >> "$output_file"
    
    echo "  Done: $(wc -l < "$output_file") lines"
}

# Modülleri çıkar
echo "Extracting modules from $CODEGEN_C..."
echo ""

# İlk 10 temel modül
extract_module "codegen_core.c" "codegen_core.c.new" || true
extract_module "codegen_module.c" "codegen_module.c.new" || true  
extract_module "codegen_operator.c" "codegen_operator.c.new" || true
extract_module "codegen_variable.c" "codegen_variable.c.new" || true
extract_module "codegen_type.c" "codegen_type.c.new" || true
extract_module "codegen_emit.c" "codegen_emit.c.new" || true
extract_module "codegen_function.c" "codegen_function.c.new" || true
extract_module "codegen_struct.c" "codegen_struct.c.new" || true
extract_module "codegen_interface.c" "codegen_interface.c.new" || true
extract_module "codegen_prologue.c" "codegen_prologue.c.new" || true

# Declaration ve Assignment
extract_module "codegen_decl.c" "codegen_decl.c.new" || true
extract_module "codegen_assign.c" "codegen_assign.c.new" || true
extract_module "codegen_print.c" "codegen_print.c.new" || true

# Expression (en büyük)
extract_module "codegen_expr.c" "codegen_expr.c.new" || true
extract_module "codegen_compare.c" "codegen_compare.c.new" || true

# Control flow
extract_module "codegen_if.c" "codegen_if.c.new" || true
extract_module "codegen_for.c" "codegen_for.c.new" || true
extract_module "codegen_while.c" "codegen_while.c.new" || true
extract_module "codegen_switch.c" "codegen_switch.c.new" || true

# Function definition ve return
extract_module "codegen_funcdef.c" "codegen_funcdef.c.new" || true
extract_module "codegen_return.c" "codegen_return.c.new" || true

# Statement dispatcher ve main
extract_module "codegen_stmt.c" "codegen_stmt.c.new" || true
extract_module "codegen_main.c" "codegen_main.c.new" || true

echo ""
echo "=== Extraction Complete ==="
echo ""
echo "New files created with .new extension"
echo "Review them and rename to replace originals"
echo ""
echo "To apply:"
echo "  for f in *.new; do mv \"\$f\" \"\${f%.new}\"; done"
