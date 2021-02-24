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

extern const char *sym_QUOTE; // Quote symbol value
extern const char *sym_SETBang; // set! symbol value
extern const char *sym_LAMBDA; // lambda symbol value
extern const char *sym_DEFINE; // define symbol value
extern const char *sym_DEFUN; // defun symbol value
extern const char *sym_DEFVAR; // defvar symbol value
extern const char *sym_IF; // if symbol value
extern const char *sym_COND; // cond symbol value
extern const char *sym_WHEN; // when symbol value
extern const char *sym_UNLESS; // unless symbol value
extern const char *sym_AND; // and symbol value
extern const char *sym_OR; // or symbol value
extern const char *sym_LET; // let symbol value
extern const char *sym_PROGN; // progn symbol value
extern const char *sym_BEGIN; // begin symbol value
extern const char *sym_APPLY; // apply symbol value

typedef enum { // SExpression Types
    NIL,    // Nothing
    CONS,
    LAMBDA,
    BUILTIN,
    SYMBOL,
    INT,
    REAL,
    INVALID,
    STRING,
    END,
    CHAR,
} SExprType;

typedef struct SExpr SExpr;

typedef struct Cons Cons;

typedef struct Lambda Lambda;

typedef struct Builtin Builtin;

struct Builtin {
  SExpr (*apply)(SExpr args);
};

struct SExpr { // SExpression Struct
    SExprType type;
    union {
        Cons *cons;
        Lambda *lambda;
        Builtin builtin;
        int64_t i;
        double r;
        const char *symbol;
        const char *string;
        char c;
    };
};

struct Cons{ // Lisp style list notation
    SExpr car;
    SExpr cdr;
};

struct Lambda{ // Lisp style list notation
    SExpr params;
    SExpr exprs;
    SExpr env; // Support for Lexical scope
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
    SExpr Initializatiosn (for comparisons)
 */
void SExprInit(void);

/**
    Checks if an SExpr is equal to TObj
 @param c The SExpr to check
 @return 1 if equal to TObj, 0 if not
 */
int isTrue(SExpr c);

/**
    Are the two SExprs equal (equivalent lisp expression: equal)
 @return NILObj or TObj given the equality
 */
SExpr eq(SExpr a, SExpr b);


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
 @param env The Lexical Scope of the lambda
 @return The Lambda
 */
Lambda *makeLambda(SExpr params, SExpr exprs, SExpr env);

/**
    Makes a Lambda as an SExpr
 @param params  Lambda parameters
 @param exprs   Lambda expressions
 @param env The Lexical Scope of the lambda
 @return The Lambda SExpr
 */
SExpr lambdaToSExpr(SExpr params, SExpr exprs, SExpr env);

/**
    Makes a builtin SExpr
 @param apply Function to apply as part of builtin
 @return The builtin as an SExpr
 */

SExpr makeBuiltin(SExpr (*apply)(SExpr args));

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
    Makes a symbol an SExpr and struniq's it
 @param symbol The symbol to convert to an SExpr
 @return The new SExpr
 */
SExpr makeSymbol(const char* symbol);

/**
    Makes a NIL SExpr
 @return The new SExpr
 */
SExpr makeNIL(void);

/**
    Makes a string an SExpr and strdup's it
 @param str The string to convert to an SExpr
 @return The new SExpr
 */
SExpr stringToSExpr(const char* str);

/**
    Makes a char an SExpr
 @param c The char to convert to an SExpr
 @return The new SExpr
 */
SExpr charToSExpr(char c);

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
    Deals with bracketing chars
 @param fp   The file pointer for the file to be read and parsed
 @param expType The expected closing type
 @param failType The failing closing type
 @return The next SExpr in the file as a SExpr struct
 */
SExpr readList(FILE *fp, TokenType expType, TokenType failType);

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
    length Builtin - gets the length of the list (recursive call)
 @param list The list to find the length of
 @return The length as an SExpr
 */
SExpr length(SExpr list);

/**
    setcar Builtin - sets the car of the CONS
 @param target The target whose car to set
 @param value The value to set the target's car to
 @return Null
 */
SExpr setcar(SExpr target, SExpr value);

/**
    setcar Builtin - sets the cdr of the CONS
 @param target The target whose cdr to set
 @param value The value to set the target's cdr to
 @return Null
 */
SExpr setcdr(SExpr target, SExpr value);

/**
    assoc builtin (return the first pair in a-list which has its car equal to key, or nil if no pair on the list matches key.)
 @param key The key to find in the a-list
 @param a_list The a-list to look in
 @return The first matching pair
 */
SExpr assoc(SExpr key, SExpr a_list);

/**
    acons builtin (add to front of an a-list)
 @param key The key to add for the pair
 @param value The value to add for the pair
 @param a_list The list to add the pair to the front of
 @return The updated a-list
 */
SExpr acons(SExpr key, SExpr value, SExpr a_list);

/**
    + builtin (add rest to first)
 @param args The SExpr to build the addition sequence from
 @return The answer as an SExpr for 0 terms: 0, for 1 term: itself, for 2+ terms: recursive addition (autoconvert to REAL if necessary)
 */
SExpr addSExpr(SExpr args);

/**
    - builtin (subract rest from first)
 @param args The SExpr to build the subtraction sequence from
 @return The answer as an SExpr for 0 terms: 0, for 1 term: negated itself, for 2+ terms: first minus sum of rest (recursive addition) (autoconvert to REAL if necessary)
 */
SExpr subtractSExpr(SExpr args);

/**
    * builtin (multiply first by rest)
 @param args The SExpr to build the multiplication sequence from
 @return The answer as an SExpr for 0 terms: 1, for 1 term: itself, for 2+ terms: recursive multiplication (autoconvert to REAL if necessary)
 */
SExpr multiplySExpr(SExpr args);

/**
    / builtin (divide first by rest)
 @param args The SExpr to build the division sequence from
 @return The answer as an SExpr for 0 terms: 1, for 1 term: itself, for 2+ terms: first divided by product of rest (recursive multiplication) (autoconvert to REAL if necessary)
 */
SExpr divideSExpr(SExpr args);

/**
    > builtin
 @param a The first term
 @param b  The second term
 @return The answer as an SExpr
 */
SExpr greater(SExpr a, SExpr b);

/**
    >= builtin
 @param a The first term
 @param b  The second term
 @return The answer as an SExpr
 */
SExpr greaterEQ(SExpr a, SExpr b);

/**
    <builtin
 @param a The first term
 @param b  The second term
 @return The answer as an SExpr
 */
SExpr less(SExpr a, SExpr b);

/**
    <= builtin
 @param a The first term
 @param b  The second term
 @return The answer as an SExpr
 */
SExpr lessEQ(SExpr a, SExpr b);

/**
    not
 @param arg The one argument to invert
 @return the Result as an SExpr
 */
SExpr not(SExpr arg);

/**
    cons?
 @param arg The 1 arguments to cons
 @return the Result as an SExpr
 */
SExpr cons(SExpr arg);

/**
    string-legnth
 @param arg The arg to get the legnth of string
 @return the result as an SExpr
 */
SExpr strlength(SExpr arg);

/**
    string?
 @param arg The arg
 @return the result as an SExpr
 */
SExpr str(SExpr arg);

/**
    string-upcase
 @param arg The arg to upcase
 @return the result as an SExpr
 */
SExpr strup(SExpr arg);

/**
    string-downcase
 @param arg The arg to downcase
 @return the result as an SExpr
 */
SExpr strlow(SExpr arg);

/**
    Eval string-append
 @param args The list of strings
 @return The appeneded string
 */
SExpr evalAppend(SExpr args);

/**
    string-append
 @param a The first string
 @param b The second string
 @return the result as an SExpr
 */
SExpr append(SExpr a, SExpr b);

/**
    Eval Substring
 @param args The arguments to sort through
 @return The new substring
 */
SExpr evalSubstring(SExpr args);

/**
    char?
 @param arg The arg
 @return the result as an SExpr
 */
SExpr Char(SExpr arg);

/**
    char->integer
 @param arg The arg
 @return the result as an SExpr
 */
SExpr charToInt(SExpr arg);

/**
    integer->char
 @param arg The arg
 @return the result as an SExpr
 */
SExpr intToChar(SExpr arg);

/**
    char-upcase
 @param arg The arg to upcase
 @return the result as an SExpr
 */
SExpr charup(SExpr arg);

/**
    char-downcase
 @param arg The arg to downcase
 @return the result as an SExpr
 */
SExpr charlow(SExpr arg);
/**
    Eval list->string
 @param args Turns a list of char into a string
 @return The appeneded string
 */
SExpr evalListToString(SExpr args);

/**
    string->list
 @param arg The arg
 @return the result as an SExpr
 */
SExpr stringToList(SExpr arg);

#endif /* SExpr_h */
