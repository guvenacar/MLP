#!/bin/bash
# MLP Compiler Wrapper - 3-Stage Pipeline
# Usage: ./mlpc_wrapper.sh input.mlp -o output

set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Parse arguments
INPUT_FILE=""
OUTPUT_FILE="a.out"
VERBOSE=0
KEEP_TEMP=0

while [[ $# -gt 0 ]]; do
    case $1 in
        -o|--output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        --keep-temp)
            KEEP_TEMP=1
            shift
            ;;
        -h|--help)
            echo "MLP Compiler - 3-Stage Pipeline"
            echo ""
            echo "Usage: $0 input.mlp [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -o, --output FILE    Output executable name (default: a.out)"
            echo "  -v, --verbose        Show detailed compilation steps"
            echo "  --keep-temp          Keep intermediate files"
            echo "  -h, --help           Show this help"
            echo ""
            echo "Pipeline:"
            echo "  Stage 1: Syntax Normalization (C-style/Python → MLP base)"
            echo "  Stage 2: Language Translation (Turkish/Russian → English)"
            echo "  Stage 3: Compilation (English → Assembly → Binary)"
            exit 0
            ;;
        *)
            if [[ -z "$INPUT_FILE" ]]; then
                INPUT_FILE="$1"
            else
                echo -e "${RED}Error: Unknown argument: $1${NC}"
                exit 1
            fi
            shift
            ;;
    esac
done

# Validate input
if [[ -z "$INPUT_FILE" ]]; then
    echo -e "${RED}Error: No input file specified${NC}"
    echo "Usage: $0 input.mlp -o output"
    exit 1
fi

if [[ ! -f "$INPUT_FILE" ]]; then
    echo -e "${RED}Error: Input file not found: $INPUT_FILE${NC}"
    exit 1
fi

# Create temp directory
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

# Get absolute path of input file and its directory
INPUT_ABS_PATH="$(cd "$(dirname "$INPUT_FILE")" && pwd)/$(basename "$INPUT_FILE")"
INPUT_DIR="$(dirname "$INPUT_ABS_PATH")"

# Export source directory for import resolution
export MLP_SOURCE_DIR="$INPUT_DIR"

BASENAME=$(basename "$INPUT_FILE" .mlp)
TEMP_NORMALIZED="$TEMP_DIR/${BASENAME}_normalized.mlp"
TEMP_ENGLISH="$TEMP_DIR/${BASENAME}_english.mlp"
TEMP_ASM="$TEMP_DIR/${BASENAME}.asm"
TEMP_OBJ="$TEMP_DIR/${BASENAME}.o"

# Stage 1: Syntax Normalization
if [[ $VERBOSE -eq 1 ]]; then
    echo -e "${BLUE}[Stage 1/3] Syntax Normalization${NC}"
fi

python3 "$SCRIPT_DIR/stage0/syntax_preprocessor.py" "$INPUT_FILE" "$TEMP_NORMALIZED"

if [[ $? -ne 0 ]]; then
    echo -e "${RED}Error: Syntax normalization failed${NC}"
    exit 1
fi

if [[ $VERBOSE -eq 1 ]]; then
    echo -e "${GREEN}✓ Syntax normalized: $TEMP_NORMALIZED${NC}"
fi

# Stage 2: Language Translation
if [[ $VERBOSE -eq 1 ]]; then
    echo -e "${BLUE}[Stage 2/3] Language Translation${NC}"
fi

python3 "$SCRIPT_DIR/stage0/mlp_preprocessor.py" "$TEMP_NORMALIZED" "$TEMP_ENGLISH"

if [[ $? -ne 0 ]]; then
    echo -e "${RED}Error: Language translation failed${NC}"
    exit 1
fi

if [[ $VERBOSE -eq 1 ]]; then
    echo -e "${GREEN}✓ Translated to English: $TEMP_ENGLISH${NC}"
fi

# Stage 3: Compilation
if [[ $VERBOSE -eq 1 ]]; then
    echo -e "${BLUE}[Stage 3/3] Compilation${NC}"
fi

"$SCRIPT_DIR/mlp_compiler" "$TEMP_ENGLISH" "$TEMP_ASM"

if [[ $? -ne 0 ]]; then
    echo -e "${RED}Error: Compilation failed${NC}"
    exit 1
fi

if [[ $VERBOSE -eq 1 ]]; then
    echo -e "${GREEN}✓ Assembly generated: $TEMP_ASM${NC}"
fi

# Assemble with NASM
if [[ $VERBOSE -eq 1 ]]; then
    echo -e "${BLUE}[Assembling]${NC}"
fi

nasm -f elf64 "$TEMP_ASM" -o "$TEMP_OBJ"

if [[ $? -ne 0 ]]; then
    echo -e "${RED}Error: Assembly failed${NC}"
    exit 1
fi

# Link with runtime
if [[ $VERBOSE -eq 1 ]]; then
    echo -e "${BLUE}[Linking]${NC}"
fi

gcc "$TEMP_OBJ" \
    "$SCRIPT_DIR/runtime/runtime.c" \
    "$SCRIPT_DIR/runtime/hashmap.c" \
    "$SCRIPT_DIR/runtime/json_parser.c" \
    -o "$OUTPUT_FILE" \
    -lm -no-pie

if [[ $? -ne 0 ]]; then
    echo -e "${RED}Error: Linking failed${NC}"
    exit 1
fi

# Success
echo -e "${GREEN}✓ Successfully compiled: $OUTPUT_FILE${NC}"

# Keep temp files if requested
if [[ $KEEP_TEMP -eq 1 ]]; then
    cp "$TEMP_NORMALIZED" "${BASENAME}_normalized.mlp"
    cp "$TEMP_ENGLISH" "${BASENAME}_english.mlp"
    cp "$TEMP_ASM" "${BASENAME}.asm"
    echo -e "${BLUE}Intermediate files saved:${NC}"
    echo "  ${BASENAME}_normalized.mlp"
    echo "  ${BASENAME}_english.mlp"
    echo "  ${BASENAME}.asm"
fi
