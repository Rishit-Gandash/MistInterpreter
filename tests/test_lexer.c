#include "../interpreter.c"

void print_lexer(Lexer* lexer){
    Token* token;
    token = next_token(lexer);
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
                printf("EOF???\n");
                break;
        }
        token = next_token(lexer);
    }
}

int main() {
    char* tests[] = {
        "a = b + c",
        "((a+b=c))",
        "dfgsdfgbsdcvvbsdf_asdsdfsdfgbhsdfgsdf=aasdasdfvdcfhbdf_asdsdfgsdfg",
        "((a) + (b+c))",
        "a = !false",
        "a = -5", // 6
    };

    for(int i = 0; i < 6; i++) {
        printf("----------%d CURRENT TEST CASE: %s----------\n", i, tests[i]); 
        Lexer* lexer = new_lexer(tests[i]);
        print_lexer(new_lexer(tests[i]));
        free(lexer);
        printf("\n\n");
    }
    return 0;
}
