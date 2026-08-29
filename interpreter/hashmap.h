#ifndef HASHMAP_H_
#define HASHMAP_H_



typedef enum {
    VAL_INT,
    VAL_BOOL,
} ValueType;

typedef struct {
    ValueType type;
    union {
        int i;
        int b;
    } data;
} Value;



#define HASH_SIZE 4096


/* The HashMap struct is a generic key-value hashmap with
 * max capacity of HASH_SIZE, there is currently no way to
 * delete elements. Collisions are linearly probed. There
 * is another hashmap HashMap_v which has the exact same
 * functions prefixed suffixed with _v, which is for a 
 * char* - Value pair (from interpreter.h) */


unsigned long hash_function(const char* str);
typedef struct {
    char* key;
    int value;
} Pair;

typedef struct {
    Pair* items;
    int count; 
    int capacity;
} HashMap;

HashMap* new_hashmap();


void append_hashmap(HashMap* map, Pair* pair); 

Pair* search_hashmap(HashMap* map, char* key);

typedef struct {
    char* key;
    Value value;
} Pair_v;

typedef struct {
    Pair_v* items;
    int count; 
    int capacity;
} HashMap_v;

HashMap_v* new_hashmap_v();


void append_hashmap_v(HashMap_v* map, Pair_v* pair);


Pair_v* search_hashmap_v(HashMap_v* map, char* key);

#endif // HASHMAP_H_


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

