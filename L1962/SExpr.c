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

const SExpr NILObj  = { NIL };
SExpr TObj  = { SYMBOL };

const char *sym_QUOTE = NULL;
const char *sym_BQUOTE = NULL;
const char *sym_COMMA = NULL;
const char *sym_SETBang = NULL;
const char *sym_LAMBDA = NULL;
const char *sym_DEFINE = NULL;
const char *sym_DEFUN = NULL;
const char *sym_DEFVAR = NULL;
const char *sym_IF = NULL;
const char *sym_COND = NULL;
const char *sym_WHEN = NULL;
const char *sym_UNLESS = NULL;
const char *sym_AND = NULL;
const char *sym_OR = NULL;
const char *sym_LET = NULL;
const char *sym_PROGN = NULL;
const char *sym_BEGIN = NULL;
const char *sym_APPLY = NULL;


void SExprInit(void) {
    sym_QUOTE = struniq("quote");
    sym_BQUOTE = struniq("`");
    sym_COMMA = struniq(",");
    sym_SETBang = struniq("set!");
    sym_LAMBDA = struniq("lambda");
    sym_DEFINE = struniq("define");
    sym_DEFUN = struniq("defun");
    sym_DEFVAR = struniq("defvar");
    sym_IF = struniq("if");
    sym_COND = struniq("cond");
    sym_WHEN = struniq("when");
    sym_UNLESS = struniq("unless");
    sym_AND = struniq("and");
    sym_OR = struniq("or");
    sym_LET = struniq("let");
    sym_PROGN = struniq("progn");
    sym_BEGIN = struniq("begin");
    sym_APPLY = struniq("apply");
    sym_MACRO = struniq("macro");
    
    TObj.symbol = struniq("true");
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
                
            case NIL:
                return TObj;
            
            case LAMBDA:
            {
                SExpr params = eq(a.lambda->params, b.lambda->params);
                SExpr exprs = eq(a.lambda->exprs, b.lambda->exprs);
                if (!isNIL(params) && (!isNIL(exprs))) {
                    return TObj;
                }
            }
                
            case BUILTIN:
                if (a.builtin.apply == b.builtin.apply) {
                    return TObj;
                }
                
            case CONS: // Needs some work
            {
                SExpr firstRes = eq(car(a), car(b));
                SExpr restRes = eq(cdr(a), cdr(b));
                
                if (!isNIL(firstRes) && (!isNIL(restRes))) {
                    return TObj;
                }
            }
                
            case STRING:
                if (strcmp(a.string, b.string) == 0) {
                    return TObj;
                }
                
            case CHAR:
                if (a.c == b.c) {
                    return TObj;
                }
                
            default:
                break;
        }
    } else if (a.type == INT && b.type == REAL) {
        if ((double) a.i == b.r) {
            return TObj;
        }
    } else if (a.type == REAL && b.type == INT) {
        if (a.r == (double) b.i) {
            return TObj;
        }
    }
    return NILObj;
}

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

Lambda *makeLambda(SExpr params, SExpr exprs, SExpr env) {
    Lambda *lambda = malloc(sizeof(Lambda));
    lambda->params = params;
    lambda->exprs = exprs;
    lambda->env = env;
    return lambda;
}

SExpr lambdaToSExpr(SExpr params, SExpr exprs, SExpr env) {
    SExpr expr;
    expr.type = LAMBDA;
    expr.lambda = makeLambda(params, exprs, env);
    return expr;
}

SExpr makeBuiltin(SExpr (*apply)(SExpr args)) {
    SExpr expr;
    expr.type = BUILTIN;
    expr.builtin.apply = apply;
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

SExpr stringToSExpr(const char* str) {
    SExpr expr;
    expr.type = STRING;
    expr.string = strdup(str);
    return expr;
}

SExpr charToSExpr(char c) {
    SExpr expr;
    expr.type = CHAR;
    expr.c = c;
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
            
        case STRING:
            printf("\t%s\n", expr.string);
            break;
            
        case CHAR:
            printChar(expr.c);
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
            
        case STRING:
            printf("%s", expr.string);
            break;
            
        case CHAR:
            printChar(expr.c);
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
            
        case STRING:
            return "STRING";
            break;
        
        case CHAR:
            return "CHAR";
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
        
        case TOKEN_STRING:
            expr = stringToSExpr(token.value.str);
            break;
            
        case TOKEN_CHAR:
            expr = charToSExpr(token.value.c);
            break;
            
        case TOKEN_OPENP:
            expr = readList(fp, TOKEN_CLOSEP, TOKEN_CLOSEB);
            break;
            
        case TOKEN_OPENB:
            expr = readList(fp, TOKEN_CLOSEB, TOKEN_CLOSEP);
            break;
            
        case TOKEN_QUOTE: {
            SExpr quote;
            quote.type = SYMBOL;
            quote.symbol = sym_QUOTE;
            expr = consToSExpr(quote, consToSExpr(readSExpr(fp), NILObj));
            
            break;
            }
        
        case TOKEN_BQUOTE: {
            SExpr quote;
            quote.type = SYMBOL;
            quote.symbol = sym_BQUOTE;
            expr = consToSExpr(quote, consToSExpr(readSExpr(fp), NILObj));
            
            break;
            }
        
        case TOKEN_COMMA: {
            SExpr comma;
            comma.type = SYMBOL;
            comma.symbol = sym_COMMA;
            expr = consToSExpr(comma, consToSExpr(readSExpr(fp), NILObj));
            
            break;
            }
            
        default:
            fail("Default Error, Not of Valid Token Type");
    }
    return expr;
}

SExpr readList(FILE *fp, TokenType expType, TokenType failType){
    SExpr expr;
    Token token = readToken(fp); // First term or close parens
    if(token.type == expType) { // Auto Nil;
        expr.type = NIL;
    } else if (token.type == failType) { // Auto Fail;
        fail("Incorrect closing character");
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
        while(token.type != expType) {
            if (token.type == failType) { // Auto Fail;
                fail("Incorrect closing character");
            }
            if (token.type == TOKEN_DOT) {
                SExpr cdr = readSExpr(fp);
                if (cdr.type == EOF) {
                    fail("Early EOF");
                }
                last.cons->cdr = cdr;
                token = readToken(fp);
                if (token.type != expType) {
                    if (token.type == failType) { // Auto Fail;
                        fail("Incorrect closing character");
                    }
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


SExpr length(SExpr list) {
    check(isLIST(list));
    SExpr len;
    len.type = INT;
    if (isNIL(list)) {
        len.i = 0;
    } else {
        len.i = length(cdr(list)).i + 1;
    }
    return len;
}

SExpr setcar(SExpr target, SExpr value) {
    target.cons->car = value;
    return NILObj;
}

SExpr setcdr(SExpr target, SExpr value) {
    target.cons->cdr = value;
    return NILObj;
}

SExpr assoc(SExpr key, SExpr a_list) {
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

SExpr acons(SExpr key, SExpr value, SExpr a_list) {
    check(isLIST(a_list)); // NIL will occur once unless stuff is removed from the environment
    return consToSExpr(consToSExpr(key, value), a_list);
}

SExpr addSExpr(SExpr args) {
    SExpr result;
    if (args.type == NIL) {
        result.type = INT;
        result.i = 0;
    } else {
        SExpr first = car(args);
        if (first.type != INT && first.type != REAL) {
            fail("Addition of type: %s", SExprName(first.type));
        }
        SExpr rest = addSExpr(cdr(args));
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

SExpr subtractSExpr(SExpr args) {
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
        
        SExpr rest = addSExpr(cdr(args));
        
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

SExpr multiplySExpr(SExpr args) {
    SExpr result;
    if (args.type == NIL) {
        result.type = INT;
        result.i = 1; // Terminating Condition
    } else {
        SExpr first = car(args);
        if (first.type != INT && first.type != REAL) {
            fail("Multiplication of type: %s", SExprName(first.type));
        }
        SExpr rest = multiplySExpr(cdr(args));
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

SExpr divideSExpr(SExpr args) {
    SExpr result;
    if (args.type == NIL) {
        result.type = REAL;
        result.r = 1.0;
    } else {
        SExpr first = car(args);
        if (first.type != INT && first.type != REAL) {
            fail("Division of type: %s", SExprName(first.type));
        }
        SExpr rest = multiplySExpr(cdr(args));
        
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

SExpr greater(SExpr a, SExpr b) {
    if (a.type != INT && a.type != REAL) {
        fail("Addition of type: %s", SExprName(a.type));
    } else if (b.type != INT && b.type != REAL) {
        fail("Addition of type: %s", SExprName(b.type));
    }
    if (b.type == REAL) {
        if (a.type == REAL) {
            if (a.r > b.r) {
                return TObj;
            }
        } else {
            if ((double) a.r > b.r) {
                return TObj;
            }
        }
    } else {
        if (a.type == REAL) {
            if (a.r > (double) b.r) {
                return TObj;
            }
        } else {
            if (a.i > b.i) {
                return TObj;
            }
        }
    }
    return NILObj;
}

SExpr greaterEQ(SExpr a, SExpr b) {
    if (!isNIL(eq(a, b))) {
        return TObj;
    } else {
        return greater(a, b);
    }
}

SExpr less(SExpr a, SExpr b) {
    if (a.type != INT && a.type != REAL) {
        fail("Addition of type: %s", SExprName(a.type));
    } else if (b.type != INT && b.type != REAL) {
        fail("Addition of type: %s", SExprName(b.type));
    }
    if (b.type == REAL) {
        if (a.type == REAL) {
            if (a.r < b.r) {
                return TObj;
            }
        } else {
            if ((double) a.r < b.r) {
                return TObj;
            }
        }
    } else {
        if (a.type == REAL) {
            if (a.r < (double) b.r) {
                return TObj;
            }
        } else {
            if (a.i < b.i) {
                return TObj;
            }
        }
    }
    return NILObj;
}

SExpr lessEQ(SExpr a, SExpr b) {
    if (!isNIL(eq(a, b))) {
        return TObj;
    } else{
        return less(a, b);
    }
}

SExpr not(SExpr arg) {
    if (isNIL(arg)) {
        return TObj;
    } else {
        return NILObj;
    }
}

SExpr cons(SExpr arg) {
    if (isCONS(arg)) {
        return TObj;
    } else {
        return NILObj;
    }
}

SExpr strlength(SExpr arg) {
    if (arg.type != STRING) {
        return stringToSExpr("Not of String Type");
    } else {
        return intToSExpr(strlen(arg.string));
    }
}

SExpr str(SExpr arg) {
    if (arg.type == STRING) {
        return TObj;
    } else {
        return NILObj;
    }
}

SExpr strup(SExpr arg) {
    if (arg.type != STRING) {
        return stringToSExpr("Not of Type STRING");
    } else {
        char buf[BUFSIZ] = {0};
        for (int i = 0; i < strlen(arg.string); i++) {
            buf[i] = toupper(arg.string[i]);
        }
        return stringToSExpr(buf);
    }
}

SExpr strlow(SExpr arg) {
    if (arg.type != STRING) {
        return stringToSExpr("Not of Type STRING");
    } else {
        char buf[BUFSIZ] = {0};
        for (int i = 0; i < strlen(arg.string); i++) {
            buf[i] = tolower(arg.string[i]);
        }
        return stringToSExpr(buf);
    }
}

SExpr evalAppend(SExpr args) {
    if (!isNIL(cdr(args))) {
        SExpr rest = evalAppend(cdr(args));
        return append(car(args), rest);
    } else {
        return car(args);
    }
}

SExpr append(SExpr a, SExpr b) {
    if ((a.type != STRING) || (b.type != STRING)) {
        return stringToSExpr("Not of Type STRING");
    } else {
        int alen = (int) strlen(a.string);
        int blen = (int) strlen(b.string);
        if ((alen + blen) >= BUFSIZ) {
            fail("Strings are too long and exceed BUFSIZ");
        }
        
        char buf[BUFSIZ] = {0};
        for (unsigned int  i = 0; i < alen; i++) {
            buf[i] = a.string[i];
        }
        for (int i = alen; i < (alen + blen); i++) {
            buf[i] = b.string[i - alen];
        }
        return stringToSExpr(buf);
    }
}

SExpr substring(SExpr string, int start, int end) {
    check(string.type == STRING);
    char buf[BUFSIZ] = {0};
    for (int i = start; i < end; i++) {
        buf[i - start] = string.string[i];
    }
    SExpr substring = stringToSExpr(strdup(buf));
    return substring;
}

SExpr evalSubstring(SExpr args){
    SExpr str = car(args);
    SExpr start = cadr(args);
    check(str.type == STRING);
    check(start.type == INT);
    SExpr end;
    if (isNIL(cddr(args))) {
        end.type = INT;
        end.i = strlen(str.string);
    } else {
        end = car(cddr(args));
    }
    
    return substring(str, (int) start.i, (int) end.i);
}

SExpr Char(SExpr arg) {
    if (arg.type == CHAR) {
        return TObj;
    } else {
        return NILObj;
    }
}

SExpr charToInt(SExpr arg) {
    if (arg.type == CHAR) {
        return intToSExpr(arg.c);
    } else {
        return stringToSExpr("Not of Type CHAR");
    }
}

SExpr intToChar(SExpr arg) {
    if (arg.type == INT) {
        return charToSExpr(arg.i);
    } else {
        return stringToSExpr("Not of Type INT");
    }
}

SExpr charup(SExpr arg) {
    if (arg.type == CHAR) {
        arg.c = toupper(arg.c);
        return arg;
    } else {
        return stringToSExpr("Not of Type CHAR");
    }
}

SExpr charlow(SExpr arg) {
    if (arg.type == CHAR) {
        arg.c = tolower(arg.c);
        return arg;
    } else {
        return stringToSExpr("Not of Type CHAR");
    }
}

SExpr evalListToString(SExpr args) {
    if (!isNIL(cdr(args))) {
        SExpr rest = evalListToString(cdr(args));
        char buf[2] = {0};
        buf[0] = car(args).c;
        return append(stringToSExpr(strdup(buf)), rest);
    } else {
        char buf[2] = {0};
        buf[0] = car(args).c;
        return stringToSExpr(strdup(buf));
    }
}

SExpr stringToList(SExpr arg){
    if (arg.type != STRING) {
        return stringToSExpr("Not of Type STRING");
    } else {
        SExpr car = charToSExpr(arg.string[0]);
        if (strlen(arg.string) == 1) {
            return consToSExpr(car, NILObj);
        } else {
            SExpr rest = substring(arg, 1, (int) strlen(arg.string));
            return consToSExpr(car, stringToList(rest));
        }
    }
}
