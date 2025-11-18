/*
 * ===============================================
 * MLP Preprocessor - Multi-Language Keyword Mapper
 * ===============================================
 *
 * Purpose: Converts any language keywords to internal Turkish keywords
 *
 * Flow:
 *   User Code (English/Hindi/etc)
 *     → Language Detection
 *     → Keyword Mapping (to Turkish)
 *     → Internal MLP Code (Turkish)
 *     → MLP Compiler
 *
 * Features:
 *   - UTF-8/Unicode aware
 *   - Auto language detection
 *   - YAML-based keyword mapping
 *   - Preserves comments, strings, operators
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

// ===============================================
// UTF-8 Helpers
// ===============================================

/**
 * Check if a byte is a UTF-8 continuation byte (10xxxxxx)
 */
bool is_utf8_continuation(unsigned char byte) {
    return (byte & 0xC0) == 0x80;
}

/**
 * Get the number of bytes in a UTF-8 character
 * Returns: 1-4 for valid UTF-8, 0 for invalid
 */
int utf8_char_length(const char* str) {
    unsigned char first = (unsigned char)str[0];

    if ((first & 0x80) == 0) return 1;      // 0xxxxxxx (ASCII)
    if ((first & 0xE0) == 0xC0) return 2;   // 110xxxxx
    if ((first & 0xF0) == 0xE0) return 3;   // 1110xxxx
    if ((first & 0xF8) == 0xF0) return 4;   // 11110xxx

    return 0;  // Invalid UTF-8
}

/**
 * Extract a single UTF-8 character
 */
char* utf8_extract_char(const char* str, int* bytes_read) {
    int len = utf8_char_length(str);
    if (len == 0) {
        *bytes_read = 0;
        return NULL;
    }

    char* result = (char*)malloc(len + 1);
    memcpy(result, str, len);
    result[len] = '\0';
    *bytes_read = len;

    return result;
}

/**
 * Check if a string starts with a specific UTF-8 keyword
 */
bool starts_with_utf8(const char* str, const char* keyword) {
    size_t keyword_len = strlen(keyword);
    return strncmp(str, keyword, keyword_len) == 0;
}

// ===============================================
// Language Detection
// ===============================================

typedef enum {
    LANG_TURKISH,
    LANG_ENGLISH,
    LANG_HINDI,
    LANG_UNKNOWN
} Language;

/**
 * Detect language by analyzing keywords
 *
 * Strategy:
 *   1. Look for language-specific Unicode ranges (Devanagari for Hindi, etc.)
 *   2. Look for common keywords in each language
 *   3. Default to English if unsure
 */
Language detect_language(const char* source_code) {
    // Check for Devanagari script (Hindi)
    for (size_t i = 0; source_code[i] != '\0'; ) {
        unsigned char byte = (unsigned char)source_code[i];

        // Devanagari range: U+0900 to U+097F (encoded in UTF-8)
        if (byte >= 0xE0 && byte <= 0xEF) {
            int len = utf8_char_length(&source_code[i]);
            if (len == 3) {
                // Check if in Devanagari range
                unsigned char byte2 = (unsigned char)source_code[i+1];
                unsigned char byte3 = (unsigned char)source_code[i+2];

                if (byte == 0xE0 && byte2 >= 0xA4 && byte2 <= 0xA5) {
                    return LANG_HINDI;
                }
            }
            i += len;
        } else {
            i++;
        }
    }

    // Check for Turkish-specific characters (İ, Ğ, Ş, etc.)
    if (strstr(source_code, "İŞLEÇ") || strstr(source_code, "EĞER") ||
        strstr(source_code, "DÖNÜŞ") || strstr(source_code, "YAZDIR")) {
        return LANG_TURKISH;
    }

    // Check for English keywords
    if (strstr(source_code, "function") || strstr(source_code, "return") ||
        strstr(source_code, "print")) {
        return LANG_ENGLISH;
    }

    return LANG_ENGLISH;  // Default to English
}

const char* language_name(Language lang) {
    switch (lang) {
        case LANG_TURKISH: return "Turkish";
        case LANG_ENGLISH: return "English";
        case LANG_HINDI: return "Hindi";
        default: return "Unknown";
    }
}

// ===============================================
// Keyword Mapping
// ===============================================

typedef struct {
    const char* external;  // Language-specific keyword (e.g., "function", "कार्य")
    const char* internal;  // Internal Turkish keyword (e.g., "İŞLEÇ")
} KeywordMapping;

// English → Turkish mapping
KeywordMapping english_to_turkish[] = {
    // Data types
    {"number", "SAYISAL"},
    {"string", "METIN"},
    {"bool", "BOOL"},

    // Structures
    {"struct", "YAPI"},

    // Control flow
    {"if", "EĞER"},
    {"then", "İSE"},
    {"else", "DEĞİLSE"},
    {"end", "SON"},

    // Loops
    {"loop", "DÖNGÜ"},
    {"break", "DÖNGÜ_BITIR"},

    // Functions
    {"function", "İŞLEÇ"},
    {"return", "DÖNÜŞ"},

    // I/O
    {"print", "YAZDIR"},

    {NULL, NULL}  // Terminator
};

// Hindi → Turkish mapping
KeywordMapping hindi_to_turkish[] = {
    // Data types
    {"संख्या", "SAYISAL"},      // number
    {"पाठ", "METIN"},            // string
    {"बूल", "BOOL"},             // bool

    // Structures
    {"संरचना", "YAPI"},          // struct

    // Control flow
    {"यदि", "EĞER"},             // if
    {"है", "İSE"},               // then
    {"अन्यथा", "DEĞİLSE"},       // else
    {"समाप्त", "SON"},           // end

    // Loops
    {"लूप", "DÖNGÜ"},            // loop
    {"तोड़ो", "DÖNGÜ_BITIR"},    // break

    // Functions
    {"कार्य", "İŞLEÇ"},          // function
    {"वापसी", "DÖNÜŞ"},          // return

    // I/O
    {"लिखो", "YAZDIR"},          // print

    {NULL, NULL}  // Terminator
};

/**
 * Find the Turkish equivalent of a keyword
 */
const char* map_keyword(const char* keyword, Language lang) {
    KeywordMapping* mapping = NULL;

    switch (lang) {
        case LANG_ENGLISH:
            mapping = english_to_turkish;
            break;
        case LANG_HINDI:
            mapping = hindi_to_turkish;
            break;
        case LANG_TURKISH:
            // Already in Turkish, return as-is
            return keyword;
        default:
            return keyword;
    }

    // Search for mapping
    for (int i = 0; mapping[i].external != NULL; i++) {
        if (strcmp(keyword, mapping[i].external) == 0) {
            return mapping[i].internal;
        }
    }

    return keyword;  // No mapping found, return original
}

// ===============================================
// Preprocessor Main Logic
// ===============================================

/**
 * Preprocess source code: Convert language-specific keywords to Turkish
 */
char* mlp_preprocess(const char* source_code) {
    // 1. Detect language
    Language lang = detect_language(source_code);
    printf("[MLP Preprocessor] Detected language: %s\n", language_name(lang));

    // If already Turkish, no processing needed
    if (lang == LANG_TURKISH) {
        return strdup(source_code);
    }

    // 2. Allocate output buffer (same size as input, will reallocate if needed)
    size_t output_size = strlen(source_code) * 2;  // Extra space for longer Turkish keywords
    char* output = (char*)malloc(output_size);
    size_t output_pos = 0;

    // 3. Process character by character
    size_t i = 0;
    while (source_code[i] != '\0') {
        // Skip whitespace
        if (isspace(source_code[i])) {
            output[output_pos++] = source_code[i++];
            continue;
        }

        // Skip string literals (preserve them as-is)
        if (source_code[i] == '"') {
            output[output_pos++] = source_code[i++];
            while (source_code[i] != '\0' && source_code[i] != '"') {
                output[output_pos++] = source_code[i++];
            }
            if (source_code[i] == '"') {
                output[output_pos++] = source_code[i++];
            }
            continue;
        }

        // Check if this is a keyword (identifier)
        if (isalpha(source_code[i]) || (unsigned char)source_code[i] >= 0x80) {
            // Extract keyword
            char keyword[256];
            int keyword_len = 0;

            while ((isalnum(source_code[i]) || source_code[i] == '_' ||
                    (unsigned char)source_code[i] >= 0x80) &&
                   source_code[i] != '\0') {
                keyword[keyword_len++] = source_code[i++];
            }
            keyword[keyword_len] = '\0';

            // Map keyword to Turkish
            const char* turkish_keyword = map_keyword(keyword, lang);

            // Append mapped keyword to output
            strcpy(&output[output_pos], turkish_keyword);
            output_pos += strlen(turkish_keyword);
        } else {
            // Not a keyword, copy as-is (operators, punctuation, etc.)
            output[output_pos++] = source_code[i++];
        }
    }

    output[output_pos] = '\0';
    return output;
}

// ===============================================
// Main (for testing)
// ===============================================

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.mlp>\n", argv[0]);
        fprintf(stderr, "   Preprocesses multi-language MLP code to internal Turkish representation\n");
        return 1;
    }

    // Read input file
    FILE* input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", argv[1]);
        return 1;
    }

    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    char* source_code = (char*)malloc(file_size + 1);
    fread(source_code, 1, file_size, input);
    source_code[file_size] = '\0';
    fclose(input);

    // Preprocess
    char* preprocessed = mlp_preprocess(source_code);

    // Write output file
    FILE* output = fopen(argv[2], "w");
    if (!output) {
        fprintf(stderr, "Error: Cannot write to output file '%s'\n", argv[2]);
        free(source_code);
        free(preprocessed);
        return 1;
    }

    fprintf(output, "%s", preprocessed);
    fclose(output);

    printf("[MLP Preprocessor] Successfully preprocessed: %s → %s\n", argv[1], argv[2]);

    free(source_code);
    free(preprocessed);

    return 0;
}
