#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"


int unwrap_int(const Value* v) {
    if(v->type != VAL_INT){
        printf("ERROR: unwrap_int on a non int value\n");
        exit(EXIT_FAILURE);
    }
    return v->data.i;
}

int unwrap_bool(const Value* v) {
    if(v->type != VAL_BOOL){
        printf("ERROR: unwrap_bool on a non bool value\n");
        exit(EXIT_FAILURE);
    }


    if(v->data.b != 0 && v->data.b != 1){
        printf("ERROR: bool has a non 0/1 value\n");
        exit(EXIT_FAILURE);
    }

    return v->data.b;
}

// Since we are simply assigning memory to the lexer struct,
// a simple free is sufficient to completely destroy the lexer
Lexer* new_lexer(char* src) {
    int len = strlen(src);
    Lexer* lexer = malloc(sizeof(Lexer) + len + 1);
    lexer->pos = 0;
    lexer->len = len;
    memcpy(lexer->chars, src, len + 1);
    return lexer;
}

void free_lexer(Lexer* l) {
    if(l == NULL){
        printf("ERROR: Tried freeing lexer but failed..\n");
        exit(EXIT_FAILURE);
    }
    free(l);
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


    if(ch == EOF || ch == '\0') { // null strings are added for the testing suite
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

Parser* new_parser(Lexer* lexer) {
    Parser* parser = calloc(1, sizeof(Parser));
    Token* token = next_token(lexer);
    parser->lexer = lexer;
    parser->current = token;
    return parser;
}

void free_parser(Parser* parser) {
    if(parser == NULL){
        printf("ERROR: Tried freeing parser but failed..\n");
        exit(EXIT_FAILURE);
    }
    free(parser->lexer);
    free(parser->current);
    free(parser);
}

void bump_parser(Parser* parser) {
    parser->current = next_token(parser->lexer);
}

int evaluate_precedence(Token* token) {
    if(token == NULL){
        return 1;
    }
    if(token->type != TOKEN_OP){
        return 1;
    }
    if(
        strcmp(token->data.op, "==") == 0 ||
        strcmp(token->data.op, "!=") == 0 ||
        strcmp(token->data.op, ">=") == 0 ||
        strcmp(token->data.op, ">") == 0 ||
        strcmp(token->data.op, "<=") == 0 ||
        strcmp(token->data.op, "<") == 0
    ) {
        return 1;
    } else if (
        strcmp(token->data.op, "+") == 0 ||
        strcmp(token->data.op, "-") == 0
    ) {
        return 2;
    } else if (
        strcmp(token->data.op, "*") == 0 ||
        strcmp(token->data.op, "/") == 0
    ) {
        return 3;
    } else {
        printf("This code is never supposed to execute!!!");
        exit(EXIT_FAILURE);
    }
}

Expr* parse_expr(Parser* parser);
Expr* parse_precedence(Parser* parser, int min_bp) {
    Expr* lhs;
    Token *curr = parser->current;
    switch (curr->type) {
        case TOKEN_INT: {
            int x = curr->data.i;
            bump_parser(parser);
            Expr *expr = calloc(1, sizeof(Expr));
            expr->type = EXPR_INT;
            expr->data.i = x;
            lhs = expr;
            break;
        }
        case TOKEN_BOOL: {
            int x = curr->data.b;
            bump_parser(parser);
            Expr *expr = calloc(1, sizeof(Expr));
            expr->type = EXPR_BOOL;
            expr->data.b = x;
            lhs = expr;
            break;
        }
        case TOKEN_IDENT: {
            char* ident = curr->data.ident;
            bump_parser(parser);
            Expr *expr = calloc(1, sizeof(Expr));
            expr->type = EXPR_VAR;
            expr->data.var = ident;
            lhs = expr;
            break;
        }
        case TOKEN_OP: {
            Expr *expr = calloc(1, sizeof(Expr));
            if(strcmp(curr->data.op, "-")) {
                bump_parser(parser);
                Expr* rhs = parse_precedence(parser, 4);
                Expr *expr = calloc(1, sizeof(Expr));
                expr->type = EXPR_UNARY;
                expr->data.unary.unaryOp = UNARY_NEG;
                expr->data.unary.expr = rhs;
            } else if (strcmp(curr->data.op, "!")) {
                bump_parser(parser);
                Expr* rhs = parse_precedence(parser, 4);
                Expr *expr = calloc(1, sizeof(Expr));
                expr->type = EXPR_UNARY;
                expr->data.unary.unaryOp = UNARY_NOT;
                expr->data.unary.expr = rhs;
            }
            lhs = expr;
            break;
        }
        case TOKEN_LPAREN: {
            bump_parser(parser);

            Expr* expr = parse_expr(parser);
            if(parser->current->type != TOKEN_RPAREN){
                printf("Unclosed Parentheses");
                exit(EXIT_FAILURE);
            }
            bump_parser(parser); // WE bump parser since we know we are at a RPAREN token
            lhs = expr;
            break;
        }
        default: {
            printf("Token type wasnt caught in the switch case");
            exit(EXIT_FAILURE);
        }
    }


    while(1) {
        Token* op_token = NULL;
        if(curr->type == TOKEN_OP) {
            op_token = curr;
        }
        int bp = evaluate_precedence(op_token);
        if(bp < min_bp) break;

        BinaryOp op;
        if(strcmp(op_token->data.op, "+") == 0) op = BINARY_ADD;
        else if(strcmp(op_token->data.op, "-") == 0) op = BINARY_SUB;
        else if(strcmp(op_token->data.op, "*") == 0) op = BINARY_MUL;
        else if(strcmp(op_token->data.op, "/") == 0) op = BINARY_DIV;
        else if(strcmp(op_token->data.op, "<") == 0) op = BINARY_LT;
        else if(strcmp(op_token->data.op, "<=") == 0) op = BINARY_LE;
        else if(strcmp(op_token->data.op, ">") == 0) op = BINARY_GT;
        else if(strcmp(op_token->data.op, ">=") == 0) op = BINARY_GE;
        else if(strcmp(op_token->data.op, "==") == 0) op = BINARY_EQ;
        else if(strcmp(op_token->data.op, "!=") == 0) op = BINARY_NE;
        else break;

        bump_parser(parser);
        Expr* rhs = parse_precedence(parser, bp + 1);
        lhs->type = EXPR_BINARY;
        lhs->data.binary.lexpr = lhs;
        lhs->data.binary.rexpr = rhs;
        lhs->data.binary.binaryOp = op;
    }
    return lhs;
}

Expr* parse_expr(Parser* parser) {
    return parse_precedence(parser, 0);
};

