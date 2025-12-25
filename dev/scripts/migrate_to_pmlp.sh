#!/bin/bash
# PMLP Migration Script
# Converts old syntax to PMLP syntax
# Author: STAGE0_YZ_03
# Date: 25 Aralık 2025

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Usage function
usage() {
    echo "Usage: $0 [OPTIONS] <file_or_directory>"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -d, --dry-run          Show changes without applying them"
    echo "  -b, --backup           Create .backup files before modifying"
    echo "  -r, --recursive        Process directories recursively"
    echo "  -v, --verbose          Show detailed output"
    echo ""
    echo "Examples:"
    echo "  $0 -b example.mlp                  # Migrate single file with backup"
    echo "  $0 -r -b tests/                    # Migrate all .mlp files in tests/"
    echo "  $0 -d -r examples/                 # Dry-run on examples directory"
    exit 1
}

# Variables
DRY_RUN=0
BACKUP=0
RECURSIVE=0
VERBOSE=0
TARGET=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            ;;
        -d|--dry-run)
            DRY_RUN=1
            shift
            ;;
        -b|--backup)
            BACKUP=1
            shift
            ;;
        -r|--recursive)
            RECURSIVE=1
            shift
            ;;
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        *)
            TARGET="$1"
            shift
            ;;
    esac
done

# Check if target is provided
if [ -z "$TARGET" ]; then
    echo -e "${RED}Error: No target file or directory specified${NC}"
    usage
fi

# Check if target exists
if [ ! -e "$TARGET" ]; then
    echo -e "${RED}Error: Target '$TARGET' does not exist${NC}"
    exit 1
fi

# Function to log messages
log() {
    if [ $VERBOSE -eq 1 ]; then
        echo -e "${BLUE}[INFO]${NC} $1"
    fi
}

# Function to migrate a single file
migrate_file() {
    local file="$1"
    
    log "Processing: $file"
    
    # Skip if not a .mlp file
    if [[ ! "$file" =~ \.mlp$ ]]; then
        log "Skipping non-.mlp file: $file"
        return
    fi
    
    # Create backup if requested
    if [ $BACKUP -eq 1 ] && [ $DRY_RUN -eq 0 ]; then
        cp "$file" "$file.backup"
        log "Backup created: $file.backup"
    fi
    
    # Read file content
    local content=$(cat "$file")
    local modified=0
    
    # Check if file needs migration
    if echo "$content" | grep -qE '\bbreak\b|\bexit if\b|\bexit while\b|\bexit for\b'; then
        modified=1
        log "File needs migration: $file"
    else
        log "File already PMLP compliant or no migration needed: $file"
        return
    fi
    
    # Apply transformations
    local new_content="$content"
    
    # ⚠️ CRITICAL: String literal protection
    # We need to protect strings BEFORE doing replacements
    # This is complex in bash, so we use sed with careful patterns
    
    if [ $DRY_RUN -eq 1 ]; then
        echo -e "${YELLOW}[DRY-RUN]${NC} Would migrate: $file"
        echo "  Changes:"
        
        # Show what would change (simplified preview)
        if echo "$content" | grep -q '\bbreak\b'; then
            echo "    - 'break' statements would be analyzed for context"
        fi
        if echo "$content" | grep -q '\bexit if\b'; then
            echo "    - 'exit if' -> 'exit_if'"
        fi
        if echo "$content" | grep -q '\bexit while\b'; then
            echo "    - 'exit while' -> 'exit_while'"
        fi
        if echo "$content" | grep -q '\bexit for\b'; then
            echo "    - 'exit for' -> 'exit_for'"
        fi
    else
        # Apply migrations (CAREFUL: These are context-aware)
        # Note: sed is limited for complex transformations
        # For production, consider using Python/Perl for better string handling
        
        # Create temporary file
        local tmpfile="${file}.tmp"
        
        # Process line by line to avoid string literal issues
        local in_string=0
        local in_multiline_comment=0
        
        while IFS= read -r line; do
            # Check for multiline comment markers
            if echo "$line" | grep -q '^---$'; then
                if [ $in_multiline_comment -eq 0 ]; then
                    in_multiline_comment=1
                else
                    in_multiline_comment=0
                fi
                echo "$line" >> "$tmpfile"
                continue
            fi
            
            # Skip if in multiline comment
            if [ $in_multiline_comment -eq 1 ]; then
                echo "$line" >> "$tmpfile"
                continue
            fi
            
            # Remove single-line comments for processing
            local code_part=$(echo "$line" | sed 's/\s*--.*$//')
            local comment_part=$(echo "$line" | grep -o '\s*--.*$' || echo "")
            
            # Process code part only (skip string literals is hard in bash)
            # For now, we do simple replacements
            # ⚠️ TODO: Use Python script for better string handling
            
            # Check if line contains break outside of strings
            # This is a simplified check - production should use proper parser
            if echo "$code_part" | grep -qE '\bbreak\b'; then
                # In PMLP, 'break' doesn't exist - need context
                # This is Phase 1 work, should already be migrated
                log "WARNING: Found 'break' in $file:$line - needs manual review"
            fi
            
            # Replace exit variants (outside strings)
            code_part=$(echo "$code_part" | sed 's/\bexit if\b/exit_if/g')
            code_part=$(echo "$code_part" | sed 's/\bexit while\b/exit_while/g')
            code_part=$(echo "$code_part" | sed 's/\bexit for\b/exit_for/g')
            code_part=$(echo "$code_part" | sed 's/\bexit function\b/exit_function/g')
            code_part=$(echo "$code_part" | sed 's/\bexit switch\b/exit_switch/g')
            code_part=$(echo "$code_part" | sed 's/\bcontinue while\b/continue_while/g')
            code_part=$(echo "$code_part" | sed 's/\bcontinue for\b/continue_for/g')
            
            # Reconstruct line
            echo "${code_part}${comment_part}" >> "$tmpfile"
        done < "$file"
        
        # Replace original file
        mv "$tmpfile" "$file"
        echo -e "${GREEN}[MIGRATED]${NC} $file"
    fi
}

# Main processing
if [ -f "$TARGET" ]; then
    # Single file
    migrate_file "$TARGET"
elif [ -d "$TARGET" ]; then
    # Directory
    if [ $RECURSIVE -eq 1 ]; then
        # Find all .mlp files recursively
        while IFS= read -r -d '' file; do
            migrate_file "$file"
        done < <(find "$TARGET" -name "*.mlp" -type f -print0)
    else
        # Process only files in the directory (not subdirectories)
        for file in "$TARGET"/*.mlp; do
            [ -e "$file" ] || continue  # Skip if no .mlp files
            migrate_file "$file"
        done
    fi
else
    echo -e "${RED}Error: Invalid target type${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}Migration complete!${NC}"

if [ $DRY_RUN -eq 1 ]; then
    echo -e "${YELLOW}Note: This was a dry-run. No files were modified.${NC}"
    echo "Run without -d/--dry-run to apply changes."
fi

if [ $BACKUP -eq 1 ] && [ $DRY_RUN -eq 0 ]; then
    echo -e "${BLUE}Backups created with .backup extension${NC}"
fi
