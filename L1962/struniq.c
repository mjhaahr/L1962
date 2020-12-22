//
//  struniq.c
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#include "struniq.h"

const char *struniq(const char *s){
    const char *out = get(s);
    if (out == NULL){ // Not already seen (e.g. unique)
        out = add(strdup(s));
        //printf("%p\t", out);
        //printf("Unique\t");
        return out;
    } else { // In map // Seen alreay (pull from list)
        //printf("%p\t", out);
        //printf("Found\t");
        return out;
    }
}
