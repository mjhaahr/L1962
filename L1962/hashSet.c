//
//  hashSet.c
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#include "hashSet.h"

unsigned int size = 101;

// The hashed array
char **set; //initialized in init

void hashInit(void){
    set = malloc(size);
    for (int i = 0; i < size; i++){
        set[i] = NULL;
    }
}

unsigned int hashing(const char *s){
    return (hashCode(s) % size);
}

const char *add(const char *s){
    char *out = strdup(s);
    unsigned int hash = hashing(s);
    if (set[hash] == NULL){
        set[hash] = out;
    } else { // Collision
        fprintf(stderr, "HashSet Collision\n");
        exit(1);
    }
    return out;
}

const char *get(const char *s){
    char *hashed = set[hashing(s)];
    if (hashed == NULL){
        return NULL;
    } else if (strcmp(s, hashed) == 0){ // If the input is equal to the stored value, return stored, else NULL
        return hashed;
    } else {
        fprintf(stderr, "Hash Get Error, no clue why\n");
        exit(1);
    }
}
