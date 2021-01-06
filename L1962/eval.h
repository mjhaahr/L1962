//
//  eval.h
//  L1962
//
//  Created by Matthew Haahr on 12/29/20.
//

#ifndef eval_h
#define eval_h

#include "SExpr.h"

/**
    Initializes the environment
 */
void evalInit(void);

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
    Are the two SExprs equal
 @return NILObj or TObj given the equality
 */
SExpr eq(SExpr a, SExpr b);

/**
    Makes a dotted pair (cons) of a list with two elements
 @param args The list to make the cons from
 @return The new cons
 */
SExpr ApplyCONS(SExpr args);

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

/**
    Wrapper for assoc (return the first pair in a-list which has its car equal to key, or nil if no pair on the list matches key.)
 @param key The key to find in the a-list
 @param a_list The a-list to look in
 @return The first matching pair
 */
SExpr ApplyASSOC(SExpr key, SExpr a_list);

/**
    Wrapper for acons (add to front of an a-list)
 @param key The key to add for the pair
 @param value The value to add for the pair
 @param a_list The list to add the pair to the front of
 @return The updated a-list
 */
SExpr ApplyACONS(SExpr key, SExpr value, SExpr a_list);

/**
    Wrapper for set! (assigns variables in the environmet)
 @param name The name for the variable
 @param value The value to assign to the variable (to be evaluated)
 @return NIL for now
 */
SExpr ApplySETBang(SExpr name, SExpr value);

/**
    Wrapper for set!-car (RPLACA)
 @param target The cons with the value to be replaced
 @param value The value to replaced with
 @return NIL for now
 */
SExpr ApplySETCAR(SExpr target, SExpr value);

/**
    Wrapper for set!-cdr (RPLACD)
 @param target The cons with the value to be replaced
 @param value The value to replaced with
 @return NIL for now
 */
SExpr ApplySETCDR(SExpr target, SExpr value);

#endif /* eval_h */
