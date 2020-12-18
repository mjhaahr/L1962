//
//  Tokenizer.c
//  L1962
//
//  Created by Matthew Haahr on 12/16/20.
//
#include <stdio.h>
#include <string.h>

enum TokenType {
    INVALID = 0,
    SYMBOL = 1,      // [A-Za-z+*/%!?&^|<>][A-Za-z+\-*/%!?&^|<>]*
    NUM = 2,         // -?[0-9]*.[0-9]*
    OPENP = 3,       // (
    CLOSEP = 4,      // )
    QUOTE = 5,       // '
};

int isSymbol(char c){
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '+') || (c == '*') || (c == '/') || (c == '%') || (c == '!') || (c == '?') || (c == '&') || (c == '^') || (c == '|') || (c == '<') || (c == '>')){
        return 1;
    } else{
        return 0;
    }
}

int isSymbolContinue(char c){
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '+') || (c == '\\') || (c == '-') || (c == '*') || (c == '/') || (c == '%') || (c == '!') || (c == '?') || (c == '&') || (c == '^') || (c == '|') || (c == '<') || (c == '>')){
        return 1;
    } else{
        return 0;
    }
}

void readSTDIN(){
    enum TokenType last = INVALID;
    int decimal = 1; //has this num seen a decimal yet
    int index = 0;
    for (;;) {
        char c = getc(stdin);
        if (c == EOF) {
            return;
        }
        char buf[BUFSIZ];
        
        enum TokenType id;
        
        switch (last) { //maybe clean up some of the if checks
            case OPENP:
                //intentional fall through
            case CLOSEP:
                //intentional fall through
            case QUOTE:
                //intentional fall through
            case INVALID:
                decimal = 1;
                if (isSymbol(c)) {
                    id = SYMBOL;
                } else if (((c >= '0') && (c <= '9')) || (c == '-')) {
                    id = NUM;
                } else if (c == '(') {
                    id = OPENP;
                } else if (c == ')') {
                    id = CLOSEP;
                } else if (c == '\'') {
                    id = QUOTE;
                } else {
                    id = INVALID;
                }
                break;
                
            case SYMBOL:
                decimal = 1;
                if (isSymbolContinue(c)){
                    id = SYMBOL; //continue case
                } else if ((c >= '0') && (c <= '9')) {
                    id = NUM;
                } else if (c == '(') {
                    id = OPENP;
                } else if (c == ')') {
                    id = CLOSEP;
                } else if (c == '\'') {
                    id = QUOTE;
                } else {
                    id = INVALID;
                }
                break;
                
            case NUM:
                if ((c >= '0') && (c <= '9')) {
                    id = NUM; //continue case
                } else if (isSymbol(c)) {
                    id = SYMBOL;
                } else if (c == '(') {
                    id = OPENP;
                } else if (c == ')') {
                    id = CLOSEP;
                } else if (c == '\'') {
                    id = QUOTE;
                } else if ((c == '.') && (decimal == 1)) {
                    decimal = 0;
                    id = NUM; //continue case
                } else {
                    id = INVALID;
                }
                break;
                
            default:
                decimal = 1;
                id = INVALID;
                break;
        }
        
        if (id != INVALID && ((id == last) || (index == 0))){
            buf[index] = c;
            index++;
        } else if (id != last){
            buf[index] = 0;
            printf("%d:\t%s\n", last, buf);
            index = 0;
            ungetc(c, stdin);
        } else if (id == INVALID){
            index = 0;
        }
        last = id;
    }
}


int main(int argc, char **argv){
    if (argc == 1){
        readSTDIN();
    } else {
        printf("Add handling for file name input");
    }
    
    return 0;
}
