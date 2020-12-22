//
//  main.c
//  L1962
//
//  Created by Matthew Haahr on 12/22/20.
//

#include <stdio.h>

#include "Tokenizer.h"


/**
    Reads a file (can be stdin) and reads each token individually and prints them
 @param fp   The file pointer for the file to be read and tokenized
 */
void readFile(FILE *fp){
    for (;;) {
        Token token = readToken(fp);
        if (token.type == TOKEN_END) {
            break;
        }
        if (token.type == TOKEN_INVALID){
            printToken(token);
        } else {
            printf("%s:\t", tokenName(token.type));
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

