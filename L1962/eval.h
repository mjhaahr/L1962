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
    Initializes the global environment (global)
 */
void evalInit(void);

/**
    Evaluates the given SExpr as if it were Lisp code
 @param sexpr The SExpr to eval
 @param env The environment to eval to (for local variables), scope
 @return The results of the evaluation
 */
SExpr eval(SExpr sexpr, SExpr env);

/**
    Evaluates the list
 @param c The list to eval
 @param env The environment to eval to (for local variables), scope
 @return The results of the evaluation
 */
SExpr evalList(SExpr c, SExpr env);

/**
    Makes the builtin and attaches it to the environment
 @param name The name of the builtin, resolves the need for sym_* for each builtin
 @param apply The function to apply
 */
void addBuiltin(const char *name, SExpr (*apply)(SExpr args));

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
 @param args The arguments for assoc
 @return The first matching pair
 */
SExpr ApplyASSOC(SExpr args);

/**
    Wrapper for acons (add to front of an a-list)
 @param args The arguments to pass to acons
 @return The updated a-list
 */
SExpr ApplyACONS(SExpr args);

/**
    set! special form eval (assigns variables in the environmet)
 @param name The name for the variable
 @param value The value to assign to the variable, already evaluated
 @param env  The local scope
 @return NIL for now
 */
SExpr evalSETBang(SExpr name, SExpr value, SExpr env);

/**
    Wrapper for set!-car (RPLACA)
 @param args The arguments:
    target: The cons with the value to be replaced
    value: The value to replaced with
 @return NIL for now
 */
SExpr ApplySETCAR(SExpr args);

/**
    Wrapper for set!-cdr (RPLACD)
 @param args The arguments:
    target: The cons with the value to be replaced
    value: The value to replaced with
 @return NIL for now
 */
SExpr ApplySETCDR(SExpr args);

/**
    Wrapper for +
 @param args The list of items to add
 @return The result as an SExpr
 */
SExpr ApplyPLUS(SExpr args);

/**
    Wrapper for -
 @param args The list of items to subtract
 @return The result as an SExpr
 */
SExpr ApplyMINUS(SExpr args);

/**
    Wrapper for *
 @param args The list of items to multiply
 @return The result as an SExpr
 */
SExpr ApplyMULT(SExpr args);

/**
    Wrapper for /
 @param args The list of items to divide
 @return The result as an SExpr
 */
SExpr ApplyDIV(SExpr args);

/**
    lambda special form eval
 @param lambda  The lambda
 @param args  List of arguments
 @param env  The local scope
 @return The result as an SExpr
 */
SExpr evalLambda(Lambda lambda, SExpr args, SExpr env);

/**
    define special form eval (Scheme-sytle)
 @param id  Either variable or function name and parameters
 @param expr The expression
 @return The name of the variable/function
 */
SExpr evalDefine(SExpr id, SExpr expr);

/**
    defun special form eval (Common Lisp-style, (define function))
 @param name  Variable name
 @param params List of parameters
 @param expr The expression for the function
 @return The name of the function
 */
SExpr evalDEFUN(SExpr name, SExpr params, SExpr expr);

/**
    defcar special form eval (Common Lisp-style, (define varaivle))
 @param name  Variable name
 @param expr The expression for the varaible
 @return The name of the variable
 */
SExpr evalDEFVAR(SExpr name, SExpr expr);

/**
    Wrapper for length
 @param args The length of the list, must be one element
 @return The result as an SExpr
 */
SExpr ApplyLENGTH(SExpr args);

/**
    Wrapper for equal
 @param args The equality evaluation, must be two elements, based on lisp equal
 @return The result as an SExpr
 */
SExpr ApplyEQ(SExpr args);


/**
    Wrapper for env
 @param args The arguments, needs to be NIL
 @return The result of env
 */
SExpr ApplyENV(SExpr args);

/**
    If special form eval
 @param condition The condition
 @param ifTrue Eval if true
 @param ifFalse Eval if false
 @param env The environemnt to eval to
 @return The result of the eval
 */
SExpr evalIf(SExpr condition, SExpr ifTrue, SExpr ifFalse, SExpr env);

/**
    > wrapper
 @param args The arguments
 @return The result as an SExpr
 */
SExpr ApplyGREATER(SExpr args);

/**
    >= wrapper
 @param args The arguments
 @return The result as an SExpr
 */
SExpr ApplyGREATEREQ(SExpr args);

/**
    < wrapper
 @param args The arguments
 @return The result as an SExpr
 */
SExpr ApplyLESS(SExpr args);

/**
    <= wrapper
 @param args The arguments
 @return The result as an SExpr
 */
SExpr ApplyLESSEQ(SExpr args);

/**
    not wrapper
 @param arg The one argument to invert
 @return the Result as an SExpr
 */
SExpr ApplyNOT(SExpr arg);

/**
    and macro
 @param args The list of arguments to eval (must be two or more)
 @param env The environemnt to eval to
 @return True if all arguments are true
 */
SExpr ApplyAND(SExpr args, SExpr env);

/**
    or marco
 @param args The list of arguments to eval (must be two or more)
 @param env The environemnt to eval to
 @return True if one argument is true
 */
SExpr ApplyOR(SExpr args, SExpr env);

/**
    Evals let
 @param pairs List of varaible value pairs (as lists)
 @param expr The expression to evaluate and apply the args to
 @param env The environemnt to eval to
 @return The evlauated expression
 */
SExpr evalLet(SExpr pairs, SExpr expr, SExpr env);

#endif /* eval_h */
