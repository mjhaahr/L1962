//
//  Tokenizer.c
//      Tokenizes a given file or input from stdin
//  L1962
//
//  Created by Matthew Haahr on 12/16/20.
//
#include "Tokenizer.h"

static Token unread;
static int unreadPresent = 0;

/**
    Checks if a character is part of the allowed character set for the beginning of a symbol token
 @param c   The character to check if it is in the set for a valid symbol token (leading symbol)
 @return True if in set, false if not
 */
int isSymbol(int c) {
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '+') || (c == '*') || (c == '/') || (c == '%') || (c == '!') || (c == '?') || (c == '&') || (c == '^') || (c == '|') || (c == '<') || (c == '>') || (c == '$') || (c == '=')) {
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
int isSymbolContinue(int c) {
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '+') || (c == '\\') || (c == '-') || (c == '*') || (c == '/') || (c == '%') || (c == '!') || (c == '?') || (c == '&') || (c == '^') || (c == '|') || (c == '<') || (c == '>') || (c == '=')) {
        return 1;
    } else {
        return 0;
    }
}

Token readToken(FILE *fp) {
    
    if (unreadPresent) { //if unread has been called, reset flag and print the unread value
        unreadPresent = 0;
        return unread;
    }
    
    Token token;
    int c = getc(fp);      // Gets next char in fp
    if (c == EOF) {         // Early return if EF
        token.type = TOKEN_END;
        return token;
    }
    
    int index = 0;
    char buf[BUFSIZ] = {0};       // Compiler allocates in a smart way, reallocates it at same location
    
    TokenType id;      // Matching ID for the given char
    
    if(c == '-') { // Special Minus sign case
        buf[index++] = c;
        buf[index] = 0;
        id = TOKEN_SYMBOL;
        c = getc(fp);
        if (isspace(c) || c == ')' || c == '(') { // solo minus sign check
            token.type = TOKEN_SYMBOL;
            token.value.s = struniq(buf);
            ungetc(c, fp);
            return token;
        }
    }
    
    if (isSymbol(c)) {
        id = TOKEN_SYMBOL;
        buf[index++] = c;
        for (c = getc(fp); isSymbolContinue(c); c = getc(fp)) { // While is a matching symbol add to buffer and increment index
            buf[index++] = c;
        }
        ungetc(c, fp);      // Roll position in fp back to allow proper reading
        buf[index] = 0;     // End Token Value
    } else if (c == '.') {
        buf[index++] = c;
        c = getc(fp); // For get next
        if (!((c >= '0') && (c <= '9'))) {
            id = TOKEN_DOT;
            ungetc(c, fp);
            index++;
            token.type = TOKEN_DOT;
            return token;
        } else {
            id = TOKEN_REAL;
            while ((c >= '0') && (c <= '9')) { // Real
                buf[index++] = c;
                c = getc(fp);
            }
            ungetc(c, fp);      // Roll position in fp back to allow proper reading
            buf[index] = 0;     // End Token Value
        }
    } else if (((c >= '0') && (c <= '9'))) {
        id = TOKEN_INT;
        buf[index++] = c;
        int decimal = 1;    // Decimal signifier
        for (c = getc(fp); (((c >= '0') && (c <= '9')) || ((c == '.') && decimal)); c = getc(fp)) { // While is a number or is the first decimal point '.' seen, add to buffer  and increment index
            buf[index++] = c;
            if (c == '.') {  // If decimal is seen (has to be first), change flag and change ID to real
                decimal = 0;
                id = TOKEN_REAL;
            }
        }
        ungetc(c, fp);      // Roll position in fp back to allow proper reading
        buf[index] = 0;     // End Token Value
    } else if (c == '(') {
        index++;
        token.type = TOKEN_OPENP;
        unread = token;
        return token;
    } else if (c == ')') {
        index++;
        token.type = TOKEN_CLOSEP;
        unread = token;
        return token;
    } else if (c == '[') {
        index++;
        token.type = TOKEN_OPENB;
        unread = token;
        return token;
    } else if (c == ']') {
        index++;
        token.type = TOKEN_CLOSEB;
        unread = token;
        return token;
    } else if (c == '\'') {
        index++;
        token.type = TOKEN_QUOTE;
        unread = token;
        return token;
    } else if (c == '"') {
        id = TOKEN_STRING;
        for (c = getc(fp); c != '"'; c = getc(fp)) { // While c is not a closed quote
            if (c == '\\') {    // Escape char, skip and copy
                buf[index++] = getc(fp);
            } else {
                buf[index++] = c;
            }
        }
        buf[index] = 0;     // End Token Value
    } else if (c == '#') {
        token.type = TOKEN_CHAR;
        char slash = getc(fp);
        token.value.c = getc(fp);
        if (slash != '\\') {
            fail("Poorly Formatted Character");
        }
        return token;
    } else {
        token.type = TOKEN_INVALID;
        if (isspace(c)) {    // Valid Token break (whitespace or ';')
            token.value.e = 0;
        } else {            // Invalid Token type (char not valid)
            token.value.e = c;
        }
        
        if (c == ';') {
            for (c = getc(fp);  (c != EOF && c != '\n'); c = getc(fp)); // If ';' then rest of line is comment, keep going until '\n' or EOF is seen
            ungetc(c, fp);      // Roll position in fp back to allow proper reading
            token.value.e = 0;
        }
        while (token.type == TOKEN_INVALID) {
            token = readToken(fp);
            
        }
        return token;
    }
    
    // If it reaches the end of the if without returning, it is of type INT, REAL, or TOKEN_SYMBOL, that is given
    
    
    token.type = id;
    if (id == TOKEN_INT) {             // if INT, atoi()
        token.value.i = atoi(buf);
    } else if (id == TOKEN_REAL) {     // if REAL, atof()
        token.value.r = atof(buf);
    } else if (id == TOKEN_SYMBOL) {   // if SYMBOL, write the buffer (via struniq)
        token.value.s = struniq(buf);
    } else if (id == TOKEN_STRING) {   // if STRING, strdup
        token.value.str = strdup(buf);
    }
    unread = token;
    return token;
}

void unreadToken(Token token) {
    assert(unreadPresent == 0);
    unread = token;
    unreadPresent = 1;
}

void printToken(Token token) {
    switch (token.type) {
        case TOKEN_INVALID:
            if (token.value.e != 0) {
                printf("INVALID Token: %c\n", token.value.e);
            }
            break;
            
        case TOKEN_SYMBOL:
            printf("%s", token.value.s);
            break;
            break;
            
        case TOKEN_INT:
            printf("%lld", token.value.i);
            break;
            break;
            
        case TOKEN_OPENP:
            printf("(");
            break;
            
        case TOKEN_CLOSEP:
            printf(")");
            break;
            
        case TOKEN_OPENB:
            printf("[");
            break;
            
        case TOKEN_CLOSEB:
            printf("]");
            break;
            
        case TOKEN_QUOTE:
            printf("'");
            break;
            
        case TOKEN_REAL:
            printf("%f", token.value.r);
            break;
            
        case TOKEN_DOT:
            printf(".");
            break;
        
        case TOKEN_STRING:
            printf("%s", token.value.str);
            break;
            
        case TOKEN_CHAR:
            printf("#\\%c", token.value.c);
            break;
            
        default:
            break;
    }
}

void printChar(unsigned char c) {
    char *str;
    switch (c) {
        case 0: // Doesn't work, 0 is used as reference else where
            str = "null";
            break;
            
        case 7:
            str = "bell";
            break;
            
        case '\t':
            str = "tab";
            break;
        
        case '\n':
            str = "newline";
            break;
            
        case ' ':
            str = "space";
            break;
            
        default:
        {
            if (isprint(c)) {
                printf("#\\%c", c);
                return;
            } else {
                printf("#\\%03o", c);
                return;
            }
            
        }
    }
    printf("#\\%s", str);
}

const char *tokenName(TokenType type) {
    switch (type) {
        case TOKEN_END:
            return "END";
            break;
        
        case TOKEN_INVALID:
            return "INVALID";
            break;
            
        case TOKEN_SYMBOL:
            return "SYMBOL";
            break;
            
        case TOKEN_INT:
            return "INT";
            break;
            
        case TOKEN_OPENP:
            return "OPEN P";
            break;
            
        case TOKEN_CLOSEP:
            return "CLOSE P";
            break;
            
        case TOKEN_OPENB:
            return "OPEN B";
            break;
            
        case TOKEN_CLOSEB:
            return "CLOSE B";
            break;
            
        case TOKEN_QUOTE:
            return "QUOTE";
            break;
            
        case TOKEN_REAL:
            return "REAL";
            break;
            
        case TOKEN_DOT:
            return "DOT";
            break;
            
        case TOKEN_STRING:
            return "STRING";
            break;
            
        case TOKEN_CHAR:
            return "CHAR";
            break;
            
        default:
            return "INVALID";
            break;
    }
}
