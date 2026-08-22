#include <stdio.h>
#include <stdlib.h>
#include "../../interpreter/interpreter.h"

void print_lexer(Lexer* lexer){
    Token* token = next_token(lexer);
    while(token->type != TOKEN_EOF) {
        switch(token->type){
            case TOKEN_INT:
                printf("TOKEN_INT: %d\n", token->data.i);
                break;
            case TOKEN_IDENT:
                printf("TOKEN_IDENT: %s\n", token->data.ident);
                break;
            case TOKEN_BOOL:
                printf("TOKEN_BOOL\n");
                break;
            case TOKEN_OP:
                printf("TOKEN_OP: %s\n", token->data.op);
                break;
            case TOKEN_LPAREN:
                printf("TOKEN_LPAREN\n");
                break;
            case TOKEN_RPAREN:
                printf("TOKEN_RPAREN\n");
                break;
            case TOKEN_EOF:
                printf("EOF\n");
                break;
        }
        token = next_token(lexer);
    }
}

void output_lexer(Lexer* lexer) {
    printf("{\n");
    Token* token = next_token(lexer);
    while(token->type != TOKEN_EOF) {
        switch(token->type){
            case TOKEN_INT:
                printf("    INT:\"%d\"", token->data.i);
                break;
            case TOKEN_IDENT:
                printf("    IDENT:\"%s\"", token->data.ident);
                break;
            case TOKEN_BOOL:
                printf("    BOOL:\"%s\"", token->data.b == 1 ? "true" : "false");
                break;
            case TOKEN_OP:
                printf("    OP:\"%s\"", token->data.op);
                break;
            case TOKEN_LPAREN:
                printf("\"(\"");
                break;
            case TOKEN_RPAREN:
                printf("\")\"");
                break;
            case TOKEN_EOF:
                printf("\"EOF\"");
                break;
        }
        printf(",\n");
        token = next_token(lexer);
    }
    printf("}\n");
}

void usage() {
    printf("Usage: ./lexer <text-to-lex>\n");
}

int main(int argc, char** argv) {
    if(argc == 1 || argc > 2){
        usage();
        exit(EXIT_FAILURE);
    }
    Lexer* lexer = new_lexer(argv[argc - 1]);
    output_lexer(lexer);
    free_lexer(lexer);
    return 0;
}
