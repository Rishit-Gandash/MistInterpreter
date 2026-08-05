#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Values

typedef enum {
    VAL_INT,
    VAL_BOOL,
} ValueType;

typedef struct {
    ValueType type;
    union {
        int i;
        int b;
    } data;
} Value;

int unwrap_int(const Value* v) {
    if(v->type != VAL_INT){
        printf("ERROR: unwrap_int on a non int value");
        exit(EXIT_FAILURE);
    }
    return v->data.i;
}

int unwrap_bool(const Value* v) {
    if(v->type != VAL_BOOL){
        printf("ERROR: unwrap_bool on a non bool value");
        exit(EXIT_FAILURE);
    }


    if(v->data.b != 0 && v->data.b != 1){
        printf("ERROR: bool has a non 0/1 value");
        exit(EXIT_FAILURE);
    }

    return v->data.b;
}

// Expressions


typedef enum {
    UNARY_NEG,
    UNARY_NOT,
} UnaryOp;

typedef enum {
    BINARY_ADD,
    BINARY_SUB,
    BINARY_MUL,
    BINARY_DIV,
    BINARY_LT,
    BINARY_LE,
    BINARY_GT,
    BINARY_GE,
    BINARY_EQ,
    BINARY_NE,
} BinaryOp;

typedef enum {
    EXPR_INT,
    EXPR_BOOL,
    EXPR_VAR,
    EXPR_UNARY,
    EXPR_BINARY
} ExprType;

typedef struct e Expr;

typedef struct e {
    ExprType type;
    union {
        int i;
        int b;
        char* string;
        union {UnaryOp unaryOp; Expr* expr;} unary;
        union {Expr* lexpr; BinaryOp binaryOp; Expr* rexpr;} binary;
    } data;
} Expr;


// Statements

typedef struct {
    union {char* name; Expr* expr;} assign;
    union {char* label;} jump;
    union {char* label; Expr* cond;} ifjump;
    union {char* name;} print;
    union {Expr* expr;} sleep;
} Stmt;

// Lexer

typedef enum {
    TOKEN_INT,
    TOKEN_IDENT,
    TOKEN_BOOL,
    TOKEN_OP,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType type;
    union {
        int i;
        char* ident;
        int b;
        char* op;
    } data;
} Token;

typedef struct {
    int pos;
    int len;
    char* src;
    char chars[];
} Lexer;

Lexer* new_lexer(char* src) {
    int len = strlen(src);
    Lexer* lexer = malloc(sizeof(Lexer) + len + 1);
    lexer->pos = 0;
    lexer->src = src;
    lexer->len = len;
    memcpy(lexer->chars, src, len + 1);
    return lexer;
}

char peek_lexer(Lexer* lexer){
    return lexer->chars[lexer->pos];
}

char bump_lexer(Lexer* lexer) {
    char c = peek_lexer(lexer); 
    lexer->pos++;
    return c;
}

void skip_ws_lexer(Lexer* lexer) {
    while(peek_lexer(lexer) == ' ') {
        bump_lexer(lexer);
    }
}

int is_digit(char ch) {
    return (ch >= '0' && ch <= '9');
}


int is_alpha(char ch) {
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}


Token* next_token(Lexer* lexer) {
    skip_ws_lexer(lexer);
    char ch = bump_lexer(lexer);
    Token* token = calloc(1, sizeof(Token));


    if(ch == EOF) {
        token->type = TOKEN_EOF; 
        return token;
    }


    else if(is_digit(ch)){
        int i = 0;
        while(is_digit(ch)) {
            i = i*10 + (int)(ch - '0');
            ch = bump_lexer(lexer);
        }
        token->type = TOKEN_INT;
        token->data.i = i;
        return token;
    }

    else if(is_alpha(ch) || ch == '_') { 
        char* s = calloc(1, 256);
        int curr = 0;
        s[curr] = ch;
        while(is_alpha(ch) || ch == '_') {
            s[curr] = ch;
            ch = bump_lexer(lexer);
            curr++;
        }
        if(strcmp("true", s) == 0){
            token->type = TOKEN_BOOL;
            token->data.b = 1;
            free(s);
        }

        else if(strcmp("false", s) == 0){
            token->type = TOKEN_BOOL;
            token->data.b = 0;
            free(s);
        }

        else {
            token->type = TOKEN_IDENT;
            token->data.ident = s;
        }

        return token;
    }

    switch (ch) {
        case '(':
            token->type = TOKEN_LPAREN;
            return token;
        case ')':
            token->type = TOKEN_RPAREN;
            return token;
        case '+':
        case '*':
        case '-':
        case '/':
            token->type = TOKEN_OP;
            char* op  = &ch;
            token->data.op = op;
            return token;
        case '=':
        case '!':
        case '<':
        case '>':
        {
            char c = peek_lexer(lexer);
            if(c == '='){
                char* op = &ch;
                op[1] = c;
                token->type = TOKEN_OP;
                token->data.op = op;
            } else {
                token->type = TOKEN_OP;
                token->data.op = &ch;
            }
            return token;
        }
        default:
            printf("%c", ch);
            printf("ERR: unknown char");
            exit(EXIT_FAILURE);
    }

}

void print_lexer(Lexer* lexer){
    Token* token;
    while(peek_lexer(lexer) != '\0') {
        token = next_token(lexer);
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
                printf("TOKEN_OP\n");
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
        bump_lexer(lexer);
    }
}

int main() {

    Lexer* lexer = new_lexer("a = 56566");
    print_lexer(lexer);
}

