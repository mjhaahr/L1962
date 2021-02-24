//
//  eval.c
//  L1962
//
//  Created by Matthew Haahr on 12/29/20.
//

#include <stdio.h>

#include "eval.h"

DEFINE_WRAPPER_1(car);
DEFINE_WRAPPER_1(cdr);

DEFINE_WRAPPER_1(not);
DEFINE_WRAPPER_1(cons);

DEFINE_WRAPPER_1(strlength);
DEFINE_WRAPPER_1(str);
DEFINE_WRAPPER_1(strup);
DEFINE_WRAPPER_1(strlow);
DEFINE_WRAPPER_1(stringToList);

DEFINE_WRAPPER_1(Char);
DEFINE_WRAPPER_1(charToInt);
DEFINE_WRAPPER_1(intToChar);
DEFINE_WRAPPER_1(charup);
DEFINE_WRAPPER_1(charlow);

DEFINE_WRAPPER_2(consToSExpr);
DEFINE_WRAPPER_2(assoc);
DEFINE_WRAPPER_2(setcar);
DEFINE_WRAPPER_2(setcdr);

DEFINE_WRAPPER_2(eq);
DEFINE_WRAPPER_2(greater);
DEFINE_WRAPPER_2(greaterEQ);
DEFINE_WRAPPER_2(less);
DEFINE_WRAPPER_2(lessEQ);

DEFINE_WRAPPER_3(acons);

SExpr global = { NIL }; //The global environment

void evalInit(void) {
    global = acons(makeSymbol("nil"), NILObj, global);
    global = acons(makeSymbol("true"), TObj, global);
    global = acons(makeSymbol("t"), TObj, global);
    
    addBuiltin("car", apply_car);
    addBuiltin("cdr", apply_cdr);
    addBuiltin("cons", apply_consToSExpr);
    addBuiltin("set-car!", apply_setcar);
    addBuiltin("set-cdr!", apply_setcdr);
    addBuiltin("assoc", apply_assoc);
    addBuiltin("acons", apply_acons);
    addBuiltin("env", env);
    
    addBuiltin("+", addSExpr);
    addBuiltin("-", subtractSExpr);
    addBuiltin("*", multiplySExpr);
    addBuiltin("/", divideSExpr);
    addBuiltin("equal", apply_eq);
    addBuiltin("=", apply_eq);
    addBuiltin(">", apply_greater);
    addBuiltin(">=", apply_greaterEQ);
    addBuiltin("<", apply_less);
    addBuiltin("<=", apply_lessEQ);
    
    addBuiltin("null", apply_not);
    addBuiltin("nil?", apply_not);
    addBuiltin("empty?", apply_not);
    addBuiltin("not", apply_not);
    
    addBuiltin("listp", apply_cons);
    addBuiltin("cons?", apply_cons);
    
    addBuiltin("string-length", apply_strlength);
    addBuiltin("string?", apply_str);
    addBuiltin("string-upcase", apply_strup);
    addBuiltin("string-downcase", apply_strlow);
    addBuiltin("string-append", evalAppend);
    addBuiltin("substring", evalSubstring);
    addBuiltin("list->string", evalListToString);
    addBuiltin("string->list", apply_stringToList);
    
    addBuiltin("char?", apply_Char);
    addBuiltin("char->integer", apply_charToInt);
    addBuiltin("integer->char", apply_intToChar);
    
    addBuiltin("char-upcase", apply_charup);
    addBuiltin("char-downcase", apply_charlow);
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
            
        case STRING: // Self - Returning
            return sexpr;
            
        case CHAR: // Self - Returning
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
        { // Need quote, set!, lambda, env, define, if, and, or (things that happen before the eval step)
            // Special Forms and Macros
            const char *sym = car(sexpr).symbol;
            if (sym == sym_QUOTE) {
                check(cddr(sexpr).type == NIL);
                return cadr(sexpr);
            } else if (sym == sym_BQUOTE) {
                assert(cddr(sexpr).type == NIL);
                return lookForCommas(cadr(sexpr), env);
            } else if (sym == sym_COMMA) {
                fail("Comma found outside of backquote");
            } else if (sym == sym_SETBang) {
                check(cadr(sexpr).type == SYMBOL);
                return evalSETBang(cadr(sexpr), eval(car(cddr(sexpr)), env), env);
            } else if (sym == sym_LAMBDA) {
                return lambdaToSExpr(cadr(sexpr), cddr(sexpr), env);
            } else if (sym == sym_LET) {
                return evalLet(cadr(sexpr), cddr(sexpr), env);
            } else if (sym == sym_DEFINE) {
                return evalDefine(cadr(sexpr), cddr(sexpr));
            } else if (sym == sym_DEFUN) {
                return evalDEFUN(cadr(sexpr), car(cddr(sexpr)), cdr(cddr(sexpr)));
            } else if (sym == sym_DEFVAR) {
                return evalDEFVAR(cadr(sexpr), cddr(sexpr));
            } else if (sym == sym_IF) {
                if (cdr(cddr(sexpr)).type == NIL) {
                    return evalIf(cadr(sexpr), car(cddr(sexpr)), NILObj, env);
                } else {
                    return evalIf(cadr(sexpr), car(cddr(sexpr)), cadr(cddr(sexpr)), env);
                }
            } else if (sym == sym_COND) {
                return evalCond(cdr(sexpr), env);
            } else if (sym == sym_WHEN) {
                return evalWhen(cadr(sexpr), cddr(sexpr), env);
            } else if (sym == sym_UNLESS) {
                return evalUnless(cadr(sexpr), cddr(sexpr), env);
            } else if (sym == sym_AND) {
                check(!isNIL(cddr(sexpr)));       // Must be a two+ element list
                return evalAnd(cdr(sexpr), env);
            } else if (sym == sym_OR) {
                check(!isNIL(cddr(sexpr)));       // Must be a two+ element list
                return evalOr(cdr(sexpr), env);
            } else if (sym == sym_PROGN || sym == sym_BEGIN) {
                return evalProgn(cdr(sexpr), env);
            } else if (sym == sym_APPLY) {
                return evalApply(cadr(sexpr), cddr(sexpr), env);
            }
            
            
            // Evaluate the functions
            SExpr first = eval(car(sexpr), env);
            
            // Evaluate all the arguments
            SExpr args = evalList(cdr(sexpr), env);
            
            // Apply functions
            if (first.type == LAMBDA) {
                return evalLambda(*first.lambda, args);
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

SExpr lookForCommas(SExpr expr, SExpr env) {
    if (isCONS(expr)) {
        if (car(expr).symbol == sym_COMMA) { // If comma
            return eval(cadr(expr), env);
        } else {
            return consToSExpr(lookForCommas(car(expr), env), lookForCommas(cdr(expr), env));
        }
    } else {
        return expr;
    }
}

void addBuiltin(const char *name, SExpr (*apply)(SExpr args)) {
    SExpr key = makeSymbol(name);
    global = acons(key, makeBuiltin(apply), global);
}

SExpr evalSETBang(SExpr name, SExpr value, SExpr env) {
    check(isSYMBOL(name));
    check(name.symbol != NULL);
    SExpr scopeExisting = assoc(name, env);
    if (!isNIL(scopeExisting)) {
        scopeExisting.cons->cdr = value;
        return name;
    }
    SExpr globalExisting = assoc(name, global);
    if (!isNIL(globalExisting)) {
        globalExisting.cons->cdr = value;
    } else {
        global = acons(name, value, global);
    }
    return name;
}

SExpr evalLambda(Lambda lambda, SExpr args) {
    SExpr param;
    SExpr arg;
    for (param = lambda.params, arg = args; param.type == CONS; param = cdr(param), arg = cdr(arg)) {
        lambda.env = acons(car(param), car(arg), lambda.env);
    }
    if (param.type == NIL) {
        check(arg.type == NIL);
    } else if (param.type == SYMBOL) {
        lambda.env = acons(param, arg, lambda.env);
    } else {
        fail("Illegal type at end of lambda parameter list: %s", SExprName(param.type));
    }
    SExpr result = NILObj;
    for (SExpr expr = lambda.exprs; expr.type != NIL; expr = cdr(expr)) {
        result = eval(car(expr), lambda.env);
    }
    return result;
}

SExpr evalDefine(SExpr id, SExpr expr) {
    if (isSYMBOL(id)) {
        return evalSETBang(id, eval(car(expr), NILObj), global);
    } else if (isCONS(id)) {
        SExpr name = car(id);
        SExpr params = cdr(id);
        return evalSETBang(name, lambdaToSExpr(params, expr, NILObj), global);
    } else {
        fail("Invalid define: id is not of type SYMBOL or type CONS");
    }
}

SExpr evalDEFUN(SExpr name, SExpr params, SExpr expr) {
    return evalSETBang(name, lambdaToSExpr(params, expr, NILObj), global);
}

SExpr evalDEFVAR(SExpr name, SExpr expr) {
    if(isNIL(expr)) { // Initialize variable
        return evalSETBang(name, NILObj, global);
    } else {
        return evalSETBang(name, eval(car(expr), NILObj), global);
    }
}

SExpr env(SExpr args) {
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

SExpr evalCond(SExpr exprs, SExpr env) {
    SExpr result = NILObj;
    SExpr current;
    for (current = exprs; isNIL(eval(car(car(current)), env)); current = cdr(current)) { // Repeat until a statement is true
        if (isNIL(cdr(current))) {
            current = consToSExpr(NILObj, NILObj); // fills out the current buffer, needs a car and a cdr
            break;
        }
    }
    for (current = car(current); !isNIL(current); current = cdr(current)) {
        result = eval(car(current), env);
    }
    return result;
}

SExpr evalWhen(SExpr condition, SExpr exprs, SExpr env) {
    SExpr evaled = eval(condition, env);
    if (!isNIL(evaled)) {
        SExpr result = NILObj;
        for (SExpr current = exprs; !isNIL(current); current = cdr(current)) {
            result = eval(car(current), env);
        }
        return result;
    } else {
        return NILObj;
    }
}

SExpr evalUnless(SExpr condition, SExpr exprs, SExpr env) {
    SExpr evaled = eval(condition, env);
    if (isNIL(evaled)) {
        SExpr result = NILObj;
        for (SExpr current = exprs; !isNIL(current); current = cdr(current)) {
            result = eval(car(current), env);
        }
        return result;
    } else {
        return NILObj;
    }
}

SExpr evalAnd(SExpr args, SExpr env) {
    // does this have a required number of initial terms
    if (args.type == NIL) {
        return TObj;
    }
    if (isNIL(eval(car(args), env))) {
        return NILObj;
    } else {
        return evalAnd(cdr(args), env);
    }
    
}

SExpr evalOr(SExpr args, SExpr env) {
    // does this have a required number of initial terms
    if (args.type == NIL) {
        return NILObj;
    }
    if (isNIL(eval(car(args), env))) {
        return evalOr(cdr(args), env);
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
    Lambda *lambda = makeLambda(params, expr, env);
    return evalLambda(*lambda, args);
}

SExpr evalProgn(SExpr exprs, SExpr env) {
    SExpr result = NILObj;
    for (SExpr current = exprs; !isNIL(current); current = cdr(current)) {
        result = eval(car(current), env);
    }
    return result;
}

SExpr evalApply(SExpr func, SExpr args, SExpr env) {
    args = eval(consToSExpr(makeSymbol("list*"), args), env);
    SExpr applied = consToSExpr(func, args);
    return eval(applied, env);
}
