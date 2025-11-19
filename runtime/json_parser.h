/**
 * JSON Parser for MLP Language Definitions
 * 
 * Parses diller.json and provides keyword translation functionality
 * for the MLP multi-language preprocessor.
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stddef.h>

// Maximum limits
#define MAX_LANGUAGES 20
#define MAX_KEYWORDS 50
#define MAX_VARIANTS 10
#define MAX_STRING_LEN 256

/**
 * Keyword mapping structure
 * Maps multiple native language variants to a single English keyword
 */
typedef struct {
    char english_keyword[MAX_STRING_LEN];  // e.g., "if"
    char* variants[MAX_VARIANTS];          // e.g., ["eğer", "EĞER", "eger", "EGER"]
    int variant_count;
} KeywordMapping;

/**
 * Language definition structure
 */
typedef struct {
    char id[MAX_STRING_LEN];               // e.g., "tr-TR"
    char name[MAX_STRING_LEN];             // e.g., "Türkçe (Turkish)"
    char description[MAX_STRING_LEN];      // e.g., "Turkish programming keywords"
    KeywordMapping keywords[MAX_KEYWORDS];
    int keyword_count;
} Language;

/**
 * Language map structure - holds all loaded languages
 */
typedef struct {
    Language languages[MAX_LANGUAGES];
    int language_count;
    char default_language[MAX_STRING_LEN];
} LanguageMap;

// Core API functions

/**
 * Load languages from JSON file
 * Returns NULL on failure
 */
LanguageMap* load_languages_from_json(const char* json_file);

/**
 * Free language map memory
 */
void free_language_map(LanguageMap* map);

/**
 * Find language by ID (e.g., "tr-TR")
 * Returns NULL if not found
 */
Language* find_language(LanguageMap* map, const char* lang_id);

/**
 * Translate a keyword from native language to English
 * Returns NULL if no translation found
 * 
 * Example:
 *   translate_keyword(map, "tr-TR", "eğer") → "if"
 *   translate_keyword(map, "ru-RU", "если") → "if"
 */
const char* translate_keyword(LanguageMap* map, const char* lang_id, const char* native_keyword);

/**
 * Check if a word is a keyword in the given language
 * Returns 1 if keyword, 0 otherwise
 */
int is_keyword(LanguageMap* map, const char* lang_id, const char* word);

/**
 * Get default language ID
 */
const char* get_default_language(LanguageMap* map);

/**
 * Print language map (debug)
 */
void print_language_map(LanguageMap* map);

// MLP Bridge Functions (callable from MLP code)

/**
 * Global language map instance
 * Loaded once at program start
 */
extern LanguageMap* g_language_map;

/**
 * Initialize language system (call from MLP)
 * Returns 1 on success, 0 on failure
 */
long mlp_init_languages(const char* json_file);

/**
 * Translate keyword (call from MLP)
 * Returns pointer to static buffer (English keyword) or empty string
 * 
 * MLP Usage:
 *   METIN result = MLP_TRANSLATE("tr-TR", "eğer")
 */
const char* mlp_translate_keyword(const char* lang_id, const char* native_keyword);

/**
 * Check if word is keyword (call from MLP)
 * Returns 1 if keyword, 0 otherwise
 * 
 * MLP Usage:
 *   SAYISAL is_kw = MLP_IS_KEYWORD("tr-TR", "eğer")
 */
long mlp_is_keyword(const char* lang_id, const char* word);

/**
 * Get default language (call from MLP)
 * Returns language ID string
 * 
 * MLP Usage:
 *   METIN lang = MLP_GET_DEFAULT_LANG()
 */
const char* mlp_get_default_language(void);

/**
 * Cleanup language system (call from MLP at exit)
 */
void mlp_cleanup_languages(void);

#endif // JSON_PARSER_H
