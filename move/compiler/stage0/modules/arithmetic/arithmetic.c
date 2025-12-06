#include "arithmetic.h"
#include "../codegen_context/codegen_context.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

// Free arithmetic expression
void arithmetic_expr_free(ArithmeticExpr* expr) {
    if (!expr) return;
    
    if (expr->left) arithmetic_expr_free(expr->left);
    if (expr->right) arithmetic_expr_free(expr->right);
    if (expr->value) free(expr->value);
    
    // Phase 2.3: Free TTO info
    if (expr->tto_info) free(expr->tto_info);
    
    free(expr);
}

// Arithmetic operations module
// This module only contains operation type enums
// No implementation needed for Stage 0

// ============================================================================
// Phase 2.3: TTO Type Propagation Implementation
// ============================================================================

// Note: tto_infer_numeric_type() is implemented in codegen_context.c

// Propagate types through binary operations
TTOTypeInfo arithmetic_propagate_binary_types(TTOTypeInfo* left, TTOTypeInfo* right, ArithmeticOp op) {
    TTOTypeInfo result = {0};
    result.is_constant = left->is_constant && right->is_constant;
    result.needs_promotion = false;
    
    // Rule 1: If either operand is BIGDECIMAL, result is BIGDECIMAL
    if (left->type == INTERNAL_TYPE_BIGDECIMAL || right->type == INTERNAL_TYPE_BIGDECIMAL) {
        result.type = INTERNAL_TYPE_BIGDECIMAL;
        result.mem_location = MEM_HEAP;
        return result;
    }
    
    // Rule 2: If either operand is DOUBLE, result is DOUBLE
    if (left->type == INTERNAL_TYPE_DOUBLE || right->type == INTERNAL_TYPE_DOUBLE) {
        result.type = INTERNAL_TYPE_DOUBLE;
        result.mem_location = MEM_REGISTER;  // XMM register
        return result;
    }
    
    // Rule 3: Both are INT64
    if (left->type == INTERNAL_TYPE_INT64 && right->type == INTERNAL_TYPE_INT64) {
        // Division always produces DOUBLE (to preserve precision)
        if (op == ARITH_DIV) {
            result.type = INTERNAL_TYPE_DOUBLE;
            result.mem_location = MEM_REGISTER;
            return result;
        }
        
        // For other operations, result is INT64 but may overflow
        result.type = INTERNAL_TYPE_INT64;
        result.mem_location = MEM_REGISTER;
        result.needs_promotion = true;  // May need runtime promotion to BIGDECIMAL
        return result;
    }
    
    // Fallback: BIGDECIMAL for safety
    result.type = INTERNAL_TYPE_BIGDECIMAL;
    result.mem_location = MEM_HEAP;
    return result;
}

// Infer TTO type for arithmetic expression (recursive)
void arithmetic_infer_tto_type(ArithmeticExpr* expr) {
    if (!expr || expr->tto_analyzed) {
        return;  // Already analyzed
    }
    
    // Analyze children first
    if (expr->left) {
        arithmetic_infer_tto_type(expr->left);
    }
    if (expr->right) {
        arithmetic_infer_tto_type(expr->right);
    }
    
    // If both children have TTO info, propagate
    if (expr->left && expr->right && 
        expr->left->tto_analyzed && expr->right->tto_analyzed &&
        expr->left->tto_info && expr->right->tto_info) {
        
        TTOTypeInfo* propagated = malloc(sizeof(TTOTypeInfo));
        *propagated = arithmetic_propagate_binary_types(expr->left->tto_info, expr->right->tto_info, expr->op);
        
        expr->tto_info = propagated;
        expr->tto_analyzed = true;
        expr->needs_overflow_check = (propagated->type == INTERNAL_TYPE_INT64);
    }
}
