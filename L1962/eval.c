//
//  eval.c
//  L1962
//
//  Created by Matthew Haahr on 12/29/20.
//

#include <stdio.h>

#include "eval.h"

SExpr env = { NIL }; //The environment

void evalInit(void) {
    ApplySETBang(symbolToSExpr(struniq("nil")), NILObj);
    ApplySETBang(symbolToSExpr(struniq("true")), consToSExpr(symbolToSExpr(sym_QUOTE), consToSExpr(TObj, NILObj)));
}

SExpr eval(SExpr sexpr) {
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
            SExpr existing = ApplyASSOC(sexpr, env);
            if (!isNIL(existing)) {
                return existing.cons->cdr;
            }
            char str[56];
            sprintf(str, "No Matching Variable Found in Environmet: %s", sexpr.symbol);
            fail(str);
        }
            
        case CONS: // Functions and things
        {
            if (!isSYMBOL(car(sexpr))) {
                char str[72];
                sprintf(str, "CAR of Eval List is not of Type SYMBOL instead of Type: %s", SExprName(car(sexpr).type));
                fail(str);
            }
            
            // Special Forms
            const char *sym = car(sexpr).symbol;
            if (sym == sym_QUOTE) {
                assert(cddr(sexpr).type == NIL);
                return cadr(sexpr);
            } else if (sym == sym_SETBang) {
                check(cadr(sexpr).type == SYMBOL);
                return ApplySETBang(cadr(sexpr), car(cddr(sexpr)));
            }
            
            // Builtin Functions
            SExpr args = evalList(sexpr.cons->cdr);
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
            } else {
                char str[72];
                sprintf(str, "CAR of Eval List is of Type SYMBOL but no match in Builtin: %s", sym);
                fail(str);
            }
        }
            
        default:
            fail("Default Error, Not of Valid SExpr Type");
    }
}

SExpr evalList(SExpr c) {
    if (c.type == NIL) {
        return c;
    } else {
        check(isLIST(cdr(c)));
        return consToSExpr(eval(car(c)), evalList(cdr(c)));
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

SExpr ApplySETBang(SExpr name, SExpr value) {
    check(isSYMBOL(name));
    check(name.symbol != NULL);
    SExpr existing = ApplyASSOC(name, env);
    if (isNIL(existing)) {
        env = ApplyACONS(name, eval(value), env);
    } else {
        existing.cons->cdr = eval(value);
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
