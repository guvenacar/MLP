#ifndef PARSER_H
#define PARSER_H

// MODULAR PARSER HEADER
// This is a compatibility layer for modules that still reference parser.h
// All new code should use specific module headers instead

#include "modules/parser_core/parser_core.h"
#include "modules/expression/expression.h"
#include "modules/statement/statement.h"

// Re-export Parser struct and types from modular components
// This allows old code to work without changes

#endif
