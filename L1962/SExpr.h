//
//  SExpr.h
//  L1962
//
//  Created by Matthew Haahr on 12/23/20.
//

#ifndef SExpr_h
#define SExpr_h

#include <stdint.h>

#include "Tokenizer.h"
#include "try.h"

extern const char *sym_QUOTE; // Quote symbol value
extern const char *sym_CAR; // car symbol value
extern const char *sym_CDR; // cdr symbol value
extern const char *sym_CONS; // cons symbol value
extern const char *sym_ASSOC; // assoc symbol value
extern const char *sym_ACONS; // acons symbol value
extern const char *sym_SETBang; // set! symbol value
extern const char *sym_SETCAR; // set-car! symbol value
extern const char *sym_SETCDR; // set-cdr! symbol value
extern const char *sym_env; // env symbol value
extern const char *sym_LAMBDA; // lambda symbol value
extern const char *sym_PLUS; // + symbol value
extern const char *sym_MINUS; // - symbol value
extern const char *sym_MULT; // * symbol value
extern const char *sym_DIV; // / symbol value
extern const char *sym_LENGTH; // length symbol value

typedef enum { // SExpression Types
    NIL,    // Nothing
    CONS,
    LAMBDA,
    SYMBOL,
    INT,
    REAL,
    INVALID,
    END,
} SExprType;

typedef struct Cons Cons;

typedef struct Lambda Lambda;

typedef struct { // SExpression Struct
    SExprType type;
    union {
        Cons *cons;
        Lambda *lambda;
        int64_t i;
        double r;
        const char *symbol;
    };
} SExpr;

struct Cons{ // Lisp style list notation
    SExpr car;
    SExpr cdr;
};

struct Lambda{ // Lisp style list notation
    SExpr params;
    SExpr exprs;
};

extern const SExpr NILObj; // Const NIL value
extern SExpr TObj;

/**
    Return true if type is NIL
 */
inline int isNIL(SExpr c) {
    return c.type == NIL;
}

/**
    Return true if type is CONS
 */
inline int isCONS(SExpr c) {
    return c.type == CONS;
}

/**
    Return true if type is SYMBOL
 */
inline int isSYMBOL(SExpr c) {
    return c.type == SYMBOL;
}

/**
    Return true if c is a list
 */
inline int isLIST(SExpr c) {
    return isNIL(c) || isCONS(c);
}


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
    Makes a Lambda
 @param params  Lambda parameters
 @param exprs   Lambda expressions
 @return The Lambda
 */
Lambda *makeLambda(SExpr params, SExpr exprs);

/**
    Makes a Lambda as an SExpr
 @param params  Lambda parameters
 @param exprs   Lambda expressions
 @return The Lambda SExpr
 */
SExpr lambdaToSExpr(SExpr params, SExpr exprs);

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
    Makes a NIL SExpr
 @return The new SExpr
 */
SExpr makeNIL(void);

/**
    Prints an SExpr as a heirarchy
 @param expr The SExpr to print
 */
void debugSExpr(SExpr expr);

/**
    Prints an SExpr in list form
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
SExpr readSExpr(FILE *fp);

/**
    car Builtin - gets the first element of the cons
 @param c The cons to get the car of
 @return The car of the cons
 */
SExpr car(SExpr c);

/**
    cdr Builtin - gets the rest of the cons
 @param c The cons to get the cdr of
 @return The cdr of the cons
 */
SExpr cdr(SExpr c);

/**
    cadr Builtin - gets the first element of the rest of the cons
 @param c The cons to get the cadr of
 @return The cadr of the cons
 */
SExpr cadr(SExpr c);

/**
    caar Builtin - gets the first element of the first element of the cons
 @param c The cons to get the caar of
 @return The caar of the cons
 */
SExpr caar(SExpr c);

/**
    cdar Builtin - gets the rest of the first of the cons
 @param c The cons to get the cdar of
 @return The cdar of the cons
 */
SExpr cdar(SExpr c);

/**
    cddr Builtin - gets the rest of the rest of the cons
 @param c The cons to get the cddr of
 @return The cddr of the cons
 */
SExpr cddr(SExpr c);

/**
    SExpr Initializatiosn (for comparisons)
 */
void SExprInit(void);

/**
    length Builtin - gets the length of the list (recursive call)
 @param list The list to find the length of
 @return The length as an SExpr
 */
SExpr length(SExpr list);

#endif /* SExpr_h */
