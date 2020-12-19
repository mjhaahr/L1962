//
//  struniq.c
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#include "struniq.h"

/**
 @param s  A pointer to a string, const char to prevent overwriting
 @return The string but allocated if unique or the pointer to a known string if a duplicate
 */
const char *struniq(const char *s){
    if (s){ // Not already seen (e.g. unique)
        char *out = strdup(s);
        //add to storage
        printf("Unique - strdup'ed");
        return out;
    } else { // Seen alreay (pull from list)
        return NULL;
        printf("Duplicate - pull from data structure);
    }
}
