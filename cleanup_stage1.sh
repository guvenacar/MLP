#!/bin/bash
# MLP Stage 1 Cleanup Script
# Removes obsolete directories and prepares clean slate

set -e

echo "=== MLP Stage 1 Cleanup ==="
echo ""

cd /home/pardus/projeler/MLP/MLP

# Backup check
echo "📋 Checking git status..."
git status --short
echo ""

# Ask for confirmation
echo "⚠️  This will DELETE the following directories:"
echo "  - melp_yedek_stage0/"
echo "  - melp_yedek_stage1/"
echo "  - melp_yedek_stage2/"
echo "  - melp_yedek_stage2_final/"
echo "  - melp_yedek_stage3/"
echo "  - melp_monolitik/"
echo "  - melp/MLP/stage1/* (partial work)"
echo ""
echo "These exist in git history and can be recovered if needed."
echo ""
read -p "Continue? (yes/no): " confirm

if [ "$confirm" != "yes" ]; then
    echo "❌ Cleanup cancelled."
    exit 0
fi

echo ""
echo "🗑️  Removing backup directories..."
rm -rf melp_yedek_stage0/
rm -rf melp_yedek_stage1/
rm -rf melp_yedek_stage2/
rm -rf melp_yedek_stage2_final/
rm -rf melp_yedek_stage3/
echo "  ✓ Removed melp_yedek_*"

echo ""
echo "🗑️  Removing monolithic directory..."
rm -rf melp_monolitik/
echo "  ✓ Removed melp_monolitik/"

echo ""
echo "🗑️  Cleaning stage1 partial work..."
rm -f melp/MLP/stage1/lexer.mlp
rm -f melp/MLP/stage1/lexer.s
rm -f melp/MLP/stage1/lexer.o
rm -f melp/MLP/stage1/lexer
rm -f melp/MLP/stage1/test_*
rm -f melp/MLP/stage1/build_compiler.sh
rm -rf melp/MLP/stage1/build/
echo "  ✓ Cleaned melp/MLP/stage1/"

echo ""
echo "✅ Cleanup complete!"
echo ""
echo "📊 Remaining structure:"
du -sh melp/C/stage0/modules/ melp/C/runtime/ 2>/dev/null
echo ""
echo "📄 Documentation files:"
ls -lh *.md | awk '{print $5, $9}'
echo ""
echo "🎯 Next steps:"
echo "  1. Review STAGE1_CLEANUP_AND_ROADMAP.md"
echo "  2. Build melpc2 from Stage 0 modules"
echo "  3. Write lexer.mlp"
echo ""
echo "Commit changes:"
echo "  git add -A"
echo "  git commit -m 'chore: Clean obsolete dirs, prepare Stage 1 rebuild'"
echo "  git push"
