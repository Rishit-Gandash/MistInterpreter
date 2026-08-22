#include <stdio.h>
#include <stdlib.h>
#include "../../interpreter/interpreter.h"

void output_parser(Parser* p) {
    (void)p;
}

void usage() {
    printf("Usage: ./parser <text-to-lex>\n");
}

int main(int argc, char** argv) {
    if(argc == 1 || argc > 2){
        usage();
        exit(EXIT_FAILURE);
    }
    Parser* parser = new_parser(new_lexer(argv[argc - 1]));
    parse_expr(parser);
    free_parser(parser);
    return 0;
}