//
//  Tokenizer.h
//      Tokenizes a given file or input from stdin
//  L1962
//
//  Created by Matthew Haahr on 12/22/20.
//

#ifndef Tokenizer_h
#define Tokenizer_h

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "struniq.h"
#include "try.h"


/**
 An Enum for Tokens
 */
typedef enum TokenType {
    TOKEN_END = -1,         // End of a Token
    TOKEN_INVALID = 0,
    TOKEN_SYMBOL = 1,       // [A-Za-z+*/%!?&^|<>][A-Za-z0-9+\-*/%!?&^|<>]*
    TOKEN_INT = 2,          // -?[0-9]*
    TOKEN_OPENP = 3,        // (
    TOKEN_CLOSEP = 4,       // )
    TOKEN_QUOTE = 5,        // '
    TOKEN_REAL = 6,         // -?[0-9]*.[0-9]*
    TOKEN_DOT = 7,          // . with not sign or digit on either side
    TOKEN_OPENB = 8,        // [
    TOKEN_CLOSEB = 9,       // [
    TOKEN_STRING = 10,      // a string
    TOKEN_BQUOTE = 11,      // `
    TOKEN_CHAR = 12,        // Chars
} TokenType;


/**
    TokenValue Union
 */
typedef union {
    const char *s;      // for TOKEN_SYMBOL
    int64_t i;          // for TOKEN_INT
    double r;           // for TOKEN_REAL
    char e;             // for TOKEN_INVALID (if error)
    const char *str;    // for TOKEN_STRING
    unsigned char c;    // for TOKEN_CHAR
} TokenValue;


/**
    A Token Struct
 */
typedef struct {
    TokenType type;     // Token type
    TokenValue value;   // Token Value
} Token;

/**
    Reads a token and only one token
 @param fp   The file pointer for the file to be read and tokenized
 @return The next Token in the file as a Token struct
 */

Token readToken(FILE *fp);

/**
    Unreads a token and only one token
 @param token The token to unread
 */

void unreadToken(Token token);

/**
    Prints a token according to style
 @param token   The token to print
 */
void printToken(Token token);

/**
    Prints a char according to style
 @param c   The char to print
 */
void printChar(unsigned char c);

/**
    Returns the name of a TokenType as a string from the enum
 @param type  The TokenType to process
 @return  The TokenType as a string
 */
const char *tokenName(TokenType type);


#endif /* Tokenizer_h */
