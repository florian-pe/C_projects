
#include <stdio.h>
#include <stdlib.h>
#include "hash-lib.h"

// TODO
// modify the iterator functions to instead create a temporary array of keys to deal with the case where we would delete or add keys to the hash, while iterating

HashTable* hash_new(int n) {
    int size;

    if (n <= 0) {
        size = INIT_HASH_SIZE;
    }
    else {
        size = next_power_of_2(n);
    }

    HashTable* hash = (HashTable*) malloc(sizeof(HashTable));

    hash->buckets       = malloc(sizeof(Entry *) * size);
    hash->bucket_number = size;
    hash->entries       = 0;

    for (int i=0; i < size; i++) {
        hash->buckets[0] = NULL;
    }
    return hash;
}

HashTable* hash_custom_new(int n, void* (*allocator)(size_t), void (*freer)(void*)) {
    int size;

    if (n <= 0) {
        size = INIT_HASH_SIZE;
    }
    else {
        size = next_power_of_2(n);
    }

    if (allocator == NULL) {
        printf("hash_custom_new() :\n  Need a non NULL allocator\n");
        exit(1);
    }
    if (freer == NULL) {
        printf("hash_custom_new() :\n  Need a non NULL freer\n");
        exit(1);
    }

    HashTable* hash = (HashTable*) (allocator)(sizeof(HashTable));

    hash->buckets       = (allocator)(sizeof(Entry *) * size);
    hash->bucket_number = size;
    hash->entries       = 0;
    hash->allocator     = allocator;
    hash->freer         = freer;

    for (int i=0; i < size; i++) {
        hash->buckets[0] = NULL;
    }
    return hash;
}

int hashing_function(char *string) {
    int hash = 0;
    for (int i = 0; string[i] != '\0'; i++) {
//      hash = hash*33 + string[i];
        hash += (hash << 5) + string[i];
    }
    return hash;
}

int string_equal(char *string_a, char *string_b) {
    int i = 0;
    while (string_a[i] != '\0' && string_b[i] != '\0') {
        if (string_a[i] != string_b[i]) {
            return 0;
        }
        i++;
    }
    if (string_a[i] == '\0' && string_b[i] == '\0');
        return 1;
    return 0;
}

int hash_count(HashTable *hash_table) {
    return hash_table->entries;
}

void hash_insert(HashTable *hash_table, char *key, void *value) {
    if (key == NULL)
        return;

    if ((hash_table->entries+1) > RESIZE_RATIO * hash_table->bucket_number)
        hash_resize(hash_table, 2 * hash_table->bucket_number);

    int slot = hashing_function(key) % hash_table->bucket_number;
    if (hash_table->buckets[slot] == NULL) {
        Entry *entry = malloc(sizeof(Entry));
        entry->key   = key;
        entry->value = value;
        entry->next  = NULL;
        hash_table->buckets[slot] = entry;
        hash_table->entries++;
        return;
    }

    for (Entry *ptr = hash_table->buckets[slot]; ptr != NULL ; ptr = ptr->next) {

        if (string_equal(ptr->key, key)) {
            ptr->value = value;
            hash_table->entries++;
            return;
        }
    }

    Entry *entry = malloc(sizeof(Entry));
    entry->key   = key;
    entry->value = value;
    entry->next  = hash_table->buckets[slot];
    hash_table->buckets[slot] = entry;
    hash_table->entries++;
    return;
}

void* hash_lookup(HashTable *hash_table, char *key) {

    if (key == NULL)
        return NULL;

    int slot = hashing_function(key) % hash_table->bucket_number;
    if (hash_table->buckets[slot] == NULL)
        return NULL;

    for (Entry *ptr = hash_table->buckets[slot]; ptr != NULL ; ptr = ptr->next) {

        if (string_equal(ptr->key, key))
            return ptr->value;
    }

    return NULL;
}

void* hash_exists(HashTable *hash_table, char *key) {

    if (key == NULL)
        return NULL;

    int slot = hashing_function(key) % hash_table->bucket_number;
    if (hash_table->buckets[slot] == NULL)
        return NULL;

    for (Entry *ptr = hash_table->buckets[slot]; ptr != NULL ; ptr = ptr->next) {

        if (string_equal(ptr->key, key))
            return ptr;
    }

    return NULL;
}

char hash_exists_p(HashTable *hash_table, char *key) {

    if (key == NULL)
        return 0;

    int slot = hashing_function(key) % hash_table->bucket_number;
    if (hash_table->buckets[slot] == NULL)
        return 0;

    for (Entry *ptr = hash_table->buckets[slot]; ptr != NULL ; ptr = ptr->next) {

        if (string_equal(ptr->key, key))
            return 1;
    }

    return 0;
}

void hash_delete(HashTable *hash_table, char *key) {

    if (key == NULL)
        return;

    int slot = hashing_function(key) % hash_table->bucket_number;
    if (hash_table->buckets[slot] == NULL)
        return;

    Entry *ptr = hash_table->buckets[slot];

    if (string_equal(ptr->key, key)) {
        hash_table->buckets[slot] = ptr->next;
        free(ptr);
    }

    for ( ; ptr->next != NULL ; ptr = ptr->next ) {
        if (string_equal(ptr->next->key, key)) {
            Entry *entry_to_delete = ptr->next;
            ptr->next = ptr->next->next;
            free(entry_to_delete);
            return;
        }
    }

    return;
}

HashIterator* hash_iterator_new(HashTable *hash_table) {
    HashIterator *iterator  = malloc(sizeof(HashIterator));
    iterator->buckets       = hash_table->buckets;
    iterator->bucket_number = hash_table->bucket_number;
    iterator->entries       = hash_table->entries;

    iterator->curr_entry = iterator->buckets[0];
    iterator->curr_slot  = 0;
    iterator->passed     = 0;
    return iterator;
}


Entry* hash_iterate(HashIterator *iterator) {
    if (iterator->passed >= iterator->entries)
        return NULL;

    if (iterator->curr_entry != NULL) {
        Entry *entry = iterator->curr_entry;
        iterator->curr_entry = iterator->curr_entry->next;
        iterator->passed++;
        return entry;
    }

    iterator->curr_slot++;

    for (int slot = iterator->curr_slot; slot < iterator->bucket_number; slot++) {
        
        if (iterator->buckets[slot] != NULL) {
            iterator->curr_entry = iterator->buckets[slot]->next;
            iterator->passed++;
            return iterator->buckets[slot];
        }
        iterator->curr_slot++;
    }

    return NULL;
}

void hash_dump(HashTable *hash_table) {
    Entry *ptr;
    printf("----------------------------\n");
    for (int i=0; i < hash_table->bucket_number; i++) {
        printf("[%d]", i);

        for (ptr=hash_table->buckets[i]; ptr != NULL; ptr = ptr->next) {
            printf("->(%s => %s)", ptr->key, ptr->value);
//             printf("->{%s => %s}", ptr->key, ptr->value);
        }

        printf("->NULL\n");
    }
    printf("----------------------------\n");
}

void hash_resize(HashTable *hash_table, int size) {
    Entry **new_buckets = malloc(sizeof(Entry *) * size);
    Entry *ptr, *prev_ptr;
    int slot;
    for (int i=0; i < hash_table->bucket_number; i++) {
            
        for (ptr = hash_table->buckets[i]; ptr != NULL ; ) {

            slot = hashing_function(ptr->key) % size;
            if (new_buckets[slot] == NULL) {
                new_buckets[slot] = ptr;
                prev_ptr = ptr;
                ptr = ptr->next;
                prev_ptr->next = NULL;
            }
            else {
                prev_ptr = ptr;
                ptr = ptr->next;
                prev_ptr->next = new_buckets[slot];
                new_buckets[slot] = prev_ptr;
            }
        }
    }
    free(hash_table->buckets);
    hash_table->buckets = new_buckets;
    hash_table->bucket_number = size;
}

void hash_free(HashTable *hash_table) {
    Entry *ptr, *entry_to_free;
//     printf("SIZE %d\n", hash_table->bucket_number);
    for (int i=0; i < hash_table->bucket_number; i++) {
        for (ptr=hash_table->buckets[i]; ptr != NULL ; ) {
            entry_to_free = ptr;
            ptr = ptr->next;
            free(entry_to_free);
        }
    }
    free(hash_table->buckets);
    free(hash_table);
    hash_table = NULL;
}

