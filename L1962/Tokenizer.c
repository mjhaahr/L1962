//
//  Tokenizer.c
//      Tokenizes a given file or input from stdin
//  L1962
//
//  Created by Matthew Haahr on 12/16/20.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "struniq.h"

/**
 An Enum for Tokens
 */
enum TokenType {
    END = -1,        // End of a Token
    INVALID = 0,
    SYMBOL = 1,      // [A-Za-z+*/%!?&^|<>][A-Za-z0-9+\-*/%!?&^|<>]*
    INT = 2,         // -?[0-9]*
    OPENP = 3,       // (
    CLOSEP = 4,      // )
    QUOTE = 5,       // '
    REAL = 6,        // -?[0-9]*.[0-9]*
};

/**
    TokenValue Union
 */
typedef union {
    const char *s;  // for SYMBOL
    int64_t i;      // for INT
    double r;       // for REAL
    char e;         // for INVALID (if error)
} TokenValue;

/**
    A Token Struct
 */
typedef struct {
    enum TokenType type;    // Token type
    TokenValue value;       // Token Value
} Token;

/**
    Checks if a character is part of the allowed character set for the beginning of a symbol token
 @param c   The character to check if it is in the set for a valid symbol token (leading symbol)
 @return True if in set, false if not
 */
int isSymbol(int c){
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '+') || (c == '*') || (c == '/') || (c == '%') || (c == '!') || (c == '?') || (c == '&') || (c == '^') || (c == '|') || (c == '<') || (c == '>')){
        return 1;
    } else {
        return 0;
    }
}

/**
    Checks if a character is part of the allowed character set for the contiuation of a symbol token
 @param c   The character to check if it is in the set for a valid symbol token (non-leading symbol)
 @return True if in set, false if not
 */
int isSymbolContinue(int c){
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '+') || (c == '\\') || (c == '-') || (c == '*') || (c == '/') || (c == '%') || (c == '!') || (c == '?') || (c == '&') || (c == '^') || (c == '|') || (c == '<') || (c == '>')){
        return 1;
    } else {
        return 0;
    }
}

/**
    Reads a token and only one token
 @param fp   The file pointer for the file to be read and tokenized
 @return The next Token in the file as a Token struct
 */

Token readToken(FILE *fp) {
    Token token;
    int c = getc(fp);      // Gets next char in fp
    if (c == EOF) {         // Early return if EF
        token.type = END;
        return token;
    }
    
    int index = 0;
    char buf[BUFSIZ];       // Compiler allocates in a smart way, reallocates it at same location
    
    enum TokenType id;      // Matching ID for the given char
    
    if (isSymbol(c)) {
        id = SYMBOL;
        buf[index++] = c;
        for (c = getc(fp); isSymbolContinue(c); c = getc(fp)) { // While is a matching symbol add to buffer and increment index
            buf[index++] = c;
        }
        ungetc(c, fp);      // Roll position in fp back to allow proper reading
        buf[index] = 0;     // End Token Value
    } else if (((c >= '0') && (c <= '9')) || (c == '-')) {
        id = INT;
        buf[index++] = c;
        int decimal = 1;    // Decimal signifier
        for (c = getc(fp); (((c >= '0') && (c <= '9')) || ((c == '.') && decimal)); c = getc(fp)) { // While is a number or is the first decimal point '.' seen, add to buffer  and increment index
            buf[index++] = c;
            if (c == '.'){  // If decimal is seen (has to be first), change flag and change ID to real
                decimal = 0;
                id = REAL;
            }
        }
        ungetc(c, fp);      // Roll position in fp back to allow proper reading
        buf[index] = 0;     // End Token Value
    } else if (c == '(') {
        index++;
        token.type = OPENP;
        return token;
    } else if (c == ')') {
        index++;
        token.type = CLOSEP;
        return token;
    } else if (c == '\'') {
        index++;
        token.type = QUOTE;
        return token;
    } else {
        token.type = INVALID;
        if (isspace(c)){    // Valid Token break (whitespace or ';')
            token.value.e = 0;
        } else {            // Invalid Token type (char not valid)
            token.value.e = c;
        }
        
        if (c == ';'){
            for (c = getc(fp);  (c != EOF && c != '\n'); c = getc(fp)); // If ';' then rest of line is comment, keep going until '\n' or EOF is seen
            ungetc(c, fp);      // Roll position in fp back to allow proper reading
            token.value.e = 0;
        }
        return token;
    }
    
    // If it reaches the end of the if without returning, it is of type INT, REAL, or SYMBOL, that is given
    
    
    token.type = id;
    if (id == INT){             // if INT, atoi()
        token.value.i = atoi(buf);
    } else if (id == REAL){     // if REAL, atof()
        token.value.r = atof(buf);
    } else {                    // else (SYMBOL), write the buffer (via strdup)
        token.value.s = struniq(buf);
    }
    return token;
}


/**
    Prints a token according to style
 @param token   The token to print
 */
void printToken(Token token){
    switch (token.type) {
        case INVALID:
            if (token.value.e != 0){
                printf("INVALID Token: %c\n", token.value.e);
            }
            break;
            
        case SYMBOL:
            printf("%s", token.value.s);
            break;
            break;
            
        case INT:
            printf("%lld", token.value.i);
            break;
            break;
            
        case OPENP:
            printf("(");
            break;
            
        case CLOSEP:
            printf(")");
            break;
            
        case QUOTE:
            printf("'");
            break;
            
        case REAL:
            printf("%f", token.value.r);
            break;
            
        default:
            break;
    }
}

/**
    Reads a file (can be stdin) and reads each token individually and prints them
 @param fp   The file pointer for the file to be read and tokenized
 */
void readFile(FILE *fp){
    for (;;) {
        Token token = readToken(fp);
        if (token.type == END) {
            break;
        }
        if (token.type == INVALID){
            printToken(token);
        } else {
            printf("%d:\t", token.type);
            printToken(token);
            printf("\n");
        }
    }
}

int main(int argc, char **argv){
    hashInit();
    if (argc == 1){     // If one arg, tokenize stdin
        readFile(stdin);
    } else {            // If more than one arg, args past calling arg will be files to tokenize
        for (int i = 1; i < argc; i++){
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                fprintf(stderr, "%s: can't open fp: %s\n", argv[0], argv[1]);
                exit(1);
            }
            printf("Tokens in File: %s\n", argv[i]);
            readFile(fp);
            printf("\n");
            fclose(fp);
        }
    }
    
    return 0;
}
