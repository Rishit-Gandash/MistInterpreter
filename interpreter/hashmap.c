#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

#define HASH_SIZE 4096


/* The HashMap struct is a generic key-value hashmap with
 * max capacity of HASH_SIZE, there is currently no way to
 * delete elements. Collisions are linearly probed. There
 * is another hashmap HashMap_v which has the exact same
 * functions prefixed suffixed with _v, which is for a 
 * char* - Value pair (from interpreter.h) */


unsigned long hash_function(const char* str) {
    unsigned long hash = 0;
    int c;

    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}


HashMap* new_hashmap() {
    HashMap* hashmap = calloc(1, sizeof(HashMap));
    hashmap->items = calloc(HASH_SIZE, sizeof(Pair));
    hashmap->capacity = HASH_SIZE;
    hashmap->count = 0;
    return hashmap;
}

void append_hashmap(HashMap* map, Pair* pair) {
    unsigned long hash = hash_function(pair->key);
    hash = hash % map->capacity;
    int seen_end = 0;
    while(map->items[hash].key){
        hash++;
        if(seen_end == 1){
            printf("No space left broski\n");
            exit(EXIT_FAILURE);
        }
        if(hash >= HASH_SIZE - 1){
            hash = hash % HASH_SIZE;
            seen_end = 1;
        }
    }
    map->items[hash] = *pair;
    free(pair);
    
}

Pair* search_hashmap(HashMap* map, char* key) {
    unsigned long hash = hash_function(key);
    hash = hash % map->capacity; 
    if(!map->items[hash].key){
        printf("No such item\n");
        return NULL;
    }
    int seen_end = 0;
    while(strcmp(key, map->items[hash].key) != 0)
    {
        hash++;
        if(seen_end == 1){
            printf("No such item\n");
            return NULL;
        }
        if(hash >= HASH_SIZE - 1){
            hash = hash % HASH_SIZE;
            seen_end = 1;
        }
    }
    return &map->items[hash];
}





HashMap_v* new_hashmap_v() {
    HashMap_v* hashmap = calloc(1, sizeof(HashMap_v));
    hashmap->items = calloc(HASH_SIZE, sizeof(Pair_v));
    hashmap->capacity = HASH_SIZE;
    hashmap->count = 0;
    return hashmap;
}

void append_hashmap_v(HashMap_v* map, Pair_v* pair) {
    unsigned long hash = hash_function(pair->key);
    hash = hash % map->capacity;
    int seen_end = 0;
    while(map->items[hash].key){
        hash++;
        if(seen_end == 1){
            printf("No space left\n");
            exit(EXIT_FAILURE);
        }
        if(hash >= HASH_SIZE - 1){
            hash = hash % HASH_SIZE;
            seen_end = 1;
        }
    }
    map->items[hash] = *pair;
    free(pair);
}

Pair_v* search_hashmap_v(HashMap_v* map, char* key) {
    unsigned long hash = hash_function(key);
    hash = hash % map->capacity; 
    if(!map->items[hash].key){
        printf("No such item\n");
        return NULL;
    }
    int seen_end = 0;
    while(strcmp(key, map->items[hash].key) != 0)
    {
        hash++;
        if(seen_end == 1){
            printf("No such item\n");
            return NULL;
        }
        if(hash >= HASH_SIZE - 1){
            hash = hash % HASH_SIZE;
            seen_end = 1;
        }
    }
    return &map->items[hash];
}



// int main() {
//     HashMap* map = new_hashmap();
//     Pair* pair = malloc(sizeof(Pair));
//     pair->key = "";
//     pair->value = 25;
//     append_hashmap(map, pair);
//     pair->key = "Sus";
//     pair->value = 35;
//     append_hashmap(map, pair);
//     pair->key = "Baka";
//     pair->value = 123123;
//     append_hashmap(map, pair);
//     pair->key = "they dont know me son";
//     pair->value = 3453;
//     append_hashmap(map, pair);
//     pair->key = "six oneee";
//     pair->value = 4;
//     append_hashmap(map, pair);
//     Pair* test = search_hashmap(map, "");
//     printf("%d: %s\n", test->value, test->key);
//     test = search_hashmap(map, "Sus");
//     printf("%d: %s\n", test->value, test->key);
//     test = search_hashmap(map, "Baka");
//     printf("%d: %s\n", test->value, test->key);
//     test = search_hashmap(map, "they dont know me son");
//     printf("%d: %s\n", test->value, test->key);
//     test = search_hashmap(map, "six oneee");
//     printf("%d: %s\n", test->value, test->key);
//
//     test = search_hashmap(map, "bullshit");
//     if(test != NULL)
//         printf("%d: %s\n", test->value, test->key);
//     return 0;
// }
