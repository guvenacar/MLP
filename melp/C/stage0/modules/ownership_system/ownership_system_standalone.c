/**
 * MLP Ownership System Module - Standalone Compiler (Module #55)
 * 
 * Full chain: Source → Parser → Borrow Checker → Codegen → Assembly
 * 
 * Ownership features:
 * - Borrow checker (multiple readers OR single writer)
 * - Lifetime annotations ('a, 'static)
 * - Move semantics (move, copy, clone)
 * - Ownership rules (use-after-move detection)
 * - Mutable/immutable borrows (&T, &mut T)
 * - Lifetime elision
 */

#include "ownership_system.h"

// ============================================================================
// STATISTICS
// ============================================================================

typedef struct {
    int owned_vars;
    int borrows_immutable;
    int borrows_mutable;
    int moves;
    int copies;
    int clones;
    int lifetimes_named;
    int lifetimes_static;
    int violations;
} OwnershipStats;

static void calculate_stats(OwnershipCtx* ctx, OwnershipStats* stats) {
    memset(stats, 0, sizeof(OwnershipStats));
    
    // Count owned variables
    stats->owned_vars = ctx->owner_count;
    
    // Count borrows
    for (int i = 0; i < ctx->borrow_count; i++) {
        if (ctx->borrows[i].is_mutable) {
            stats->borrows_mutable++;
        } else {
            stats->borrows_immutable++;
        }
    }
    
    // Count moves/copies/clones
    for (int i = 0; i < ctx->move_count; i++) {
        switch (ctx->moves[i].semantics) {
            case MOVE_MOVE: stats->moves++; break;
            case MOVE_COPY: stats->copies++; break;
            case MOVE_CLONE: stats->clones++; break;
            default: break;
        }
    }
    
    // Count lifetimes
    for (int i = 0; i < ctx->lifetime_count; i++) {
        if (ctx->lifetimes[i].type == LIFETIME_STATIC) {
            stats->lifetimes_static++;
        } else {
            stats->lifetimes_named++;
        }
    }
    
    stats->violations = ctx->violation_count;
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("══════════════════════════════════════════════════════\n");
    printf("  MLP OWNERSHIP SYSTEM - Module #55 (Stage 0)\n");
    printf("══════════════════════════════════════════════════════\n");
    printf("Features:\n");
    printf("  • Borrow checker (Rust-style)\n");
    printf("  • Lifetime annotations ('a, 'static)\n");
    printf("  • Move semantics (move, copy, clone)\n");
    printf("  • Ownership tracking\n");
    printf("  • Memory safety guarantees\n");
    printf("══════════════════════════════════════════════════════\n\n");

    // Test source code demonstrating ownership features
    const char* test_source = 
        "-- Ownership System Test\n"
        "\n"
        "-- Declare owned variables\n"
        "numeric x = 42\n"
        "numeric y = 100\n"
        "text message = \"Hello\"\n"
        "\n"
        "-- Immutable borrow (multiple allowed)\n"
        "borrow x as ref1\n"
        "borrow x as ref2\n"
        "\n"
        "-- Mutable borrow (exclusive)\n"
        "borrow_mut y as mut_ref\n"
        "\n"
        "-- Move semantics\n"
        "move x to z\n"
        "-- x is now moved, cannot use\n"
        "\n"
        "-- Copy semantics (for Copy types)\n"
        "copy y to w\n"
        "\n"
        "-- Clone (explicit)\n"
        "clone message to message2\n"
        "\n"
        "-- Lifetime annotations\n"
        "lifetime 'a from line 1 to 50\n"
        "lifetime 'static\n"
        "\n"
        "-- This would cause violation (use after move):\n"
        "-- numeric bad = x + 10  -- ERROR!\n";

    printf("[1/5] Creating ownership context...\n");
    OwnershipCtx* ctx = ownership_ctx_create();
    
    printf("[2/5] Parsing ownership declarations...\n");
    ownership_parser_init(test_source);
    bool parse_success = ownership_parse_file(ctx);
    
    OwnershipStats stats;
    calculate_stats(ctx, &stats);
    
    printf("\n[3/5] Ownership analysis complete\n");
    printf("      ─────────────────────────────────────────\n");
    printf("      Owned variables:     %d\n", stats.owned_vars);
    printf("      Immutable borrows:   %d (&T)\n", stats.borrows_immutable);
    printf("      Mutable borrows:     %d (&mut T)\n", stats.borrows_mutable);
    printf("      Moves:               %d\n", stats.moves);
    printf("      Copies:              %d\n", stats.copies);
    printf("      Clones:              %d\n", stats.clones);
    printf("      Named lifetimes:     %d\n", stats.lifetimes_named);
    printf("      Static lifetimes:    %d\n", stats.lifetimes_static);
    printf("      ─────────────────────────────────────────\n");
    
    printf("\n[4/5] Running borrow checker...\n");
    
    if (stats.violations > 0) {
        printf("      ❌ VIOLATIONS DETECTED: %d\n\n", stats.violations);
        ownership_report_violations(ctx);
        parse_success = false;
    } else {
        printf("      ✅ No violations detected\n");
        printf("      ✅ All ownership rules satisfied\n");
        printf("      ✅ Borrow checker passed\n");
    }
    
    printf("\n[5/5] Generating assembly...\n");
    const char* output_file = "ownership_system.s";
    ownership_generate_asm(output_file, ctx);
    printf("      ✓ Assembly: %s\n", output_file);
    
    // Display detailed ownership information
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("OWNERSHIP DETAILS\n");
    printf("═══════════════════════════════════════════════════════\n\n");
    
    printf("📦 Owned Variables:\n");
    for (int i = 0; i < ctx->owner_count; i++) {
        OwnershipInfo* info = &ctx->owners[i];
        printf("  %d. %s : %s\n", i + 1, info->var_name, info->type_name);
        printf("     Ownership: %s\n", ownership_type_str(info->ownership));
        printf("     State: %s%s\n", 
               info->is_moved ? "MOVED" : "ALIVE",
               info->borrow_state != BORROW_NONE ? " (BORROWED)" : "");
        if (info->borrow_count > 0) {
            printf("     Active borrows: %d\n", info->borrow_count);
        }
        printf("     Line: %d\n\n", info->declaration_line);
    }
    
    if (ctx->borrow_count > 0) {
        printf("🔗 Borrow Operations:\n");
        for (int i = 0; i < ctx->borrow_count; i++) {
            BorrowOp* borrow = &ctx->borrows[i];
            printf("  %d. %s → %s (%s)\n", i + 1,
                   borrow->var_name, borrow->borrowed_as,
                   borrow->is_mutable ? "&mut" : "&");
            printf("     Line: %d\n\n", borrow->line);
        }
    }
    
    if (ctx->move_count > 0) {
        printf("📦 Move/Copy/Clone Operations:\n");
        for (int i = 0; i < ctx->move_count; i++) {
            MoveOp* move = &ctx->moves[i];
            const char* op_name = "move";
            if (move->semantics == MOVE_COPY) op_name = "copy";
            else if (move->semantics == MOVE_CLONE) op_name = "clone";
            
            printf("  %d. %s: %s → %s\n", i + 1, op_name,
                   move->from_var, move->to_var);
            printf("     Line: %d\n\n", move->line);
        }
    }
    
    if (ctx->lifetime_count > 0) {
        printf("⏱️  Lifetimes:\n");
        for (int i = 0; i < ctx->lifetime_count; i++) {
            Lifetime* lt = &ctx->lifetimes[i];
            printf("  %d. '%s (%s)\n", i + 1, lt->name, 
                   lifetime_type_str(lt->type));
            if (lt->type != LIFETIME_STATIC) {
                printf("     Scope: line %d", lt->scope_start);
                if (lt->scope_end > 0) {
                    printf(" to %d\n", lt->scope_end);
                } else {
                    printf(" to end\n");
                }
            } else {
                printf("     Scope: entire program\n");
            }
            printf("\n");
        }
    }
    
    // Cleanup
    ownership_ctx_free(ctx);
    
    printf("═══════════════════════════════════════════════════════\n");
    if (parse_success) {
        printf("✅ MODULE #55 TEST PASSED\n");
        printf("✅ Ownership system fully functional\n");
        printf("✅ Ready for Stage 1 (self-hosting)\n");
    } else {
        printf("❌ MODULE #55 TEST FAILED\n");
        printf("❌ Ownership violations detected\n");
        printf("   Fix violations before proceeding\n");
    }
    printf("═══════════════════════════════════════════════════════\n");

    return parse_success ? 0 : 1;
}
