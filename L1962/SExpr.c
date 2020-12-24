//
//  SExpr.c
//  L1962
//
//  Created by Matthew Haahr on 12/23/20.
//

#include "SExpr.h"

Cons *makeCons(SExpr car, SExpr cdr){
    Cons *cons = malloc(sizeof(Cons));
    cons->car = car;
    cons->cdr = cdr;
    
    return cons;
}

SExpr consToSExpr(SExpr car, SExpr cdr){
    SExpr expr;
    expr.type = CONS;
    expr.cons = makeCons(car, cdr);
    return expr;
    
}

SExpr intToSExpr(int64_t value){
    SExpr expr;
    expr.type = INT;
    expr.i = value;
    return expr;
}

SExpr realToSExpr(double value){
    SExpr expr;
    expr.type = REAL;
    expr.r = value;
    return expr;
}

SExpr symbolToSExpr(const char* symbol){
    SExpr expr;
    expr.type = SYMBOL;
    expr.symbol = symbol;
    return expr;
}

void printSExprDepth(SExpr expr, int depth){
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

void printSExpr(SExpr expr){
    printSExprDepth(expr, 1);
}

const char *SExprName(SExprType type){
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

MaybeSExpr readSExpr(FILE *fp){
    Token token = readToken(fp);
    MaybeSExpr expr;
    switch (token.type) {
        case TOKEN_END:
            expr.eof = 1;
            expr.error = NULL;
            break;
            
        case TOKEN_SYMBOL:
            expr.sexpr = symbolToSExpr(token.value.s);
            expr.eof = 0;
            expr.error = NULL;
            break;
            
        case TOKEN_INT:
            expr.sexpr = intToSExpr(token.value.i);
            expr.eof = 0;
            expr.error = NULL;
            break;
            
        case TOKEN_REAL:
            expr.sexpr = realToSExpr(token.value.r);
            expr.eof = 0;
            expr.error = NULL;
            break;
            
        case TOKEN_OPENP:
            expr.eof = 0;
            expr.error = NULL;
            token = readToken(fp); // First term or close parens
            if(token.type == TOKEN_CLOSEP){ // Auto Nil;
                expr.sexpr.type = NIL;
            } else {
                unreadToken(token);
                MaybeSExpr car = readSExpr(fp);
                Token dot = readToken(fp); // First term or close parens
                if (dot.type != TOKEN_DOT){
                    expr.sexpr.type = INVALID;
                    expr.error = "No Dot Error, missing separating dot in middle of Cons SExpr";
                    break;
                }
                MaybeSExpr cdr = readSExpr(fp);
                expr.sexpr = consToSExpr(car.sexpr, cdr.sexpr);
                expr.sexpr.type = CONS;
                if (readToken(fp).type != TOKEN_CLOSEP){
                    expr.sexpr.type = INVALID;
                    expr.error = "No Close Parens Error, missing closing parenthesis in Cons SExpr";
                    break;
                }
            }
            break;
            
        default:
            expr.eof = 0;
            expr.sexpr.type = INVALID;
            expr.error = "Default Error, should not reach end of Switch";
            break;
    }
    return expr;
}
