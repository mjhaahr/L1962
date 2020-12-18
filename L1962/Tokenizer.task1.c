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

/**
 An Enum for Tokens
 */
enum TokenType {
    INVALID = 0,
    SYMBOL = 1,      // [A-Za-z+*/%!?&^|<>][A-Za-z+\-*/%!?&^|<>]*
    INT = 2,         // -?[0-9]*
    OPENP = 3,       // (
    CLOSEP = 4,      // )
    QUOTE = 5,       // '
    REAL = 6,        // -?[0-9]*.[0-9]*
};

/**
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
 @param c   The character to check if it is in the set for a valid symbol token (non-leading symbol)
 @return True if in set, false if not
 */
int isSymbolContinue(int c){
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '+') || (c == '\\') || (c == '-') || (c == '*') || (c == '/') || (c == '%') || (c == '!') || (c == '?') || (c == '&') || (c == '^') || (c == '|') || (c == '<') || (c == '>')){
        return 1;
    } else {
        return 0;
    }
}

/**
 @param fp   The file pointer for the file to be read and tokenized
 */
void readToken(FILE *fp){
    for (;;) {
        char c = getc(fp);      // Gets next char in fp
        if (c == EOF) {         // Early return if EF
            return;
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
        } else if (c == '(') {
            id = OPENP;
            buf[index++] = c;
        } else if (c == ')') {
            id = CLOSEP;
            buf[index++] = c;
        } else if (c == '\'') {
            id = QUOTE;
            buf[index++] = c;
        } else if (c == ';') {  // If ';' then rest of line is comment, keep going until '\n' or EOF is seen
            id = INVALID;
            for (c = getc(fp);  (c != EOF && c != '\n'); c = getc(fp));
            ungetc(c, fp);      // Roll position in fp back to allow proper reading
        } else {
            id = INVALID;
        }
        
        if (id != INVALID){
            buf[index] = 0;         // If valid Token (will be full token b/c of for loops) , add null to buffer and print buffer
            printf("%d:\t%s\n", id, buf);
        } else if (!isspace(c)){    // If the invalid Token is not a space, print the invalid token to stderr
            fprintf(stderr, "INVALID Token: %c\n", c);
        }
    }
}


int main(int argc, char **argv){
    if (argc == 1){     // If one arg, tokenize stdin
        readToken(stdin);
    } else {            // If more than one arg, args past calling arg will be files to tokenize
        for (int i = 1; i < argc; i++){
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                fprintf(stderr, "%s: can't open fp: %s\n", argv[0], argv[1]);
                exit(1);
            }
            printf("Tokens in File: %s\n", argv[i]);
            readToken(fp);
            printf("\n");
            fclose(fp);
        }
    }
    
    return 0;
}
