//
//  struniq.c
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "struniq.h"

const char *struniq(const char *s){
    char buf[(strlen(s) + 1)];
    for(int i = 0; s[i]; i++){
        buf[i] = tolower(s[i]);
    }
    const char *out = get(buf);
    if (out == NULL){ // Not already seen (e.g. unique)
        out = add(strdup(buf));
        return out;
    } else { // In map // Seen alreay (pull from list)
        return out;
    }
}
