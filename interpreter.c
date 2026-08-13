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
        char* var;
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
    char chars[];
} Lexer;

Lexer* new_lexer(char* src) {
    int len = strlen(src);
    Lexer* lexer = malloc(sizeof(Lexer) + len + 1);
    lexer->pos = 0;
    lexer->len = len;
    memcpy(lexer->chars, src, len + 1);
    return lexer;
}


char peek_lexer(Lexer* lexer){
    return lexer->chars[lexer->pos];
}

char bump_lexer(Lexer* lexer) {
    char c = peek_lexer(lexer); 
    lexer->pos+=1;
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
    char ch = peek_lexer(lexer);
    Token* token = calloc(1, sizeof(Token));


    if(ch == EOF || ch == '\0') {
        token->type = TOKEN_EOF; 
        return token;
    }


    else if(is_digit(ch)){
        int i = 0;
        while(is_digit(peek_lexer(lexer))) {
            ch = bump_lexer(lexer);
            i = i*10 + (int)(ch - '0');
        }
        token->type = TOKEN_INT;
        token->data.i = i;
        return token;
    }

    else if(is_alpha(ch) || ch == '_') { 
        char* s = calloc(1, 256);
        int curr = 0;
        while(is_alpha(peek_lexer(lexer)) || peek_lexer(lexer) == '_' || is_digit(peek_lexer(lexer))) {
            ch = bump_lexer(lexer);
            s[curr] = ch;
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
            bump_lexer(lexer);
            return token;
        case ')':
            token->type = TOKEN_RPAREN;
            bump_lexer(lexer);
            return token;
        case '+':
        case '*':
        case '-':
        case '/':
            // token->type = TOKEN_OP;
            // char* op  = &ch;
            // token->data.op = op;
            // bump_lexer(lexer);
            // return token;
        case '=':
        case '!':
        case '<':
        case '>':
        {
            bump_lexer(lexer);
            char c = peek_lexer(lexer);
            if(c == '='){
                char* op = calloc(3, sizeof(char)); // op + null term
                op[0] = ch;
                op[1] = c;
                op[2] = '\0';
                token->type = TOKEN_OP;
                token->data.op = op;
                bump_lexer(lexer);
            } else {
                char* op = calloc(2, sizeof(char)); // op + null term
                op[0] = ch;
                op[1] = '\0';
                token->type = TOKEN_OP;
                token->data.op = op;
            }
            return token;
        }
        default:
            printf("%c", ch);
            printf("ERR: unknown char");
            exit(EXIT_FAILURE);
    }
}


typedef struct {
    Lexer* lexer;
    Token* current;
} Parser;

Parser* new_parser(char* src) {
    Parser* parser = calloc(1, sizeof(Parser));
    Lexer* lexer = new_lexer(src);
    Token* token = next_token(lexer);
    parser->lexer = lexer;
    parser->current = token;
    return parser;
}

int delete_parser(Parser* parser) {
    if(!parser) {
        return -1;
    }
    free(parser->lexer);
    free(parser->current);
    free(parser);
    return 0;
}

void bump_parser(Parser* parser) {
    parser->current = next_token(parser->lexer);
}

Expr* parse_precedence(Parser* parser, int min_bp) {
    Token *lhs = parser->current;
    switch (lhs->type) {
        case TOKEN_INT: {
            int x = lhs->data.i;
            bump_parser(parser);
            Expr *expr = calloc(1, sizeof(Expr));
            expr->type = EXPR_INT;
            expr->data.i = x;
            return expr;
        }
        case TOKEN_BOOL: {
            int x = lhs->data.b;
            bump_parser(parser);
            Expr *expr = calloc(1, sizeof(Expr));
            expr->type = EXPR_BOOL;
            expr->data.b = x;
            return expr;
        }
        case TOKEN_IDENT: {
            char* ident = lhs->data.ident;
            bump_parser(parser);
            Expr *expr = calloc(1, sizeof(Expr));
            expr->type = EXPR_VAR;
            expr->data.var = ident;
            return expr;
        }
        case TOKEN_OP: {
            char* ident = lhs->data.ident;
            bump_parser(parser);
            Expr *expr = calloc(1, sizeof(Expr));
            expr->type = EXPR_VAR;
            expr->data.var = ident;
            return expr;
        }
    }
}

// int main() { 
//     Lexer* lexer = new_lexer("a(3*5)");
//     print_lexer(lexer);
//     return 0;
// }

