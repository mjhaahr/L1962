//
//  hashSet.h
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#ifndef hashSet_h
#define hashSet_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"

/**
    Initialization function
 */
void hashInit(void);

/**
    Resizes hashSet by 2n+1
 */
void resize(void);


/**
    Hasing Function, adapts the hashCode method from hash.h to the space needed
 @param s Input string to hash
 @return Hashed string
 */
unsigned int hashing(const char *s);

/**
    Adds a string to the hashSet
 @param s The String to add to the hashSet
 @return The allocated string
 */
const char *add(const char *s);
/**
    Gets the allocated memory of the string in the hashSet
 @param s The string to get
 @return The string if allocated, NULL if not
 */
const char *get(const char *s);

#endif /* hashSet_h */
