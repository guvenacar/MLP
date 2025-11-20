/**
 * MLP Hash Map Implementation
 * Generic hash table with string keys and void* values
 * Uses open addressing with linear probing
 */

#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_CAPACITY 16
#define LOAD_FACTOR 0.7

/**
 * Hash function - DJB2 algorithm
 */
unsigned int hashmap_hash(const char* key, int capacity) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return (unsigned int)(hash % capacity);
}

/**
 * Create new hash map
 */
HashMap* hashmap_create(int initial_capacity) {
    if (initial_capacity <= 0) {
        initial_capacity = DEFAULT_CAPACITY;
    }
    
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (!map) {
        fprintf(stderr, "HATA [HASHMAP_CREATE]: Hafıza ayırma hatası\n");
        return NULL;
    }
    
    map->capacity = initial_capacity;
    map->size = 0;
    map->threshold = (int)(initial_capacity * LOAD_FACTOR);
    
    map->entries = (HashMapEntry*)calloc(initial_capacity, sizeof(HashMapEntry));
    if (!map->entries) {
        fprintf(stderr, "HATA [HASHMAP_CREATE]: Entries hafıza hatası\n");
        free(map);
        return NULL;
    }
    
    return map;
}

/**
 * Resize hash map when load factor exceeded
 */
static void hashmap_resize(HashMap* map) {
    int old_capacity = map->capacity;
    HashMapEntry* old_entries = map->entries;
    
    // Double capacity
    map->capacity = old_capacity * 2;
    map->threshold = (int)(map->capacity * LOAD_FACTOR);
    map->entries = (HashMapEntry*)calloc(map->capacity, sizeof(HashMapEntry));
    
    if (!map->entries) {
        fprintf(stderr, "HATA [HASHMAP_RESIZE]: Yeniden boyutlandırma hatası\n");
        map->entries = old_entries;
        map->capacity = old_capacity;
        return;
    }
    
    // Rehash all entries
    map->size = 0;
    for (int i = 0; i < old_capacity; i++) {
        if (old_entries[i].is_occupied && !old_entries[i].is_deleted) {
            hashmap_put(map, old_entries[i].key, old_entries[i].value);
            free(old_entries[i].key); // Free old key
        }
    }
    
    free(old_entries);
}

/**
 * Put key-value pair
 */
int hashmap_put(HashMap* map, const char* key, void* value) {
    if (!map || !key) {
        return 0;
    }
    
    // Resize if needed
    if (map->size >= map->threshold) {
        hashmap_resize(map);
    }
    
    unsigned int index = hashmap_hash(key, map->capacity);
    unsigned int start_index = index;
    
    // Linear probing
    while (1) {
        HashMapEntry* entry = &map->entries[index];
        
        // Empty slot or deleted slot
        if (!entry->is_occupied || entry->is_deleted) {
            entry->key = strdup(key);
            entry->value = value;
            entry->is_occupied = 1;
            entry->is_deleted = 0;
            map->size++;
            return 1;
        }
        
        // Update existing key
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return 1;
        }
        
        // Move to next slot
        index = (index + 1) % map->capacity;
        
        // Full table (shouldn't happen with proper load factor)
        if (index == start_index) {
            fprintf(stderr, "HATA [HASHMAP_PUT]: Tablo dolu\n");
            return 0;
        }
    }
}

/**
 * Get value for key
 */
void* hashmap_get(HashMap* map, const char* key) {
    if (!map || !key) {
        return NULL;
    }
    
    unsigned int index = hashmap_hash(key, map->capacity);
    unsigned int start_index = index;
    
    // Linear probing search
    while (1) {
        HashMapEntry* entry = &map->entries[index];
        
        // Empty slot - key not found
        if (!entry->is_occupied) {
            return NULL;
        }
        
        // Found key (not deleted)
        if (!entry->is_deleted && strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        
        // Move to next slot
        index = (index + 1) % map->capacity;
        
        // Searched entire table
        if (index == start_index) {
            return NULL;
        }
    }
}

/**
 * Remove key-value pair
 */
int hashmap_remove(HashMap* map, const char* key) {
    if (!map || !key) {
        return 0;
    }
    
    unsigned int index = hashmap_hash(key, map->capacity);
    unsigned int start_index = index;
    
    // Linear probing search
    while (1) {
        HashMapEntry* entry = &map->entries[index];
        
        // Empty slot - key not found
        if (!entry->is_occupied) {
            return 0;
        }
        
        // Found key (not deleted)
        if (!entry->is_deleted && strcmp(entry->key, key) == 0) {
            free(entry->key);
            entry->key = NULL;
            entry->value = NULL;
            entry->is_deleted = 1; // Tombstone
            map->size--;
            return 1;
        }
        
        // Move to next slot
        index = (index + 1) % map->capacity;
        
        // Searched entire table
        if (index == start_index) {
            return 0;
        }
    }
}

/**
 * Check if key exists
 */
int hashmap_contains(HashMap* map, const char* key) {
    return hashmap_get(map, key) != NULL;
}

/**
 * Get size
 */
int hashmap_size(HashMap* map) {
    if (!map) {
        return 0;
    }
    return map->size;
}

/**
 * Clear all entries
 */
void hashmap_clear(HashMap* map) {
    if (!map) {
        return;
    }
    
    for (int i = 0; i < map->capacity; i++) {
        if (map->entries[i].is_occupied && map->entries[i].key) {
            free(map->entries[i].key);
        }
    }
    
    memset(map->entries, 0, map->capacity * sizeof(HashMapEntry));
    map->size = 0;
}

/**
 * Destroy hash map
 */
void hashmap_destroy(HashMap* map) {
    if (!map) {
        return;
    }
    
    hashmap_clear(map);
    free(map->entries);
    free(map);
}
