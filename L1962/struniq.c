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
        const char *out = add(s);
        printf("%p\t", out);
        return out;
    } else { // In map // Seen alreay (pull from list)
        printf("%p\t", out);
        return out;
    }
}
