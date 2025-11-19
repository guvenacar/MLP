/**
 * MLP Preprocessor - C Implementation
 * 
 * Replaces dil_cevirici.py - translates MLP keywords from any language to English.
 * Uses json_parser.c to read diller.json dynamically.
 * 
 * Usage: mlp_preprocessor <input.mlp> [--lang=tr-TR] <output.mlp>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "json_parser.h"

#define MAX_LINE_LENGTH 4096
#define MAX_TOKEN_LENGTH 256

// Forward declaration
char* get_executable_path();

/**
 * Check if character can be part of an identifier
 */
int is_identifier_char(unsigned char c) {
    // Allow ASCII letters, digits, underscore
    if (isalnum(c) || c == '_') return 1;
    
    // Allow UTF-8 multibyte characters (for Turkish, Russian, Chinese, Hindi)
    if (c >= 0x80) return 1;
    
    return 0;
}

/**
 * Extract a token (identifier) from line starting at position
 * Returns length of token
 */
int extract_token(const char* line, int pos, char* token) {
    int len = 0;
    
    while (line[pos] && is_identifier_char((unsigned char)line[pos]) && len < MAX_TOKEN_LENGTH - 1) {
        token[len++] = line[pos++];
    }
    
    token[len] = '\0';
    return len;
}

/**
 * Check if we're inside a string literal at this position
 */
int is_in_string(const char* line, int pos) {
    int in_string = 0;
    char quote_char = 0;
    
    for (int i = 0; i < pos; i++) {
        if (!in_string) {
            if (line[i] == '"' || line[i] == '\'') {
                in_string = 1;
                quote_char = line[i];
            }
        } else {
            if (line[i] == quote_char && (i == 0 || line[i-1] != '\\')) {
                in_string = 0;
            }
        }
    }
    
    return in_string;
}

/**
 * Check if we're inside a comment at this position
 */
int is_in_comment(const char* line, int pos) {
    // Check for // style comments
    for (int i = 0; i < pos - 1; i++) {
        if (line[i] == '/' && line[i+1] == '/') {
            return 1;
        }
    }
    return 0;
}

/**
 * Process a single line, translating keywords
 */
void process_line(const char* input_line, char* output_line, const char* lang_id) {
    int in_pos = 0;
    int out_pos = 0;
    int line_len = strlen(input_line);
    
    while (in_pos < line_len) {
        // Skip translation if we're in a string or comment
        if (is_in_string(input_line, in_pos) || is_in_comment(input_line, in_pos)) {
            output_line[out_pos++] = input_line[in_pos++];
            continue;
        }
        
        // Check if this could be the start of an identifier
        if (is_identifier_char((unsigned char)input_line[in_pos])) {
            char token[MAX_TOKEN_LENGTH];
            int token_len = extract_token(input_line, in_pos, token);
            
            if (token_len > 0) {
                // Try to translate the token
                const char* translated = mlp_translate_keyword(lang_id, token);
                
                if (translated && translated[0] != '\0') {
                    // Token is a keyword, replace it
                    strcpy(&output_line[out_pos], translated);
                    out_pos += strlen(translated);
                    in_pos += token_len;
                } else {
                    // Not a keyword, keep original
                    strcpy(&output_line[out_pos], token);
                    out_pos += token_len;
                    in_pos += token_len;
                }
            } else {
                output_line[out_pos++] = input_line[in_pos++];
            }
        } else {
            // Not an identifier character, copy as-is
            output_line[out_pos++] = input_line[in_pos++];
        }
    }
    
    output_line[out_pos] = '\0';
}

/**
 * Main preprocessor function
 */
int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> [--lang=LANG] <output.mlp>\n", argv[0]);
        fprintf(stderr, "  LANG: Language ID (e.g., tr-TR, ru-RU, zh-CN, hi-IN)\n");
        return 1;
    }
    
    // Parse arguments
    char* input_file = argv[1];
    char* output_file = NULL;
    char* lang_id = NULL;
    
    // Check for --lang option
    for (int i = 2; i < argc; i++) {
        if (strncmp(argv[i], "--lang=", 7) == 0) {
            lang_id = argv[i] + 7;
        } else {
            output_file = argv[i];
        }
    }
    
    if (!output_file) {
        fprintf(stderr, "Error: Output file not specified\n");
        return 1;
    }
    
    // Get directory of executable to find diller.json
    char* exe_path = get_executable_path();
    if (!exe_path) {
        fprintf(stderr, "Error: Cannot determine executable path\n");
        return 1;
    }
    
    // Extract directory
    char json_path[2048];
    char* dir = strdup(exe_path);
    char* last_slash = strrchr(dir, '/');
    if (last_slash) {
        *last_slash = '\0';
        snprintf(json_path, sizeof(json_path), "%s/diller.json", dir);
    } else {
        strcpy(json_path, "diller.json");
    }
    free(dir);
    free(exe_path);
    
    // Initialize language system
    if (!mlp_init_languages(json_path)) {
        fprintf(stderr, "Error: Cannot load languages from %s\n", json_path);
        return 1;
    }
    
    // If no language specified, use default
    if (!lang_id) {
        lang_id = (char*)mlp_get_default_language();
        if (!lang_id || lang_id[0] == '\0') {
            lang_id = "en-US";  // Fallback to English
        }
    }
    
    // Open input file
    FILE* input_fp = fopen(input_file, "r");
    if (!input_fp) {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_file);
        mlp_cleanup_languages();
        return 1;
    }
    
    // Open output file
    FILE* output_fp = fopen(output_file, "w");
    if (!output_fp) {
        fprintf(stderr, "Error: Cannot create output file %s\n", output_file);
        fclose(input_fp);
        mlp_cleanup_languages();
        return 1;
    }
    
    // Process file line by line
    char input_line[MAX_LINE_LENGTH];
    char output_line[MAX_LINE_LENGTH * 2];  // Allow for expansion
    
    while (fgets(input_line, sizeof(input_line), input_fp)) {
        process_line(input_line, output_line, lang_id);
        fputs(output_line, output_fp);
    }
    
    // Cleanup
    fclose(input_fp);
    fclose(output_fp);
    mlp_cleanup_languages();
    
    return 0;
}

/**
 * Get executable path (needed for finding diller.json)
 */
char* get_executable_path() {
    char* path = malloc(2048);
    if (!path) return NULL;
    
    ssize_t len = readlink("/proc/self/exe", path, 2047);
    if (len != -1) {
        path[len] = '\0';
        return path;
    }
    
    free(path);
    return NULL;
}
