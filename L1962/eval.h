//
//  eval.h
//  L1962
//
//  Created by Matthew Haahr on 12/29/20.
//

#ifndef eval_h
#define eval_h

#include <stdio.h>
#include "SExpr.h"

/**
    Evaluates the given SExpr as if it were Lisp code
 @param sexpr The SExpr to eval
 @return The results of the evaluation
 */
SExpr eval(SExpr sexpr);

/**
    Evaluates the list
 @param c The list to eval
 @return The results of the evaluation
 */
SExpr evalList(SExpr c);

/**
    Makes a dotted pair (cons) of a list with two elements
 @param args The list to make the cons from
 @return The new cons
 */
SExpr ApplyCons(SExpr args);

/**
    Wrapper for a car
 @param args The list take the car of
 @return The car
*/
SExpr ApplyCAR(SExpr args);

/**
    Wrapper for a cdr
 @param args The list take the cdr of
 @return The cdr
*/
SExpr ApplyCDR(SExpr args);

#endif /* eval_h */
