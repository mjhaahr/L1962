//
//  eval.h
//  L1962
//
//  Created by Matthew Haahr on 12/29/20.
//

#ifndef eval_h
#define eval_h

#include "SExpr.h"

#define DEFINE_WRAPPER_1(name) \
    SExpr apply_ ## name(SExpr args) { \
        check(isNIL(cdr(args))); \
        return name(car(args)); \
    }
// check that args is a list of exactly one element

#define DEFINE_WRAPPER_2(name) \
    SExpr apply_ ## name(SExpr args) { \
        check(isNIL(cddr(args))); \
        return name(car(args), cadr(args)); \
    }
// check that args is a list of exactly two elements

#define DEFINE_WRAPPER_3(name) \
    SExpr apply_ ## name(SExpr args) { \
        check(isNIL(cdr(cddr(args)))); \
        return name(car(args), cadr(args), car(cddr(args))); \
    }
// check that args is a list of exactly three elements

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
    set! special form eval (assigns variables in the environmet)
 @param name The name for the variable
 @param value The value to assign to the variable, already evaluated
 @param env  The local scope
 @return NIL for now
 */
SExpr evalSETBang(SExpr name, SExpr value, SExpr env);

/**
    lambda special form eval
 @param lambda  The lambda
 @param args  List of arguments
 @return The result as an SExpr
 */
SExpr evalLambda(Lambda lambda, SExpr args);

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
    Wrapper for env
 @param args The arguments, needs to be NIL
 @return The result of env
 */
SExpr env(SExpr args);

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
    and macro
 @param args The list of arguments to eval (must be two or more)
 @param env The environemnt to eval to
 @return True if all arguments are true
 */
SExpr evalAnd(SExpr args, SExpr env);

/**
    or marco
 @param args The list of arguments to eval (must be two or more)
 @param env The environemnt to eval to
 @return True if one argument is true
 */
SExpr evalOr(SExpr args, SExpr env);

/**
    Evals let
 @param pairs List of varaible value pairs (as lists)
 @param expr The expression to evaluate and apply the args to
 @param env The environemnt to eval to
 @return The evlauated expression
 */
SExpr evalLet(SExpr pairs, SExpr expr, SExpr env);

#endif /* eval_h */
