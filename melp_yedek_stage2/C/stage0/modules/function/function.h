#ifndef FUNCTION_H
#define FUNCTION_H

#include "../variable/variable.h"

// Function parameter
typedef struct {
    char* name;         // Parameter name
    VarType type;       // Parameter type (VAR_NUMERIC, VAR_STRING, VAR_BOOLEAN)
} Parameter;

// Function definition
typedef struct {
    char* name;                 // Function name
    Parameter** parameters;     // Array of parameters
    int param_count;            // Number of parameters
    VarType return_type;        // Return type (or VAR_NUMERIC for void)
    struct Statement** body;    // Function body statements
    int body_count;             // Number of statements in body
    int has_return;             // Whether function has explicit return
} Function;

// Function call
typedef struct {
    char* function_name;        // Name of function to call
    struct Expression** arguments;  // Array of argument expressions
    int argument_count;         // Number of arguments
} FunctionCall;

// Return statement
typedef struct {
    struct Expression* return_value;  // Expression to return (NULL for void)
} ReturnStatement;

#endif
