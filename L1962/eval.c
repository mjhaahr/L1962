//
//  eval.c
//  L1962
//
//  Created by Matthew Haahr on 12/29/20.
//

#include "eval.h"


SExpr eval(SExpr sexpr){
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
            fail("Of Type SYMBOL");
        
        case CONS: // errors
            if (sexpr.cons->car.type != SYMBOL){
                char str[72];
                sprintf(str, "CAR of Eval List is not of Type SYMBOL instead of Type: %s", SExprName(sexpr.cons->car.type));
                fail(str);
            } else if (sexpr.cons->car.symbol == sym_QUOTE){
                assert(sexpr.cons->cdr.cons->cdr.type == NIL);
                return cadr(sexpr);
            }
            SExpr args = evalList(sexpr.cons->cdr);
            if (sexpr.cons->car.symbol == sym_CAR){
                return ApplyCAR(args);
            } else if (sexpr.cons->car.symbol == sym_CDR){
                return ApplyCDR(args);
            } else if (sexpr.cons->car.symbol == sym_CONS){
                return ApplyCons(args);
            } else {
                char str[72];
                sprintf(str, "CAR of Eval List is of Type SYMBOL but no match in Builtin: %s", sexpr.cons->car.symbol);
                fail(str);
            }
            
        default:
            fail("Default Error, Not of Valid SExpr Type");
    }
}

SExpr evalList(SExpr c){
    if (c.type == NIL){
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
