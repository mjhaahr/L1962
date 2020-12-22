//
//  hashSet.c
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#include "hashSet.h"

unsigned int size = 101;
unsigned int stored = 0;

// The hashed array
const char **set; //initialized in init

void hashInit(void){
    set = malloc(size * sizeof(char*));
    for (int i = 0; i < size; i++){
        set[i] = NULL;
    }
}

void resize(void){
    if (stored >= (size * 3 / 4)){ // Checks how full
        unsigned int oldSize = size;
        size = 2 * oldSize + 1;
        const char **oldSet = set;
        set = malloc(size * sizeof(char*));
        
        for (int i = 0; i < size; i++){ // Refill
            set[i] = NULL;
        }
        stored = 0;
        for (int i = 0; i < oldSize; i++){ // Copy
            if (oldSet[i] != NULL){
                add(oldSet[i]);
            }
        }
        free(oldSet);
        //printf("Resized:\t New Size: %u, Old Size: %u, 3/4 Old Size: %u, Stored %u\t", size, oldSize, (oldSize * 3 / 4), stored);
    }
}

unsigned int hashing(const char *s){
    return (hashCode(s) % size);
}

const char *add(const char *s){
    resize(); // Resize if needed
    unsigned int index = hashing(s);
    while(set[index] != NULL){
        index = (index + 1) % size;
    }
    set[index] = s;
    stored++;
    return s;
}

const char *get(const char *s){
    //printf("get(\"%s\")\thashing = %d\n", s, hashing(s));
    int index = hashing(s);
    while(set[index] != NULL){
        if (strcmp(s, set[index]) == 0){
            return set[index];
        }
        index = (index + 1) % size;
    }
    return NULL;
}
