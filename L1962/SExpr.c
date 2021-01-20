//
//  SExpr.c
//  L1962
//
//  Created by Matthew Haahr on 12/23/20.
//

#include <stdlib.h>
#include <stdio.h>

#include "SExpr.h"

/**
    Duplication
 */
extern inline int isNIL(SExpr c);

extern inline int isCONS(SExpr c);

extern inline int isSYMBOL(SExpr c);

extern inline int isLIST(SExpr c);

Cons *makeCons(SExpr car, SExpr cdr) {
    Cons *cons = malloc(sizeof(Cons));
    cons->car = car;
    cons->cdr = cdr;
    
    return cons;
}

SExpr consToSExpr(SExpr car, SExpr cdr) {
    SExpr expr;
    expr.type = CONS;
    expr.cons = makeCons(car, cdr);
    return expr;
    
}

Lambda *makeLambda(SExpr params, SExpr exprs) {
    Lambda *lambda = malloc(sizeof(Lambda));
    lambda->params = params;
    lambda->exprs = exprs;
    return lambda;
}

SExpr lambdaToSExpr(SExpr params, SExpr exprs) {
    SExpr expr;
    expr.type = LAMBDA;
    expr.lambda = makeLambda(params, exprs);
    return expr;
}

SExpr intToSExpr(int64_t value) {
    SExpr expr;
    expr.type = INT;
    expr.i = value;
    return expr;
}

SExpr realToSExpr(double value) {
    SExpr expr;
    expr.type = REAL;
    expr.r = value;
    return expr;
}

SExpr symbolToSExpr(const char* symbol) {
    SExpr expr;
    expr.type = SYMBOL;
    expr.symbol = symbol;
    return expr;
}

SExpr makeSymbol(const char* symbol) {
    SExpr expr;
    expr.type = SYMBOL;
    expr.symbol = struniq(symbol);
    return expr;
}

SExpr makeNIL(void) {
    SExpr expr;
    expr.type = NIL;
    return expr;
}

void printSExprDepth(SExpr expr, int depth) {
    printf("Type: %s", SExprName(expr.type));
    switch (expr.type) {
        case NIL:
            printf("\n");
            break;
            
        case CONS:
            printf("\n%*scar: ", depth * 4, "");
            printSExprDepth(expr.cons->car, depth + 1);
            printf("%*scdr: ", depth * 4, "");
            printSExprDepth(expr.cons->cdr, depth + 1);
            break;
        
        case LAMBDA:
            printf("Params:\t");
            printSExpr(expr.lambda->params);
            printf("\nExprs:\t");
            printSExpr(expr.lambda->exprs);
            printf("\n");
            break;
            
        case SYMBOL:
            printf("\t%s\n", expr.symbol);
            break;
            
        case INT:
            printf("\t%lld\n", expr.i);
            break;
            
        case REAL:
            printf("\t%f\n", expr.r);
            break;
            
        default:
            printf("\n");
            break;
    }
}

void debugSExpr(SExpr expr) {
    printSExprDepth(expr, 1);
}

/**
 Prints Cons SExpr (private)
 @param expr The Cons SExpr to print
 */
void printCons(SExpr expr) {
    printf("(");
    while (expr.type != NIL) {
        printSExpr(expr.cons->car);
        if (expr.cons->cdr.type != NIL) {
            printf(" ");
            if (expr.cons->cdr.type != CONS) {
                printf(". ");
                printSExpr(expr.cons->cdr);
                break;
            }
        }
        expr = expr.cons->cdr;
    }
    printf(")");
}

/**
 Prints lambda SExpr (private)
 @param expr The lambda SExpr to print
 */
void printLambda(SExpr expr) {
    printf("LAMBDA: Params: ");
    printSExpr(expr.lambda->params);
    printf("\tExprs: ");
    printSExpr(expr.lambda->exprs);
}

/**
 Prints builtin SExpr (private)
 @param expr The builtin SExpr to print
 */
void printBuiltin(SExpr expr) {
    printf("<builtin %p>", expr.builtin.apply);
}

void printSExpr(SExpr expr) {
    switch (expr.type) {
        case NIL:
            printf("NIL");
            break;
            
        case CONS:
            printCons(expr);
            break;
            
        case LAMBDA:
            printLambda(expr);
            break;
            
        case BUILTIN:
            printBuiltin(expr);
            break;
            
        case SYMBOL:
            printf("%s", expr.symbol);
            break;
            
        case INT:
            printf("%lld", expr.i);
            break;
            
        case REAL:
            printf("%f", expr.r);
            break;
            
        default:
            printf("");
            break;
    }
}

const char *SExprName(SExprType type) {
    switch (type) {
        case NIL:
            return "NIL";
            break;
            
        case CONS:
            return "CONS";
            break;
        
        case LAMBDA:
            return "LAMBDA";
            break;
            
        case SYMBOL:
            return "SYMBOL";
            break;
            
        case INT:
            return "INT";
            break;
            
        case REAL:
            return "REAL";
            break;
            
        default:
            return "INVALID";
            break;
    }
}

SExpr readSExpr(FILE *fp) {
    Token token = readToken(fp);
    SExpr expr;
    switch (token.type) {
        case TOKEN_END:
            expr.type = END;
            break;
            
        case TOKEN_SYMBOL:
            expr = symbolToSExpr(token.value.s);
            break;
            
        case TOKEN_INT:
            expr = intToSExpr(token.value.i);
            break;
            
        case TOKEN_REAL:
            expr = realToSExpr(token.value.r);
            break;
            
        case TOKEN_OPENP:
            token = readToken(fp); // First term or close parens
            if(token.type == TOKEN_CLOSEP) { // Auto Nil;
                expr.type = NIL;
            } else {
                unreadToken(token);
                SExpr car = readSExpr(fp);
                if (car.type == EOF) {
                    fail("Early EOF");
                }
                expr = consToSExpr(car, NILObj); // First
                expr.type = CONS;
                SExpr last = expr;
                
                token = readToken(fp);
                while(token.type != TOKEN_CLOSEP) {
                    if (token.type == TOKEN_DOT) {
                        SExpr cdr = readSExpr(fp);
                        if (cdr.type == EOF) {
                            fail("Early EOF");
                        }
                        last.cons->cdr = cdr;
                        token = readToken(fp);
                        if (token.type != TOKEN_CLOSEP) {
                            fail("Invalid Token of type: %s instead of CLOSE P", tokenName(token.type));
                        }
                        unreadToken(token);
                        
                    } else {
                        unreadToken(token);
                        car = readSExpr(fp);
                        if (car.type == END) {
                            fail("Early EOF");
                        }
                        last.cons->cdr = consToSExpr(car, NILObj);
                        last = last.cons->cdr;
                    }
                    token = readToken(fp);
                }
                
            }
            break;
            
        case TOKEN_QUOTE: {
            SExpr quote;
            quote.type = SYMBOL;
            quote.symbol = sym_QUOTE;
            expr = consToSExpr(quote, consToSExpr(readSExpr(fp), NILObj));
            
            break;
            }
            
        default:
            fail("Default Error, Not of Valid Token Type");
    }
    return expr;
}

SExpr car(SExpr c) {
    check(c.type == CONS);
    return c.cons->car;
}

SExpr cdr(SExpr c) {
    check(c.type == CONS);
    return c.cons->cdr;
}

SExpr cadr(SExpr c) {
    return car(cdr(c));
}

SExpr caar(SExpr c) {
    return car(car(c));
}

SExpr cdar(SExpr c) {
    return cdr(car(c));
}

SExpr cddr(SExpr c) {
    return cdr(cdr(c));
}

const char *sym_QUOTE = NULL;
const char *sym_ASSOC = NULL;
const char *sym_ACONS = NULL;
const char *sym_SETBang = NULL;
const char *sym_SETCAR = NULL;
const char *sym_SETCDR = NULL;
const char *sym_env= NULL;
const char *sym_LAMBDA = NULL;
const char *sym_LENGTH = NULL;
const char *sym_DEFINE = NULL;
const char *sym_DEFUN = NULL;
const char *sym_IF = NULL;

const SExpr NILObj  = { NIL };
SExpr TObj  = { SYMBOL };

void SExprInit(void) {
    sym_QUOTE = struniq("quote");
    sym_ASSOC = struniq("assoc");
    sym_ACONS = struniq("acons");
    sym_SETBang = struniq("set!");
    sym_SETCAR = struniq("set-car!");
    sym_SETCDR = struniq("set-cdr!");
    sym_env = struniq("env");
    sym_LAMBDA = struniq("lambda");
    sym_LENGTH = struniq("length");
    sym_DEFINE = struniq("define");
    sym_DEFUN = struniq("defun");
    sym_IF = struniq("if");
    
    TObj.symbol = struniq("true");
}

SExpr length(SExpr list){
    check(isLIST(list));
    SExpr len;
    len.type = INT;
    if (isNIL(list)){
        len.i = 0;
    } else {
        len.i = length(cdr(list)).i + 1;
    }
    return len;
}

SExpr makeBuiltin(SExpr (*apply)(SExpr args)){
    SExpr expr;
    expr.type = BUILTIN;
    expr.builtin.apply = apply;
    return expr;
}
