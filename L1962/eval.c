//
//  eval.c
//  L1962
//
//  Created by Matthew Haahr on 12/29/20.
//

#include <stdio.h>

#include "eval.h"

SExpr global = { NIL }; //The global environment

void evalInit(void) {
    global = acons(makeSymbol("nil"), NILObj, global);
    global = acons(makeSymbol("true"), TObj, global);
    
    addBuiltin("car", ApplyCAR);
    addBuiltin("cdr", ApplyCDR);
    addBuiltin("cons", ApplyCONS);
    addBuiltin("set-car!", ApplySETCAR);
    addBuiltin("set-cdr!", ApplySETCDR);
    addBuiltin("assoc", ApplyASSOC);
    addBuiltin("acons", ApplyACONS);
    addBuiltin("env", ApplyENV);
    
    addBuiltin("+", ApplyPLUS);
    addBuiltin("-", ApplyMINUS);
    addBuiltin("*", ApplyMULT);
    addBuiltin("/", ApplyDIV);
    addBuiltin("length", ApplyLENGTH);
    addBuiltin("equal", ApplyEQ);
    addBuiltin(">", ApplyGREATER);
    addBuiltin(">=", ApplyGREATEREQ);
    addBuiltin("<", ApplyLESS);
    addBuiltin("<=", ApplyLESSEQ);

}

SExpr eval(SExpr sexpr, SExpr env) {
    switch (sexpr.type) {
        case INVALID: // It's an error
            fail("SExpr Error: of INVALID type");
        
        case INT: // Self - Returning
            return sexpr;
        
        case REAL: // Self - Returning
            return sexpr;
            
        case NIL: // Self - Returning
            return sexpr;
            
        case SYMBOL: // Variable Names
        {
            SExpr scopeExisting = assoc(sexpr, env);
            if (!isNIL(scopeExisting)) {
                return scopeExisting.cons->cdr;
            }
            SExpr globalExisting = assoc(sexpr, global);
            if (!isNIL(globalExisting)) {
                return globalExisting.cons->cdr;
            }
            fail("No Matching Variable Found in Environment: %s", sexpr.symbol);
        }
        
        case END:
        {
            SExpr end;
            end.type = END;
            return end;
        }
            
        case CONS: // Functions and things
        { // Need quote, set!, lambda, env, define, if
            // Special Forms
            const char *sym = car(sexpr).symbol;
            if (sym == sym_QUOTE) {
                assert(cddr(sexpr).type == NIL);
                return cadr(sexpr);
            } else if (sym == sym_SETBang) {
                check(cadr(sexpr).type == SYMBOL);
                return ApplySETBang(cadr(sexpr), eval(car(cddr(sexpr)), env), env);
            } else if (sym == sym_LAMBDA) {
                return lambdaToSExpr(cadr(sexpr), cddr(sexpr));
            } else if (sym == sym_DEFINE || (sym == sym_DEFUN)){
                ApplyDEFINE(cadr(sexpr), cddr(sexpr));
                return NILObj;
            } else if (sym == sym_IF){
                return ApplyIF(cadr(sexpr), car(cddr(sexpr)), cadr(cddr(sexpr)), env);
            }
            
            
            // Evaluate the functions
            SExpr first = eval(car(sexpr), env);
            
            // Evaluate all the arguments
            SExpr args = evalList(cdr(sexpr), env);
            
            // Apply functions
            if (first.type == LAMBDA) {
                return ApplyLAMBDA(*first.lambda, args, env);
            } else if (first.type == BUILTIN) {
                return (first.builtin.apply)(args);
            }
            
            
            if (car(sexpr).type == SYMBOL) {
                fail("Function %s has no match", sym);
            } else {
                fail("Function Name not of Type Symbol: %s", SExprName(car(sexpr).type));
            }
                
        }
            
        default:
            fail("Default Error, Not of Valid SExpr Type");
    }
}

SExpr evalList(SExpr c, SExpr env) {
    if (c.type == NIL) {
        return c;
    } else {
        check(isLIST(cdr(c)));
        return consToSExpr(eval(car(c), env), evalList(cdr(c), env));
    }
}

void addBuiltin(const char *name, SExpr (*apply)(SExpr args)){
    SExpr key = makeSymbol(name);
    global = acons(key, makeBuiltin(apply), global);
}

SExpr ApplyCONS(SExpr args) {
    // check that args is a list of exactly two elements
    check(isNIL(cddr(args)));
    return consToSExpr(car(args), cadr(args));
}

SExpr ApplyCAR(SExpr args) {
    // check that args is a list of exactly one element
    check(isNIL(cdr(args)));
    return caar(args);
}

SExpr ApplyCDR(SExpr args) {
    // check that args is a list of exactly one element
    check(isNIL(cdr(args)));
    return cdar(args);
}

SExpr ApplyASSOC(SExpr args) {
    SExpr key = car(args);
    SExpr a_list = cadr(args);
    return assoc(key, a_list);
}

SExpr ApplyACONS(SExpr args) {
    SExpr key = car(args);
    SExpr value= cadr(args);
    SExpr a_list = car(cddr(args));
    
    return acons(key, value, a_list);
}

SExpr ApplySETBang(SExpr name, SExpr value, SExpr env) {
    check(isSYMBOL(name));
    check(name.symbol != NULL);
    SExpr scopeExisting = assoc(name, env);
    if (!isNIL(scopeExisting)) {
        scopeExisting.cons->cdr = value;
        return NILObj;
    }
    SExpr globalExisting = assoc(name, global);
    if (!isNIL(globalExisting)) {
        globalExisting.cons->cdr = value;
    } else {
        global = acons(name, value, global);
    }
    return NILObj;
}

SExpr ApplySETCAR(SExpr args) {
    SExpr target = car(args);
    SExpr value = cadr(args);
    setcar(target, value);
    return NILObj;
}

SExpr ApplySETCDR(SExpr args) {
    SExpr target = car(args);
    SExpr value = cadr(args);
    setcdr(target, value);
    return NILObj;
}

SExpr ApplyPLUS(SExpr args) {
    return addSExpr(args);
}

SExpr ApplyMINUS(SExpr args) {
    return subtractSExpr(args);
}

SExpr ApplyMULT(SExpr args) {
    return multiplySExpr(args);
}

SExpr ApplyDIV(SExpr args) {
    return divideSExpr(args);
}

SExpr ApplyLAMBDA(Lambda lambda, SExpr args, SExpr env) {
    check(length(args).i == length(lambda.params).i);
    for (SExpr param = lambda.params, arg = args;param.type != NIL; param = cdr(param), arg = cdr(arg)) {
        env = acons(car(param), car(arg), env);
    }
    SExpr result = NILObj;
    for (SExpr expr = lambda.exprs; expr.type != NIL; expr = cdr(expr)) {
        result = eval(car(expr), env);
    }
    return result;
}

void ApplyDEFINE(SExpr id, SExpr expr){
    if (isSYMBOL(id)){
        ApplySETBang(id, expr, global);
    } else if (isCONS(id)){
        SExpr name = car(id);
        SExpr params = cdr(id);
        ApplySETBang(name, lambdaToSExpr(params, expr), global);
    } else {
        fail("Invalid define: id is not of type SYMBOL or type CONS");
    }
}

SExpr ApplyLENGTH(SExpr args){
    check(isNIL(cdr(args))); // Must be one element
    return length(car(args));
}

SExpr ApplyEQ(SExpr args){
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return eq(a, b);
}

SExpr ApplyENV(SExpr args){
    check(args.type == NIL);
    return global;
}

SExpr ApplyIF(SExpr condition, SExpr ifTrue, SExpr ifFalse, SExpr env){
    SExpr evaled = eval(condition, env);
    if(isTrue(evaled)){
        return eval(ifTrue, env);
    } else {
        return eval(ifFalse, env);
    }
}

SExpr ApplyGREATER(SExpr args){
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return greater(a, b);
}

SExpr ApplyGREATEREQ(SExpr args){
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return greaterEQ(a, b);
}

SExpr ApplyLESS(SExpr args){
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return less(a, b);
}

SExpr ApplyLESSEQ(SExpr args){
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return lessEQ(a, b);
}
