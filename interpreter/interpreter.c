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

Parser* new_parser(char* src) {
    Parser* parser = calloc(1, sizeof(Parser));
    parser->lexer = new_lexer(src);
    parser->current = next_token(parser->lexer);
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

Expr* parse_precedence(Parser* parser, int min_bp) {
    Expr* lhs = calloc(1, sizeof(Expr));
    Token* curr = parser->current;
    switch (curr->type) {
        case TOKEN_INT: {
            int x = curr->data.i;
            bump_parser(parser);
            lhs->type = EXPR_INT;
            lhs->data.i = x;
            break;
        }
        case TOKEN_BOOL: {
            int x = curr->data.b;
            bump_parser(parser);
            lhs->type = EXPR_BOOL;
            lhs->data.b = x;
            break;
        }
        case TOKEN_IDENT: {
            bump_parser(parser);
            lhs->type = EXPR_VAR;
            lhs->data.var = curr->data.ident;
            break;
        }
        case TOKEN_OP: {
            if(strcmp(curr->data.op, "-") == 0) {
                bump_parser(parser);
                Expr* rhs = parse_precedence(parser, 4);
                lhs->type = EXPR_UNARY;
                printf("UNARYT NEG ASSIGNEMNT\n");
                lhs->data.unary = calloc(1, sizeof(unary));
                lhs->data.unary->unaryOp = UNARY_NEG;
                lhs->data.unary->expr = rhs;
            } else if (strcmp(curr->data.op, "!") == 0) {
                bump_parser(parser);
                Expr* rhs = parse_precedence(parser, 4);
                lhs->type = EXPR_UNARY;
                lhs->data.unary = calloc(1, sizeof(unary));
                lhs->data.unary->unaryOp = UNARY_NOT;
                lhs->data.unary->expr = rhs;
            }

            break;
        }
        case TOKEN_LPAREN: {
            bump_parser(parser);

            lhs = parse_expr(parser);
            if(parser->current->type != TOKEN_RPAREN){
                printf("Unclosed Parentheses");
                exit(EXIT_FAILURE);
            }
            bump_parser(parser); // WE bump parser since we know we are at a RPAREN token
            break;
        }
        default: {
            printf("Token type wasnt caught in the switch case");
            exit(EXIT_FAILURE);
        }
    }



    while(1) {
        Token* op_token = NULL;
        if(parser->current->type == TOKEN_OP) {
            op_token = parser->current;
        }
        if(op_token == NULL) break;
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
        Expr* rhs = parse_precedence(parser, bp+1);
        Expr* new = calloc(1, sizeof(Expr));
        new->type = EXPR_BINARY;
        new->data.binary = calloc(1, sizeof(binary));
        new->data.binary->lexpr = lhs;
        new->data.binary->rexpr = rhs;
        new->data.binary->binaryOp = op;
        lhs = new;
    }

    return lhs;
}

Expr* parse_expr(Parser* parser) {
    return parse_precedence(parser, 0);
}

void free_statement(Stmt* statement) {
    switch(statement->type){
        case STMT_ASSIGN:
            free(statement->data.assign);
            break;
        case STMT_JUMP:
            free(statement->data.jump);
            break;
        case STMT_IFJUMP:
            free(statement->data.ifjump);
            break;
        case STMT_PRINT:
            free(statement->data.print);
            break;
        case STMT_SLEEP:
            free(statement->data.sleep);
            break;
    }
    free(statement);
}

void append_statement(Statements* statements, Stmt* statement) {
    if(statements->count >= statements->capacity) {
        if(statements->capacity == 0) statements->capacity = 256;
        else statements->capacity *= 2;
        statements->items = realloc(statements->items, statements->capacity * sizeof(*statements->items));
    }
    statements->items[statements->count++] = *statement;
    free_statement(statement);
}


// Label and Statements hashmap logic

unsigned long hash_function(const char* str) {
    unsigned long hash = 0;
    int c;

    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}


HashMap* new_hashmap() {
    HashMap* hashmap = calloc(1, sizeof(HashMap));
    hashmap->items = calloc(HASH_SIZE, sizeof(Pair));
    hashmap->capacity = HASH_SIZE;
    hashmap->count = 0;
    return hashmap;
}

void append_hashmap(HashMap* map, Pair* pair) {
    unsigned long hash = hash_function(pair->key);
    hash = hash % map->capacity;
    int seen_end = 0;
    while(map->items[hash].key){
        hash++;
        if(seen_end == 1){
            printf("No space left broski\n");
            exit(EXIT_FAILURE);
        }
        if(hash >= HASH_SIZE - 1){
            hash = hash % HASH_SIZE;
            seen_end = 1;
        }
    }
    map->items[hash] = *pair;
    free(pair);
}

Pair* search_hashmap(HashMap* map, char* key) {
    unsigned long hash = hash_function(key);
    hash = hash % map->capacity; 
    if(!map->items[hash].key){
        printf("No such item\n");
        return NULL;
    }
    int seen_end = 0;
    while(strcmp(key, map->items[hash].key) != 0)
    {
        hash++;
        if(seen_end == 1){
            printf("No such item\n");
            return NULL;
        }
        if(hash >= HASH_SIZE - 1){
            hash = hash % HASH_SIZE;
            seen_end = 1;
        }
    }
    return &map->items[hash];
}





HashMap_v* new_hashmap_v() {
    HashMap_v* hashmap = calloc(1, sizeof(HashMap_v));
    hashmap->items = calloc(HASH_SIZE, sizeof(Pair_v));
    hashmap->capacity = HASH_SIZE;
    hashmap->count = 0;
    return hashmap;
}

void append_hashmap_v(HashMap_v* map, Pair_v* pair) {
    unsigned long hash = hash_function(pair->key);
    hash = hash % map->capacity;
    int seen_end = 0;
    while(map->items[hash].key){
        hash++;
        if(seen_end == 1){
            printf("No space left\n");
            exit(EXIT_FAILURE);
        }
        if(hash >= HASH_SIZE - 1){
            hash = hash % HASH_SIZE;
            seen_end = 1;
        }
    }
    map->items[hash] = *pair;
    free(pair);
}

Pair_v* search_hashmap_v(HashMap_v* map, char* key) {
    unsigned long hash = hash_function(key);
    hash = hash % map->capacity; 
    if(!map->items[hash].key){
        printf("No such item\n");
        return NULL;
    }
    int seen_end = 0;
    while(strcmp(key, map->items[hash].key) != 0)
    {
        hash++;
        if(seen_end == 1){
            printf("No such item\n");
            return NULL;
        }
        if(hash >= HASH_SIZE - 1){
            hash = hash % HASH_SIZE;
            seen_end = 1;
        }
    }
    return &map->items[hash];
}

// Interpreter 


typedef struct {
    char* text; 
    int current; 
    int size;
} Source;

Source* new_source(char* src) {
    Source* source = malloc(sizeof(Source));
    source->text = src;
    source->current = 0;
    source->size = strlen(src);
    return source;
}

char* next_line(Source* src) {
    if(src->text[src->current] == '\0'){
        return NULL;
    }
    if(src->current >= src->size) {
        return NULL;
    }
    int start = src->current;
    while(src->text[src->current] != '\n') {
        src->current++;
        if(src->current >= src->size) {
            break;
        }
    }
    char* line = calloc(src->current - start, sizeof(char));
    memcpy(line, src->text + start, src->current - start);
    src->current++;
    return line;
}



void from_source(Source* src) {
    Statements* stmts = malloc(sizeof(Statements)); 
    HashMap* labels = new_hashmap();
    char* line = next_line(src);
    while(line != NULL){
        printf("%s\n", line);
        free(line);
        line = next_line(src);
    }
}

int main() {
    Source* s = new_source("test\ntest\nasdasdasdasd\n");
    from_source(s);
    return 0;
}



//
//
// int main() {
//     Parser* parser = new_parser("(1+(1+3)");
//     Expr* expr = parse_expr(parser);
//     printf("Done\n");
//     if(expr->type==EXPR_BINARY) {
//         printf("Expression is correctly identified as bin \n");
//         if(expr->data.binary->lexpr->data.i){
//             printf("LHS: %d\n", expr->data.binary->lexpr->data.i);
//         } 
//         if(expr->data.unary->unaryOp == UNARY_NEG) {
//             printf("its unary!!\n");
//         }
//         if(expr->data.binary->rexpr->data.binary->rexpr->data.i) {
//             printf("LHS: %d\n", expr->data.binary->rexpr->data.binary->lexpr->data.i);
//             printf("RHS: %d\n", expr->data.binary->rexpr->data.binary->rexpr->data.i);
//         }
//     }
//     else if(expr->type == EXPR_BOOL) printf("its bool");
//     else if(expr->type == EXPR_INT) printf("its int");
//     else if(expr->type == EXPR_BINARY) printf("its binary");
//     else if(expr->type == EXPR_VAR){
//         printf("its var\n");
//         printf("%s\n",expr->data.var);
//     }
//     else printf("wtf");
//     return 0;
// }
//
