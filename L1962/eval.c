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
    
    addBuiltin("null", ApplyNOT);
    addBuiltin("nil?", ApplyNOT);
    addBuiltin("not", ApplyNOT);
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
        { // Need quote, set!, lambda, env, define, if, and, or
            // Special Forms and Macros
            const char *sym = car(sexpr).symbol;
            if (sym == sym_QUOTE) {
                assert(cddr(sexpr).type == NIL);
                return cadr(sexpr);
            } else if (sym == sym_SETBang) {
                check(cadr(sexpr).type == SYMBOL);
                return evalSETBang(cadr(sexpr), eval(car(cddr(sexpr)), env), env);
            } else if (sym == sym_LAMBDA) {
                return lambdaToSExpr(cadr(sexpr), cddr(sexpr));
            } else if (sym == sym_LET) {
                return evalLet(cadr(sexpr), cddr(sexpr), env);
            } else if (sym == sym_DEFINE) {
                return evalDefine(cadr(sexpr), cddr(sexpr));
            } else if (sym == sym_DEFUN) {
                return evalDEFUN(cadr(sexpr), car(cddr(sexpr)), cdr(cddr(sexpr)));
            } else if (sym == sym_DEFVAR){
                return evalDEFVAR(cadr(sexpr), cddr(sexpr));
            } else if (sym == sym_IF) {
                if (cdr(cddr(sexpr)).type == NIL) {
                    return evalIf(cadr(sexpr), car(cddr(sexpr)), NILObj, env);
                } else {
                    return evalIf(cadr(sexpr), car(cddr(sexpr)), cadr(cddr(sexpr)), env);
                }
            } else if (sym == sym_AND) {
                check(!isNIL(cddr(sexpr)));       // Must be a two+ element list
                return ApplyAND(cdr(sexpr), env);
            } else if (sym == sym_OR) {
                check(!isNIL(cddr(sexpr)));       // Must be a two+ element list
                return ApplyOR(cdr(sexpr), env);
            }
            
            
            // Evaluate the functions
            SExpr first = eval(car(sexpr), env);
            
            // Evaluate all the arguments
            SExpr args = evalList(cdr(sexpr), env);
            
            // Apply functions
            if (first.type == LAMBDA) {
                return evalLambda(*first.lambda, args, env);
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

void addBuiltin(const char *name, SExpr (*apply)(SExpr args)) {
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

SExpr evalSETBang(SExpr name, SExpr value, SExpr env) {
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
    return name;
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

SExpr evalLambda(Lambda lambda, SExpr args, SExpr env) {
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

SExpr evalDefine(SExpr id, SExpr expr) {
    if (isSYMBOL(id)) {
        return evalSETBang(id, car(expr), global);
    } else if (isCONS(id)) {
        SExpr name = car(id);
        SExpr params = cdr(id);
        return evalSETBang(name, lambdaToSExpr(params, expr), global);
    } else {
        fail("Invalid define: id is not of type SYMBOL or type CONS");
    }
}

SExpr evalDEFUN(SExpr name, SExpr params, SExpr expr) {
    return evalSETBang(name, lambdaToSExpr(params, expr), global);
}

SExpr evalDEFVAR(SExpr name, SExpr expr) {
    if(isNIL(expr)) { // Initialize varialbe
        return evalSETBang(name, NILObj, global);
    } else {
        return evalSETBang(name, car(expr), global);
    }
}

SExpr ApplyLENGTH(SExpr args) {
    check(isNIL(cdr(args))); // Must be one element
    return length(car(args));
}

SExpr ApplyEQ(SExpr args) {
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return eq(a, b);
}

SExpr ApplyENV(SExpr args) {
    check(args.type == NIL);
    return global;
}

SExpr evalIf(SExpr condition, SExpr ifTrue, SExpr ifFalse, SExpr env) {
    SExpr evaled = eval(condition, env);
    if(!isNIL(evaled)) {
        return eval(ifTrue, env);
    } else {
        return eval(ifFalse, env);
    }
}

SExpr ApplyGREATER(SExpr args) {
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return greater(a, b);
}

SExpr ApplyGREATEREQ(SExpr args) {
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return greaterEQ(a, b);
}

SExpr ApplyLESS(SExpr args) {
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return less(a, b);
}

SExpr ApplyLESSEQ(SExpr args) {
    check(isNIL(cddr(args)));       // Must be a two element list
    SExpr a = car(args);
    SExpr b = cadr(args);
    return lessEQ(a, b);
}

SExpr ApplyNOT(SExpr arg) {
    check(isNIL(cdr(arg)));
    if (isNIL(car(arg))) {
        return TObj;
    } else {
        return NILObj;
    }
}

SExpr ApplyAND(SExpr args, SExpr env) {
    // does this have a required number of initial terms
    if (args.type == NIL) {
        return TObj;
    }
    if (isNIL(eval(car(args), env))) {
        return NILObj;
    } else {
        return ApplyAND(cdr(args), env);
    }
    
}

SExpr ApplyOR(SExpr args, SExpr env) {
    // does this have a required number of initial terms
    if (args.type == NIL) {
        return NILObj;
    }
    if (isNIL(eval(car(args), env))) {
        return ApplyOR(cdr(args), env);
    } else {
        return TObj;
    }
}

SExpr evalLet(SExpr pairs, SExpr expr, SExpr env) {
    SExpr params = NILObj;
    SExpr args = NILObj;
    for (SExpr current = pairs; !isNIL(current); current = cdr(current)) {
        params = consToSExpr(caar(current), params);
        args = consToSExpr(car(cdar(current)), args);
    }
    Lambda *lambda = makeLambda(params, expr);
    return evalLambda(*lambda, args, env);
}

//SExpr evalLambda(Lambda lambda, SExpr args, SExpr env)
