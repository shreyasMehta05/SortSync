#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * Structure to hold key-value pairs for sorting and ranking
 */
typedef struct {
    char* key;
    size_t original_index;
    size_t rank;
} KeyEntry;

/**
 * Structure to hold the MMPHF data
 */
typedef struct {
    KeyEntry* entries;
    size_t size;
    uint32_t seed;  // For hash function
} MMPHF;

/**
 * FNV-1a hash function for initial string hashing
 */
uint32_t fnv1a_hash(const char* str, uint32_t seed) {
    const uint32_t FNV_PRIME = 16777619;
    const uint32_t FNV_OFFSET = 2166136261;
    
    uint32_t hash = FNV_OFFSET ^ seed;
    
    for (const unsigned char* p = (const unsigned char*)str; *p; p++) {
        hash ^= *p;
        hash *= FNV_PRIME;
    }
    
    return hash;
}

/**
 * Comparison function for sorting KeyEntries
 */
int compare_entries(const void* a, const void* b) {
    const KeyEntry* entry_a = (const KeyEntry*)a;
    const KeyEntry* entry_b = (const KeyEntry*)b;
    return strcmp(entry_a->key, entry_b->key);
}

/**
 * Initialize MMPHF with a set of keys
 */
MMPHF* mmphf_create(const char** keys, size_t num_keys) {
    MMPHF* mmphf = (MMPHF*)malloc(sizeof(MMPHF));
    if (!mmphf) return NULL;
    
    mmphf->size = num_keys;
    mmphf->seed = 0x12345678;  // Can be randomized
    
    // Allocate and initialize entries
    mmphf->entries = (KeyEntry*)malloc(num_keys * sizeof(KeyEntry));
    if (!mmphf->entries) {
        free(mmphf);
        return NULL;
    }
    
    // Copy keys and store original indices
    for (size_t i = 0; i < num_keys; i++) {
        mmphf->entries[i].key = strdup(keys[i]);
        mmphf->entries[i].original_index = i;
    }
    
    // Sort entries lexicographically
    qsort(mmphf->entries, num_keys, sizeof(KeyEntry), compare_entries);
    
    // Assign ranks (positions in sorted order)
    for (size_t i = 0; i < num_keys; i++) {
        mmphf->entries[i].rank = i;
    }
    
    return mmphf;
}

/**
 * Get the rank of a key in the MMPHF
 * Returns size_t(-1) if key is not found
 */
size_t mmphf_get_rank(const MMPHF* mmphf, const char* key) {
    // Binary search to find the key
    int left = 0;
    int right = mmphf->size - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(key, mmphf->entries[mid].key);
        
        if (cmp == 0) {
            return mmphf->entries[mid].rank;
        }
        else if (cmp < 0) {
            right = mid - 1;
        }
        else {
            left = mid + 1;
        }
    }
    
    // Key not found - return arbitrary value based on hash
    return fnv1a_hash(key, mmphf->seed) % mmphf->size;
}

/**
 * Free MMPHF resources
 */
void mmphf_destroy(MMPHF* mmphf) {
    if (mmphf) {
        if (mmphf->entries) {
            for (size_t i = 0; i < mmphf->size; i++) {
                free(mmphf->entries[i].key);
            }
            free(mmphf->entries);
        }
        free(mmphf);
    }
}

/**
 * Utility function to print MMPHF contents
 */
void mmphf_print(const MMPHF* mmphf) {
    printf("MMPHF Contents:\n");
    for (size_t i = 0; i < mmphf->size; i++) {
        printf("Key: %-20s Rank: %zu\n", 
               mmphf->entries[i].key, 
               mmphf->entries[i].rank);
    }
}

int main() {
    // Example set of keys
    const char* keys[] = {
        "apple",
        "banana",
        "cherry",
        "date",
        "elderberry"
    };
    size_t num_keys = sizeof(keys) / sizeof(keys[0]);
    
    // Create MMPHF
    MMPHF* mmphf = mmphf_create(keys, num_keys);
    
    // Print the MMPHF contents
    mmphf_print(mmphf);
    
    // Test some lookups
    printf("\nLookup Results:\n");
    printf("apple: %zu\n", mmphf_get_rank(mmphf, "apple"));        // Should be 0
    printf("cherry: %zu\n", mmphf_get_rank(mmphf, "cherry"));      // Should be 2
    printf("grape: %zu\n", mmphf_get_rank(mmphf, "grape"));        // Not in set
    
    // Clean up
    mmphf_destroy(mmphf);
    
    return 0;
}