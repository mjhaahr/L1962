//
//  hash.h
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#ifndef hash_h
#define hash_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
    Hasing Function
 @param s Input string to hash, uses sdbm algorithm
 @return Hashed string
 */
unsigned long hashCode(const char *s);

#endif /* hash_h */
