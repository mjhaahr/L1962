//
//  hash.c
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#include "hash.h" //Rem for testing


unsigned long hashCode(const char *s) {
    unsigned long hash = 0;
    int c;
    while ((c = *s++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}
/*
int main(int argc, char **argv){
    unsigned long hash = hashCode(argv[1]);
    printf("HashCode for %s: \t%lu\n", argv[1], hash);
    
    return 0;
}
 */
