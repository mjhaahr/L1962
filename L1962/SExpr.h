//
//  SExpr.h
//  L1962
//
//  Created by Matthew Haahr on 12/23/20.
//

#ifndef SExpr_h
#define SExpr_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "Tokenizer.h"

typedef enum { // SExpression Types
    NIL,    // Nothing
    CONS,
    SYMBOL,
    INT,
    REAL,
    INVALID,
} SExprType;

typedef struct Cons Cons;

typedef struct { // SExpression Struct
    SExprType type;
    union {
        Cons *cons;
        int64_t i;
        double r;
        const char *symbol;
    };
} SExpr;

struct Cons{ // Lisp style list notation
    SExpr car;
    SExpr cdr;
};

typedef struct { // An struct for possible SExpr
    int eof;
    const char *error;
    SExpr sexpr;
} MaybeSExpr;


/**
    Makes a Cons cell
 @param car  First element of a cons cell
 @param cdr  Second element of a cons cell (another cons or empty (NIL))
 @return The created cons cell
 */
Cons *makeCons(SExpr car, SExpr cdr);

/**
    Makes a Cons expr as an SExpr
 @param car  First element of a cons expr
 @param cdr  Second element of a cons expr (another cons or empty (NIL))
 @return The created cons SExpr
 */
SExpr consToSExpr(SExpr car, SExpr cdr);

/**
    Makes an int an SExpr
 @param value The int to convert to an SExpr
 @return The new SExpr
 */
SExpr intToSExpr(int64_t value);

/**
    Makes a real an SExpr
 @param value The real to convert to an SExpr
 @return The new SExpr
 */
SExpr realToSExpr(double value);

/**
    Makes a symbol an SExpr
 @param symbol The symbol to convert to an SExpr
 @return The new SExpr
 */
SExpr symbolToSExpr(const char* symbol);

/**
    Prints an SExpr
 @param expr The SExpr to print
 */
void printSExpr(SExpr expr);

/**
    Returns the name of a SExprType as a string from the enum
 @param type  The SExprType to process
 @return  The SExprType as a string
 */
const char *SExprName(SExprType type);

/**
    Reads an SExpr and only one SExpr
 @param fp   The file pointer for the file to be read and parsed
 @return The next SExpr in the file as a SExpr struct
 */
MaybeSExpr readSExpr(FILE *fp);

#endif /* SExpr_h */
