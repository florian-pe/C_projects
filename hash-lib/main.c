#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash-lib.h"

int main (int argc, char * argv[]) {

//     HashTable* hash = hash_new(4);
    HashTable* hash = hash_custom_new(4, malloc, free);


    hash_insert(hash, "key", "value");
    hash_insert(hash, "key_one", "value1");
    hash_insert(hash, "key_two", "value2");
    hash_insert(hash, "key_three", "value3");
    hash_insert(hash, "key_four", "value4");
    hash_insert(hash, "key_five", "value5");

    hash_dump(hash);
    hash_delete(hash, "key_one");
    hash_delete(hash, "key_four");
    hash_dump(hash);

    Entry *entry = hash_exists(hash, "key_three");

    printf("%d\n", hash_exists_p(hash, "key_three"));
    printf("%d\n", hash_exists_p(hash, "key_thre"));

    hash_free(hash);

	return 0;
}

