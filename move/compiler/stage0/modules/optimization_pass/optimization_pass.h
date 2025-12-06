#ifndef OPTIMIZATION_PASS_H
#define OPTIMIZATION_PASS_H

#include <stdbool.h>

// Optimization levels
typedef enum {
    OPT_NONE,           // -O0: No optimization
    OPT_BASIC,          // -O1: Basic optimizations
    OPT_MODERATE,       // -O2: Moderate optimizations
    OPT_AGGRESSIVE      // -O3: Aggressive optimizations
} OptimizationLevel;

// Optimization pass types
typedef enum {
    OPT_DEAD_CODE_ELIMINATION,      // Remove unreachable code
    OPT_CONSTANT_FOLDING,            // Evaluate constant expressions at compile time
    OPT_CONSTANT_PROPAGATION,        // Replace variables with their constant values
    OPT_COMMON_SUBEXPRESSION,        // Eliminate redundant calculations
    OPT_REGISTER_ALLOCATION,         // Better register usage
    OPT_PEEPHOLE,                    // Small local optimizations
    OPT_LOOP_UNROLLING,             // Unroll small loops
    OPT_INLINE_FUNCTIONS,            // Inline small functions
    OPT_TAIL_CALL,                   // Tail call optimization
    OPT_STRENGTH_REDUCTION           // Replace expensive ops with cheaper ones
} OptimizationPass;

// Assembly instruction representation
typedef struct AsmInstruction {
    char* opcode;                    // Instruction opcode
    char* operand1;                  // First operand
    char* operand2;                  // Second operand
    char* comment;                   // Instruction comment
    int line_number;                 // Source line number
    bool is_label;                   // Is this a label?
    bool is_dead;                    // Marked for removal?
    struct AsmInstruction* next;     // Next instruction
    struct AsmInstruction* prev;     // Previous instruction
} AsmInstruction;

// Optimization context
typedef struct OptContext {
    AsmInstruction* instructions;    // Instruction list
    OptimizationLevel level;         // Optimization level
    bool* enabled_passes;            // Which passes are enabled
    int pass_count;

    // Statistics
    int instructions_removed;
    int constants_folded;
    int expressions_eliminated;
} OptContext;

// Optimization functions
OptContext* opt_context_create(OptimizationLevel level);
void opt_context_destroy(OptContext* ctx);

// Load and parse assembly
AsmInstruction* opt_load_assembly(const char* filename);
void opt_free_instructions(AsmInstruction* instructions);

// Apply optimization passes
void opt_apply_pass(OptContext* ctx, OptimizationPass pass);
void opt_apply_all_passes(OptContext* ctx);

// Specific optimization passes
void opt_dead_code_elimination(OptContext* ctx);
void opt_constant_folding(OptContext* ctx);
void opt_constant_propagation(OptContext* ctx);
void opt_common_subexpression_elimination(OptContext* ctx);
void opt_peephole(OptContext* ctx);

// Output optimized assembly
void opt_write_assembly(const char* filename, AsmInstruction* instructions);
void opt_print_stats(OptContext* ctx);

#endif
