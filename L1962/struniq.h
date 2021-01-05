//
//  struniq.h
//  L1962
//
//  Created by Matthew Haahr on 12/19/20.
//

#ifndef struniq_h
#define struniq_h

#include "hashSet.h"

/**
    Checks if a unique string, if not returns the allocated one
 @param s  A pointer to a string, const char to prevent overwriting
 @return The string but allocated if unique or the pointer to a known string if a duplicate
 */
const char *struniq(const char *s);

#endif /* struniq_h */
