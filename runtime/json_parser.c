/**
 * JSON Parser for MLP Language Definitions
 * 
 * Simple JSON parser specifically for diller.json structure.
 * Does not aim to be a complete JSON parser - only handles the
 * specific format used in MLP language definitions.
 */

#include "json_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Global language map
LanguageMap* g_language_map = NULL;

// Helper functions for JSON parsing

/**
 * Skip whitespace in JSON string
 */
static const char* skip_whitespace(const char* json) {
    while (*json && isspace(*json)) {
        json++;
    }
    return json;
}

/**
 * Parse a JSON string value
 * Handles: "value" or 'value'
 * Returns NULL on failure
 */
static char* parse_string(const char** json_ptr) {
    const char* json = skip_whitespace(*json_ptr);
    
    if (*json != '"' && *json != '\'') {
        return NULL;
    }
    
    char quote = *json;
    json++; // skip opening quote
    
    char buffer[MAX_STRING_LEN];
    int i = 0;
    
    while (*json && *json != quote && i < MAX_STRING_LEN - 1) {
        if (*json == '\\') {
            json++;
            switch (*json) {
                case 'n': buffer[i++] = '\n'; break;
                case 't': buffer[i++] = '\t'; break;
                case 'r': buffer[i++] = '\r'; break;
                case '\\': buffer[i++] = '\\'; break;
                case '"': buffer[i++] = '"'; break;
                case '\'': buffer[i++] = '\''; break;
                default: buffer[i++] = *json; break;
            }
            json++;
        } else {
            buffer[i++] = *json++;
        }
    }
    
    if (*json != quote) {
        return NULL;
    }
    
    buffer[i] = '\0';
    json++; // skip closing quote
    
    *json_ptr = json;
    
    char* result = malloc(strlen(buffer) + 1);
    if (result) {
        strcpy(result, buffer);
    }
    return result;
}

/**
 * Skip to the next key or closing brace
 */
static const char* skip_to_next(const char* json) {
    json = skip_whitespace(json);
    if (*json == ',') {
        json++;
        json = skip_whitespace(json);
    }
    return json;
}

/**
 * Find a key in JSON object
 * Returns pointer to value after the ':' or NULL
 */
static const char* find_key(const char* json, const char* key) {
    json = skip_whitespace(json);
    
    if (*json != '{') {
        return NULL;
    }
    json++;
    
    while (*json && *json != '}') {
        json = skip_whitespace(json);
        
        // Parse key
        char* current_key = parse_string(&json);
        if (!current_key) {
            break;
        }
        
        json = skip_whitespace(json);
        if (*json != ':') {
            free(current_key);
            break;
        }
        json++; // skip ':'
        
        json = skip_whitespace(json);
        
        if (strcmp(current_key, key) == 0) {
            free(current_key);
            return json; // Found the key, return pointer to value
        }
        
        free(current_key);
        
        // Skip this value
        int depth = 0;
        int in_string = 0;
        char string_char = 0;
        
        while (*json) {
            if (!in_string) {
                if (*json == '"' || *json == '\'') {
                    in_string = 1;
                    string_char = *json;
                } else if (*json == '{' || *json == '[') {
                    depth++;
                } else if (*json == '}' || *json == ']') {
                    if (depth == 0) {
                        break;
                    }
                    depth--;
                } else if (*json == ',' && depth == 0) {
                    json++;
                    break;
                }
            } else {
                if (*json == string_char && *(json - 1) != '\\') {
                    in_string = 0;
                }
            }
            json++;
        }
    }
    
    return NULL;
}

/**
 * Parse an array of strings
 * Returns number of strings parsed
 */
static int parse_string_array(const char** json_ptr, char** array, int max_count) {
    const char* json = skip_whitespace(*json_ptr);
    
    if (*json != '[') {
        return 0;
    }
    json++;
    
    int count = 0;
    while (*json && *json != ']' && count < max_count) {
        json = skip_whitespace(json);
        
        if (*json == ']') {
            break;
        }
        
        char* str = parse_string(&json);
        if (str) {
            array[count++] = str;
        }
        
        json = skip_whitespace(json);
        if (*json == ',') {
            json++;
        }
    }
    
    if (*json == ']') {
        json++;
    }
    
    *json_ptr = json;
    return count;
}

/**
 * Parse keywords object into KeywordMapping array
 */
static int parse_keywords(const char* json, KeywordMapping* keywords, int max_count) {
    json = skip_whitespace(json);
    
    if (*json != '{') {
        return 0;
    }
    json++;
    
    int count = 0;
    while (*json && *json != '}' && count < max_count) {
        json = skip_whitespace(json);
        
        if (*json == '}') {
            break;
        }
        
        // Parse English keyword (the key)
        char* english_kw = parse_string(&json);
        if (!english_kw) {
            break;
        }
        
        json = skip_whitespace(json);
        if (*json != ':') {
            free(english_kw);
            break;
        }
        json++;
        
        json = skip_whitespace(json);
        
        // Parse variants array (the value)
        KeywordMapping* kw = &keywords[count];
        strncpy(kw->english_keyword, english_kw, MAX_STRING_LEN - 1);
        kw->english_keyword[MAX_STRING_LEN - 1] = '\0';
        free(english_kw);
        
        kw->variant_count = parse_string_array(&json, kw->variants, MAX_VARIANTS);
        
        if (kw->variant_count > 0) {
            count++;
        }
        
        json = skip_whitespace(json);
        if (*json == ',') {
            json++;
        }
    }
    
    return count;
}

/**
 * Parse a single language definition
 */
static int parse_language(const char* json, Language* lang) {
    json = skip_whitespace(json);
    
    if (*json != '{') {
        return 0;
    }
    
    // Parse id
    const char* id_value = find_key(json, "id");
    if (id_value) {
        char* id = parse_string(&id_value);
        if (id) {
            strncpy(lang->id, id, MAX_STRING_LEN - 1);
            lang->id[MAX_STRING_LEN - 1] = '\0';
            free(id);
        }
    }
    
    // Parse name
    const char* name_value = find_key(json, "name");
    if (name_value) {
        char* name = parse_string(&name_value);
        if (name) {
            strncpy(lang->name, name, MAX_STRING_LEN - 1);
            lang->name[MAX_STRING_LEN - 1] = '\0';
            free(name);
        }
    }
    
    // Parse description
    const char* desc_value = find_key(json, "description");
    if (desc_value) {
        char* desc = parse_string(&desc_value);
        if (desc) {
            strncpy(lang->description, desc, MAX_STRING_LEN - 1);
            lang->description[MAX_STRING_LEN - 1] = '\0';
            free(desc);
        }
    }
    
    // Parse keywords
    const char* keywords_value = find_key(json, "keywords");
    if (keywords_value) {
        lang->keyword_count = parse_keywords(keywords_value, lang->keywords, MAX_KEYWORDS);
    }
    
    return lang->id[0] != '\0';
}

/**
 * Load languages from JSON file
 */
LanguageMap* load_languages_from_json(const char* json_file) {
    // Read file
    FILE* fp = fopen(json_file, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open %s\n", json_file);
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* json_content = malloc(file_size + 1);
    if (!json_content) {
        fclose(fp);
        return NULL;
    }
    
    fread(json_content, 1, file_size, fp);
    json_content[file_size] = '\0';
    fclose(fp);
    
    // Allocate language map
    LanguageMap* map = calloc(1, sizeof(LanguageMap));
    if (!map) {
        free(json_content);
        return NULL;
    }
    
    // Parse root object
    const char* json = skip_whitespace(json_content);
    if (*json != '{') {
        free(json_content);
        free(map);
        return NULL;
    }
    
    // Parse defaultLanguage
    const char* default_lang_value = find_key(json, "defaultLanguage");
    if (default_lang_value) {
        char* default_lang = parse_string(&default_lang_value);
        if (default_lang) {
            strncpy(map->default_language, default_lang, MAX_STRING_LEN - 1);
            map->default_language[MAX_STRING_LEN - 1] = '\0';
            free(default_lang);
        }
    }
    
    // Parse languages array
    const char* languages_value = find_key(json, "languages");
    if (languages_value) {
        languages_value = skip_whitespace(languages_value);
        
        if (*languages_value == '[') {
            languages_value++;
            
            while (*languages_value && *languages_value != ']' && map->language_count < MAX_LANGUAGES) {
                languages_value = skip_whitespace(languages_value);
                
                if (*languages_value == ']') {
                    break;
                }
                
                Language* lang = &map->languages[map->language_count];
                if (parse_language(languages_value, lang)) {
                    map->language_count++;
                }
                
                // Skip to next language object
                int depth = 0;
                int in_string = 0;
                char string_char = 0;
                
                while (*languages_value) {
                    if (!in_string) {
                        if (*languages_value == '"' || *languages_value == '\'') {
                            in_string = 1;
                            string_char = *languages_value;
                        } else if (*languages_value == '{') {
                            depth++;
                        } else if (*languages_value == '}') {
                            depth--;
                            if (depth == 0) {
                                languages_value++;
                                break;
                            }
                        }
                    } else {
                        if (*languages_value == string_char && *(languages_value - 1) != '\\') {
                            in_string = 0;
                        }
                    }
                    languages_value++;
                }
                
                languages_value = skip_whitespace(languages_value);
                if (*languages_value == ',') {
                    languages_value++;
                }
            }
        }
    }
    
    free(json_content);
    
    if (map->language_count == 0) {
        free(map);
        return NULL;
    }
    
    return map;
}

/**
 * Free language map memory
 */
void free_language_map(LanguageMap* map) {
    if (!map) return;
    
    for (int i = 0; i < map->language_count; i++) {
        Language* lang = &map->languages[i];
        for (int j = 0; j < lang->keyword_count; j++) {
            KeywordMapping* kw = &lang->keywords[j];
            for (int k = 0; k < kw->variant_count; k++) {
                free(kw->variants[k]);
            }
        }
    }
    
    free(map);
}

/**
 * Find language by ID
 */
Language* find_language(LanguageMap* map, const char* lang_id) {
    if (!map || !lang_id) return NULL;
    
    for (int i = 0; i < map->language_count; i++) {
        if (strcmp(map->languages[i].id, lang_id) == 0) {
            return &map->languages[i];
        }
    }
    
    return NULL;
}

/**
 * Translate keyword from native language to English
 */
const char* translate_keyword(LanguageMap* map, const char* lang_id, const char* native_keyword) {
    if (!map || !lang_id || !native_keyword) return NULL;
    
    Language* lang = find_language(map, lang_id);
    if (!lang) return NULL;
    
    for (int i = 0; i < lang->keyword_count; i++) {
        KeywordMapping* kw = &lang->keywords[i];
        for (int j = 0; j < kw->variant_count; j++) {
            if (strcmp(kw->variants[j], native_keyword) == 0) {
                return kw->english_keyword;
            }
        }
    }
    
    return NULL;
}

/**
 * Check if word is a keyword
 */
int is_keyword(LanguageMap* map, const char* lang_id, const char* word) {
    return translate_keyword(map, lang_id, word) != NULL;
}

/**
 * Get default language
 */
const char* get_default_language(LanguageMap* map) {
    if (!map) return NULL;
    return map->default_language;
}

/**
 * Print language map (debug)
 */
void print_language_map(LanguageMap* map) {
    if (!map) {
        printf("Language map is NULL\n");
        return;
    }
    
    printf("Language Map:\n");
    printf("  Default Language: %s\n", map->default_language);
    printf("  Languages: %d\n\n", map->language_count);
    
    for (int i = 0; i < map->language_count; i++) {
        Language* lang = &map->languages[i];
        printf("  [%d] %s (%s)\n", i, lang->name, lang->id);
        printf("      Description: %s\n", lang->description);
        printf("      Keywords: %d\n", lang->keyword_count);
        
        for (int j = 0; j < lang->keyword_count && j < 5; j++) {
            KeywordMapping* kw = &lang->keywords[j];
            printf("        %s -> [", kw->english_keyword);
            for (int k = 0; k < kw->variant_count; k++) {
                printf("%s%s", kw->variants[k], k < kw->variant_count - 1 ? ", " : "");
            }
            printf("]\n");
        }
        if (lang->keyword_count > 5) {
            printf("        ... and %d more keywords\n", lang->keyword_count - 5);
        }
        printf("\n");
    }
}

// MLP Bridge Functions

/**
 * Initialize language system
 */
long mlp_init_languages(const char* json_file) {
    if (g_language_map) {
        free_language_map(g_language_map);
    }
    
    g_language_map = load_languages_from_json(json_file);
    return g_language_map != NULL ? 1 : 0;
}

/**
 * Translate keyword (MLP callable)
 */
const char* mlp_translate_keyword(const char* lang_id, const char* native_keyword) {
    static char empty[] = "";
    
    if (!g_language_map) {
        return empty;
    }
    
    const char* result = translate_keyword(g_language_map, lang_id, native_keyword);
    return result ? result : empty;
}

/**
 * Check if word is keyword (MLP callable)
 */
long mlp_is_keyword(const char* lang_id, const char* word) {
    if (!g_language_map) {
        return 0;
    }
    
    return is_keyword(g_language_map, lang_id, word) ? 1 : 0;
}

/**
 * Get default language (MLP callable)
 */
const char* mlp_get_default_language(void) {
    static char empty[] = "en-US";
    
    if (!g_language_map) {
        return empty;
    }
    
    const char* result = get_default_language(g_language_map);
    return result ? result : empty;
}

/**
 * Cleanup language system
 */
void mlp_cleanup_languages(void) {
    if (g_language_map) {
        free_language_map(g_language_map);
        g_language_map = NULL;
    }
}

// Test main (optional, for standalone testing)
#ifdef JSON_PARSER_TEST
int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <diller.json>\n", argv[0]);
        return 1;
    }
    
    printf("Loading languages from %s...\n\n", argv[1]);
    
    if (!mlp_init_languages(argv[1])) {
        printf("Failed to load languages!\n");
        return 1;
    }
    
    print_language_map(g_language_map);
    
    // Test translations
    printf("\nTest Translations:\n");
    printf("  tr-TR: 'eğer' -> '%s'\n", mlp_translate_keyword("tr-TR", "eğer"));
    printf("  tr-TR: 'yoksa' -> '%s'\n", mlp_translate_keyword("tr-TR", "yoksa"));
    printf("  ru-RU: 'если' -> '%s'\n", mlp_translate_keyword("ru-RU", "если"));
    printf("  zh-CN: '如果' -> '%s'\n", mlp_translate_keyword("zh-CN", "如果"));
    
    printf("\nTest is_keyword:\n");
    printf("  tr-TR: 'eğer' -> %ld\n", mlp_is_keyword("tr-TR", "eğer"));
    printf("  tr-TR: 'invalid' -> %ld\n", mlp_is_keyword("tr-TR", "invalid"));
    
    printf("\nDefault Language: %s\n", mlp_get_default_language());
    
    mlp_cleanup_languages();
    
    return 0;
}
#endif