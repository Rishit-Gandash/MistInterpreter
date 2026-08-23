#include <stdio.h>
#include <stdlib.h>
#include "../../interpreter/interpreter.h"




void output_expr(Expr* expr) {
    switch(expr->type){
        case EXPR_INT:
            printf("%d ", expr->data.i);
            break;
        case EXPR_BOOL:
            printf("%s ", expr->data.b == 1 ? "true" : "false");
            break;
        case EXPR_VAR: 
            printf("%s ", expr->data.var);
            break;
        case EXPR_BINARY:
            switch(expr->data.binary->binaryOp) {
                case BINARY_ADD:
                    printf("+ ");
                    break;
                case BINARY_SUB:
                    printf("- ");
                    break;
                case BINARY_MUL:
                    printf("* ");
                    break;
                case BINARY_DIV:
                    printf("/ ");
                    break;
                case BINARY_LT:
                    printf("< ");
                    break;
                case BINARY_LE:
                    printf("<=");
                    break;
                case BINARY_GT:
                    printf("> ");
                    break;
                case BINARY_GE:
                    printf(">= ");
                    break;
                case BINARY_EQ:
                    printf("= ");
                    break;
                case BINARY_NE:
                    printf("!= ");
                    break;
            }
            if(expr->data.binary->lexpr != NULL){
                output_expr(expr->data.binary->lexpr);
            }
            if(expr->data.binary->rexpr != NULL){
                output_expr(expr->data.binary->rexpr);
            }
            break;
            case EXPR_UNARY:
                printf("Guff\n");
                break;
    }
}

void usage() {
    printf("Usage: ./parser <text-to-lex>\n");
}

int main(int argc, char** argv) {
    if(argc == 1 || argc > 2){
        usage();
        exit(EXIT_FAILURE);
    }
    Parser* parser = new_parser(argv[argc - 1]);
    output_expr(parse_expr(parser));
    printf("\n");
    free_parser(parser);
    return 0;
}
