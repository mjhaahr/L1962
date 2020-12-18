//
//  strdup.c
//  L1962
//
//  Created by Matthew Haahr on 12/17/20.
//

#include "strdup.h"

char *strdup(const char *src){
    char *dst = malloc(strlen(src) + 1);    // Space for length of string plus the null;
    if (dst == NULL){                       // No memory
        return NULL;
    }
    strcpy(dst, src);                       // Copy chars
    return dst;                             // Return dup'ed string
}
