//
//  eval.c
//  L1962
//
//  Created by Matthew Haahr on 12/29/20.
//

#include <stdio.h>

#include "eval.h"

SExpr env = { NIL }; //The environment

void evalInit(void){
    ApplySETBang(symbolToSExpr(struniq("nil")), NILObj);
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
            
        case SYMBOL: // errors
        {
            SExpr existing = ApplyASSOC(sexpr, env);
            if (existing.type != NIL) {
                return existing.cons->cdr;
            }
            char str[56];
            sprintf(str, "No Matching Variable Found in Environmet: %s", sexpr.symbol);
            fail(str);
        }
            
        case CONS: // errors
            if (sexpr.cons->car.type != SYMBOL) {
                char str[72];
                sprintf(str, "CAR of Eval List is not of Type SYMBOL instead of Type: %s", SExprName(sexpr.cons->car.type));
                fail(str);
            } else if (sexpr.cons->car.symbol == sym_QUOTE) {
                assert(sexpr.cons->cdr.cons->cdr.type == NIL);
                return cadr(sexpr);
            } else if (sexpr.cons->car.symbol == sym_SETBang) {
                check(cadr(sexpr).type == SYMBOL);
                return ApplySETBang(cadr(sexpr), car(cddr(sexpr)));
            } else if (sexpr.cons->car.symbol == sym_SETCAR) {
                return ApplySETCAR(cadr(sexpr), car(cddr(sexpr)));
            } else if (sexpr.cons->car.symbol == sym_SETCDR) {
                return ApplySETCDR(cadr(sexpr), car(cddr(sexpr)));
            }
            SExpr args = evalList(sexpr.cons->cdr);
            if (sexpr.cons->car.symbol == sym_CAR) {
                return ApplyCAR(args);
            } else if (sexpr.cons->car.symbol == sym_CDR) {
                return ApplyCDR(args);
            } else if (sexpr.cons->car.symbol == sym_CONS) {
                return ApplyCons(args);
            } else if (sexpr.cons->car.symbol == sym_ASSOC) {
                return ApplyASSOC(args.cons->car, cadr(args));
            } else if (sexpr.cons->car.symbol == sym_ACONS) {
                return (ApplyACONS(car(args), cadr(args), car(cddr(args))));
            } else {
                char str[72];
                sprintf(str, "CAR of Eval List is of Type SYMBOL but no match in Builtin: %s", sexpr.cons->car.symbol);
                fail(str);
            }
            
        default:
            fail("Default Error, Not of Valid SExpr Type");
    }
}

SExpr evalList(SExpr c) {
    if (c.type == NIL) {
        return c;
    } else {
        check(c.cons->cdr.type == CONS || c.cons->cdr.type == NIL);
        return consToSExpr(eval(c.cons->car), evalList(c.cons->cdr));
    }
}

SExpr ApplyCons(SExpr args) {
    // check that args is a list of exactly two elements
    check(cddr(args).type == NIL);
    return consToSExpr(car(args), cadr(args));
}

SExpr ApplyCAR(SExpr args) {
    // check that args is a list of exactly one element
    check(cdr(args).type == NIL);
    return caar(args);
}

SExpr ApplyCDR(SExpr args) {
    // check that args is a list of exactly one element
    check(cdr(args).type == NIL);
    return cdar(args);
}

SExpr ApplyASSOC(SExpr key, SExpr a_list) {
    // find the first key-value pair in a_list with a key that matches key
    for(SExpr list = a_list; list.type != NIL; list = (cdr(list))) {
        check(car(list).type == CONS);
        SExpr SOI = car(list); // SExpr of Interest
        SExpr sKey = car(SOI); // SOI's key
        if(sKey.type == key.type) {
            switch (sKey.type) {
                case INT:
                    if (sKey.i == key.i) {
                        return SOI;
                    }
                    break;
                    
                case REAL:
                    if (sKey.r == key.r) {
                        return SOI;
                    }
                    break;
                    
                case SYMBOL:
                    if (sKey.symbol == key.symbol) {
                        return SOI;
                    }
                    break;
                    
                default:
                    break;
            }
        }
        
    }
    return NILObj;
}

SExpr ApplyACONS(SExpr key, SExpr value, SExpr a_list) {
    check(a_list.type == CONS || a_list.type == NIL); // NIL will occur once unless stuff is removed from the environment
    return consToSExpr(consToSExpr(key, value), a_list);
}

SExpr ApplySETBang(SExpr name, SExpr value) {
    check(name.type == SYMBOL);
    check(name.symbol != NULL);
    SExpr existing = ApplyASSOC(name, env);
    if (existing.type == NIL) {
        env = ApplyACONS(name, eval(value), env);
    } else {
        existing.cons->cdr = eval(value);
    }
    return NILObj;
}

SExpr ApplySETCAR(SExpr target, SExpr value) {
    eval(target).cons->car = eval(value);
    return NILObj;
}

SExpr ApplySETCDR(SExpr target, SExpr value){
    eval(target).cons->cdr = eval(value);
    return NILObj;
}
