//
//  SExpr.c
//  L1962
//
//  Created by Matthew Haahr on 12/23/20.
//

#include "SExpr.h"

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

void printSExpr(SExpr expr) {
    switch (expr.type) {
        case NIL:
            printf("NIL");
            break;
            
        case CONS:
            printCons(expr);
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

MaybeSExpr readSExpr(FILE *fp) {
    Token token = readToken(fp);
    MaybeSExpr expr;
    expr.eof = 0;
    expr.error = NULL;
    switch (token.type) {
        case TOKEN_END:
            expr.eof = 1;
            break;
            
        case TOKEN_SYMBOL:
            expr.sexpr = symbolToSExpr(token.value.s);
            break;
            
        case TOKEN_INT:
            expr.sexpr = intToSExpr(token.value.i);
            break;
            
        case TOKEN_REAL:
            expr.sexpr = realToSExpr(token.value.r);
            break;
            
        case TOKEN_OPENP:
            token = readToken(fp); // First term or close parens
            if(token.type == TOKEN_CLOSEP) { // Auto Nil;
                expr.sexpr.type = NIL;
            } else {
                unreadToken(token);
                MaybeSExpr car = readSExpr(fp);
                if (car.eof) {
                    fail("Early EOF");
                }
                expr.sexpr = consToSExpr(car.sexpr, makeNIL()); // First
                expr.sexpr.type = CONS;
                SExpr last = expr.sexpr;
                
                token = readToken(fp);
                while(token.type != TOKEN_CLOSEP) {
                    if (token.type == TOKEN_DOT) {
                        MaybeSExpr cdr = readSExpr(fp);
                        if (cdr.eof) {
                            fail("Early EOF");
                        }
                        last.cons->cdr = cdr.sexpr;
                        token = readToken(fp);
                        if (token.type != TOKEN_CLOSEP) {
                            char str[56];
                            sprintf(str, "Invalid Token of type: %s instead of CLOSE P", tokenName(token.type));
                            fail("Early EOF");
                        }
                        unreadToken(token);
                        
                    } else {
                        unreadToken(token);
                        car = readSExpr(fp);
                        if (car.eof) {
                            fail("Early EOF");
                        }
                        last.cons->cdr = consToSExpr(car.sexpr, makeNIL());
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
            expr.sexpr = consToSExpr(quote, consToSExpr(readSExpr(fp).sexpr, makeNIL()));
            
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
const char *sym_CAR = NULL;
const char *sym_CDR = NULL;
const char *sym_CONS = NULL;

void initSExpr(void){
    sym_QUOTE = struniq("quote");
    sym_CAR = struniq("car");
    sym_CDR = struniq("cdr");
    sym_CONS = struniq("cons");
}
