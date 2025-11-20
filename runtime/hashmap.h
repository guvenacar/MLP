/**
 * MLP Hash Map Implementation - Header File
 * Generic hash table with string keys and void* values
 * 
 * Features:
 * - Open addressing with linear probing
 * - Dynamic resizing (load factor > 0.7)
 * - String keys, generic value pointers
 * - Collision handling
 */

#ifndef MLP_HASHMAP_H
#define MLP_HASHMAP_H

#include <stddef.h>

// Hash map entry
typedef struct {
    char* key;           // String key (owned by map)
    void* value;         // Generic value pointer
    int is_occupied;     // 1 if slot is occupied
    int is_deleted;      // 1 if slot was deleted (tombstone)
} HashMapEntry;

// Hash map structure
typedef struct {
    HashMapEntry* entries;  // Array of entries
    int size;               // Number of key-value pairs
    int capacity;           // Total capacity
    int threshold;          // Resize threshold (capacity * 0.7)
} HashMap;

/**
 * hashmap_create - Create new hash map
 * @param initial_capacity: Initial capacity (default 16)
 * @return: Pointer to new HashMap
 */
HashMap* hashmap_create(int initial_capacity);

/**
 * hashmap_put - Insert or update key-value pair
 * @param map: Target hash map
 * @param key: String key (will be copied)
 * @param value: Value pointer
 * @return: 1 on success, 0 on failure
 */
int hashmap_put(HashMap* map, const char* key, void* value);

/**
 * hashmap_get - Get value for key
 * @param map: Source hash map
 * @param key: String key
 * @return: Value pointer, or NULL if not found
 */
void* hashmap_get(HashMap* map, const char* key);

/**
 * hashmap_remove - Remove key-value pair
 * @param map: Target hash map
 * @param key: String key
 * @return: 1 if removed, 0 if not found
 */
int hashmap_remove(HashMap* map, const char* key);

/**
 * hashmap_contains - Check if key exists
 * @param map: Source hash map
 * @param key: String key
 * @return: 1 if exists, 0 otherwise
 */
int hashmap_contains(HashMap* map, const char* key);

/**
 * hashmap_size - Get number of entries
 * @param map: Source hash map
 * @return: Number of key-value pairs
 */
int hashmap_size(HashMap* map);

/**
 * hashmap_clear - Remove all entries
 * @param map: Target hash map
 */
void hashmap_clear(HashMap* map);

/**
 * hashmap_destroy - Free entire hash map
 * @param map: Hash map to destroy
 */
void hashmap_destroy(HashMap* map);

/**
 * hashmap_hash - Hash function for strings (internal)
 * @param key: String to hash
 * @param capacity: Hash table capacity
 * @return: Hash value (index)
 */
unsigned int hashmap_hash(const char* key, int capacity);

#endif // MLP_HASHMAP_H
