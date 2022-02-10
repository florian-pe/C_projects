
#define INIT_HASH_SIZE 16
#define RESIZE_RATIO 0.8

#define b2(x)   (   (x) | (   (x) >> 1) )
#define b4(x)   ( b2(x) | ( b2(x) >> 2) )
#define b8(x)   ( b4(x) | ( b4(x) >> 4) )
#define b16(x)  ( b8(x) | ( b8(x) >> 8) )  
#define b32(x)  (b16(x) | (b16(x) >>16) )
#define next_power_of_2(x)      (b32(x-1) + 1)


struct Entry {
    char *key;
    void *value;
    struct Entry* next;
};

typedef struct Entry Entry;

struct HashTable {

    Entry **buckets;
    void* (*allocator)(size_t);
    void (*freer)(void*);
    int bucket_number;
    int entries;
};

typedef struct HashTable HashTable;

struct HashIterator {
    Entry **buckets;
    Entry *curr_entry;
    int curr_slot;
    int bucket_number;
    int entries;
    int passed;
};

typedef struct HashIterator HashIterator;

HashTable*      hash_new(int n);
HashTable*      hash_custom_new(int n, void* (*allocator)(size_t), void (*freer)(void*));
int             hashing_function(char*);
int             hash_count(HashTable*);
void            hash_insert(HashTable*, char*, void*);
void*           hash_lookup(HashTable*, char*);
void*           hash_exists(HashTable*, char*);
char            hash_exists_p(HashTable*, char*);
void            hash_delete(HashTable*, char*);
HashIterator*   hash_iterator_new(HashTable*);
Entry*          hash_iterate(HashIterator*);
void            hash_dump(HashTable*);
void            hash_resize(HashTable *hash_table, int size);
void            hash_free(HashTable*);


