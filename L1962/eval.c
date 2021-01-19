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
    global = ApplyACONS(symbolToSExpr(struniq("nil")), NILObj, global);
    global = ApplyACONS(symbolToSExpr(struniq("true")), TObj, global);
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
            SExpr scopeExisting = ApplyASSOC(sexpr, env);
            if (!isNIL(scopeExisting)) {
                return scopeExisting.cons->cdr;
            }
            SExpr globalExisting = ApplyASSOC(sexpr, global);
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
        {
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
            } else if (car(sexpr).type == LAMBDA) {
                return ApplyLAMBDA(*car(sexpr).lambda, cdr(sexpr), env);
            }
            
            // Evaluate all the arguments
            SExpr args = evalList(cdr(sexpr), env);
            
            // Builtin Functions
            if (sym == sym_CAR) {
                return ApplyCAR(args);
            } else if (sym == sym_CDR) {
                return ApplyCDR(args);
            } else if (sym == sym_CONS) {
                return ApplyCONS(args);
            } else if (sym == sym_ASSOC) {
                return ApplyASSOC(car(args), cadr(args));
            } else if (sym == sym_ACONS) {
                return ApplyACONS(car(args), cadr(args), car(cddr(args)));
            } else if (sym == sym_SETCAR) {
                return ApplySETCAR(car(args), cadr(args));
            } else if (sym == sym_SETCDR) {
                return ApplySETCDR(car(args), cadr(args));
            } else if (sym == sym_env) {
                check(args.type == NIL);
                return global;
            } else if (sym == sym_PLUS) {
                return ApplyPLUS(args);
            } else if (sym == sym_MINUS) {
                return ApplyMINUS(args);
            } else if (sym == sym_MULT) {
                check(cdr(args).type == CONS); // Must be more than one element
                return ApplyMULT(args);
            } else if (sym == sym_DIV) {
                check(cdr(args).type == CONS); // Must be more than one element
                return ApplyDIV(args);
            } else if (sym == sym_LENGTH) {
                check(cdr(args).type == NIL); // Must be one element
                return length(car(args));
            }
            // Evaluate the functions
            
            SExpr first = eval(car(sexpr), env);
            
            if (first.type == LAMBDA) {
                return ApplyLAMBDA(*first.lambda, args, env);
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

SExpr eq(SExpr a, SExpr b) {
    if(a.type == b.type) {
        switch (a.type) {
            case INT:
                if (a.i == b.i) {
                    return TObj;
                }
                break;
                
            case REAL:
                if (a.r == b.r) {
                    return TObj;
                }
                break;
                
            case SYMBOL:
                if (a.symbol == b.symbol) {
                    return TObj;
                }
                break;
                
            default:
                break;
        }
    }
    return NILObj;
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

SExpr ApplyASSOC(SExpr key, SExpr a_list) {
    // find the first key-value pair in a_list with a key that matches key
    for(SExpr list = a_list; !isNIL(list); list = cdr(list)) {
        check(car(list).type == CONS);
        SExpr SOI = car(list); // SExpr of Interest
        if (!isNIL(eq(car(SOI), key))) {
            return SOI;
        }
    }
    return NILObj;
}

SExpr ApplyACONS(SExpr key, SExpr value, SExpr a_list) {
    check(isLIST(a_list)); // NIL will occur once unless stuff is removed from the environment
    return consToSExpr(consToSExpr(key, value), a_list);
}

SExpr ApplySETBang(SExpr name, SExpr value, SExpr env) {
    check(isSYMBOL(name));
    check(name.symbol != NULL);
    SExpr scopeExisting = ApplyASSOC(name, env);
    if (!isNIL(scopeExisting)) {
        scopeExisting.cons->cdr = value;
        return NILObj;
    }
    SExpr globalExisting = ApplyASSOC(name, global);
    if (!isNIL(globalExisting)) {
        globalExisting.cons->cdr = value;
    } else {
        global = ApplyACONS(name, value, global);
    }
    return NILObj;
}

SExpr ApplySETCAR(SExpr target, SExpr value) {
    target.cons->car = value;
    return NILObj;
}

SExpr ApplySETCDR(SExpr target, SExpr value) {
    target.cons->cdr = value;
    return NILObj;
}

SExpr ApplyPLUS(SExpr args) {
    SExpr result;
    if (args.type == NIL) {
        result.type = INT;
        result.i = 0;
    } else {
        SExpr first = car(args);
        if (first.type != INT && first.type != REAL) {
            fail("Addition of type: %s", SExprName(first.type));
        }
        SExpr rest = ApplyPLUS(cdr(args));
        if (rest.type == REAL) {
            result.type = REAL;
            if (first.type == REAL) {
                result.r = first.r + rest.r;
            } else {
                result.r = (double) first.i + rest.r;
            }
        } else {
            if (first.type == REAL) {
                result.type = REAL;
                result.r = first.r + (double) rest.i;
            } else {
                result.type = INT;
                result.i =  first.i + rest.i;
            }
        }
    }
    return result;
}

SExpr ApplyMINUS(SExpr args) {
    SExpr result;
    if (args.type == NIL) {
        result.type = INT;
        result.i = 0;
    } else {
        SExpr first = car(args);
        
        if (first.type != INT && first.type != REAL) { // Type checking
            fail("Subtraction of type: %s", SExprName(first.type));
        }
        
        if (cdr(args).type == NIL) {
            if (first.type == REAL) {
                result.type = REAL;
                result.r = -first.r;
            } else {
                result.type = INT;
                result.i = -first.i;
            }
            return result;
        }
        
        SExpr rest = ApplyPLUS(cdr(args));
        
        if (rest.type == REAL) {
            result.type = REAL;
            if (first.type == REAL) {
                result.r = first.r - rest.r;
            } else {
                result.r = (double) first.i - rest.r;
            }
        } else {
            if (first.type == REAL) {
                result.type = REAL;
                result.r = first.r - (double) rest.i;
            } else {
                result.type = INT;
                result.i =  first.i - rest.i;
            }
        }
    }
    return result;
}

SExpr ApplyMULT(SExpr args) {
    SExpr result;
    if (args.type == NIL) {
        result.type = INT;
        result.i = 1; // Terminating Condition
    } else {
        SExpr first = car(args);
        if (first.type != INT && first.type != REAL) {
            fail("Multiplication of type: %s", SExprName(first.type));
        }
        SExpr rest = ApplyMULT(cdr(args));
        if (rest.type == REAL) {
            result.type = REAL;
            if (first.type == REAL) {
                result.r = first.r * rest.r;
            } else {
                result.r = (double) first.i * rest.r;
            }
        } else {
            if (first.type == REAL) {
                result.type = REAL;
                result.r = first.r * (double) rest.i;
            } else {
                result.type = INT;
                result.i =  first.i * rest.i;
            }
        }
    }
    return result;
}

SExpr ApplyDIV(SExpr args) {
    SExpr result;
    if (args.type == NIL) {
        result.type = REAL;
        result.r = 1.0;
    } else {
        SExpr first = car(args);
        if (first.type != INT && first.type != REAL) {
            fail("Division of type: %s", SExprName(first.type));
        }
        SExpr rest = ApplyMULT(cdr(args));
        
        result.type = REAL;
        
        if (rest.type == REAL) {
            if (first.type == REAL) {
                result.r = first.r / rest.r;
            } else {
                result.r = (double) first.i / rest.r;
            }
        } else {
            if (first.type == REAL) {
                result.r = first.r / (double) rest.i;
            } else {
                result.r = (double) first.i / (double) rest.i;
            }
        }
    }
    return result;
}

// Support multiterm
SExpr ApplyLAMBDA(Lambda lambda, SExpr args, SExpr env) {
    check(length(args).i == length(lambda.params).i);
    for (SExpr param = lambda.params, arg = args;param.type != NIL; param = cdr(param), arg = cdr(arg)) {
        env = ApplyACONS(car(param), car(arg), env);
    }
    SExpr result = NILObj;
    for (SExpr expr = lambda.exprs; expr.type != NIL; expr = cdr(expr)) {
        result = eval(car(expr), env);
    }
    return result;
}
