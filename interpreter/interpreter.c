#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
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
        printf("This code isnt supposed to execute");
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
}

Pair* search_hashmap(HashMap* map, char* key) {
    unsigned long hash = hash_function(key);
    hash = hash % map->capacity; 
    if(!map->items[hash].key){
        return NULL;
    }
    int seen_end = 0;

    while(strcmp(key, map->items[hash].key) != 0)
    {
        hash++;
        if(seen_end == 1){
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
        if(strcmp(map->items[hash].key, pair->key) == 0){
            map->items[hash].value = pair->value;
            free(pair);
            return;
        }
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
        return NULL;
    }
    int seen_end = 0;
    while(strcmp(key, map->items[hash].key) != 0)
    {
        hash++;
        if(seen_end == 1){
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
    int len = src->current - start;
    char* line = calloc(len + 1, sizeof(char));
    memcpy(line, src->text + start, src->current - start);
    line[len] = '\0';
    src->current++;
    return line;
}


char *trim(char *str)
{
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;
    
    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }
    
    len = strlen(str);
    endp = str + len;
    
    /* Move the front and back pointers to address the first non-whitespace
     * characters from each end.
     */
    while(  *frontp == ' ') { ++frontp; }
    if( endp != frontp )
    {
        while(  *(--endp) == ' ' && endp != frontp ) {}
    }
    
    if(frontp != str && endp == frontp )
    {
        // Empty string
        *((*endp == ' ') ? str : (endp + 1)) = '\0';
    }
    else if( str + len - 1 != endp )
            *(endp + 1) = '\0';
    
    /* Shift the string so that it starts at str so that if it's dynamically
     * allocated, we can still free it on the returned pointer.  Note the reuse
     * of endp to mean the front of the string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }
    
    return str;
}



char* strip_after_char(char* line, char symbol){
    int i = 0;
    int len = strlen(line);
    char* new_line;
    while(i <= len){
        if(line[i] == symbol) {
            new_line = calloc(i + 1 ,sizeof(char));
            memcpy(new_line, line, i);
            new_line[i] = '\0';
            free(line);
            return new_line;
        }
        i++;
    }
    return line;
}


int starts_with(const char *str, const char *pre)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}


Interpreter* new_interpreter_from_source(Source* src) {
    Statements* stmts = malloc(sizeof(Statements)); 
    stmts->items = NULL;
    stmts->count = 0;
    stmts->capacity = 0;
    HashMap* labels = new_hashmap();
    char* line = next_line(src);
    // TODO: This while loop mallocs exprs without ever freeing them, fix that
    while(line != NULL){

        // Strip comments (start with ;)
        if(strlen(line) == 0) {
            free(line);
            line = next_line(src);
            continue;
        }
        if(line[0] == ';') {
            free(line);
            line = next_line(src);
            continue;
        }

        line = strip_after_char(line, ';');

        // Trim whitespace 
        line = trim(line);

        // Skip lines that become empty after trimming
        if(strlen(line) == 0) {
            free(line);
            line = next_line(src);
            continue;
        }

        printf("%s\n", line);


        // Labels

        if(line[0] == '.') {

            int n = strlen(line);
            Pair* pair = malloc(sizeof(Pair));
            pair->key = calloc(n - 1 + 1, sizeof(char)); 
            strncpy(pair->key, line + 1, n - 1);
            pair->key[n - 1] = '\0';
            pair->value = stmts->count;

            printf("Label: %s\n", pair->key);

            append_hashmap(labels, pair);

            Pair* new_pair = search_hashmap(labels, pair->key);
            printf("Key: %s, Value: %d\n", new_pair->key, new_pair->value);

            free(line);
            line = next_line(src);
            continue;
        }

        if (starts_with(line, "sleep ")) {

            Parser* parser = new_parser(line + 6);
            Expr* expr = parse_expr(parser);

            Stmt* stmt = malloc(sizeof(Stmt));
            stmt->type = STMT_SLEEP;
            stmt->data.sleep = malloc(sizeof(Sleep));
            stmt->data.sleep->expr = expr;
            append_statement(stmts, stmt);

            free_parser(parser);
            free(line);
            line = next_line(src);
            continue;
        }

        if(starts_with(line, "print ")) {
            line = line + 6; // offset for length for print

            Stmt* stmt = malloc(sizeof(Stmt));
            stmt->type = STMT_PRINT;
            stmt->data.print = malloc(sizeof(Print));
            stmt->data.print->name = calloc(strlen(line) + 1, sizeof(char));

            memcpy(stmt->data.print->name, line, strlen(line));
            append_statement(stmts, stmt);

            line = line - 6;
            free(line);
            line = next_line(src);
            continue;
        }


        // if (cond) jump (label)

        if(starts_with(line, "if ")) {
            int i = 3;
            int cond_size = 0;
            int n = strlen(line);
            while(!(line[i] == 'j' && line[i+1] == 'u' && line[i+2] == 'm' && line[i+3] == 'p')){
                if(line[i+4] == '\0' || i + 4 > n) {
                    printf("FATAL: if statement doesnt have jump");
                    exit(EXIT_FAILURE);
                }
                cond_size++;
                i++;
            }
            char cond[cond_size];
            strncpy(cond, line + 3, cond_size);
            cond[cond_size] = '\0';

            i = i + 4; // (offset for if + condition size) + offset for jump
            while(line[i] != '.'){
                if(i >= n) {
                    printf("FATAL: ifjump statement doesnt have proper label");
                    exit(EXIT_FAILURE);
                }
                i++;
            }
            i++; // skip the .
            char* label = calloc(n - i, sizeof(char));
            strncpy(label, line + i, n - i);
            label[n-i] = '\0';

            printf("cond: %s, label: %s\n", cond, label);

            Parser* parser = new_parser(cond);
            Expr* expr = parse_expr(parser);
            free_parser(parser);

            Stmt* stmt = malloc(sizeof(Stmt));
            stmt->type = STMT_IFJUMP;
            stmt->data.ifjump = malloc(sizeof(Ifjump));
            stmt->data.ifjump->label = trim(label);
            stmt->data.ifjump->cond = expr;

            append_statement(stmts, stmt);
            free(line);
            line = next_line(src);
            continue;
        }

        if(starts_with(line, "jump ")) {
            int n = strlen(line);
            int i = 5;
            while(line[i] != '.') {
                if(i >= n) {
                    printf("FATAL: jump statement has no label\n");
                    exit(EXIT_FAILURE);
                }
                i++;
            }

            i++; //skip the .

            if(!line[i]) {
                printf("FATAL: jump statement has no label\n");
                exit(EXIT_FAILURE);
            }

            char* label = calloc(n - i, sizeof(char));
            strncpy(label, line + i, n - i);
            label[n - i] = '\0';

            Stmt* stmt = malloc(sizeof(Stmt));
            stmt->type = STMT_JUMP;
            stmt->data.jump = malloc(sizeof(Jump));
            stmt->data.jump->label = trim(label);

            printf("label: %s\n", stmt->data.jump->label);

            append_statement(stmts, stmt);
            free(line);
            line = next_line(src);
            continue;
        }

        int n = strlen(line);
        int i = 0;
        while(line[i] != '=') {
            if(i >= n) {
                printf("FATAL: Unknown statement type (not one of assign, jump, ifjump, print, or sleep)");
                exit(EXIT_FAILURE);
            }
            i++;
        }


        // abc = whatever
        // 01234........n
        //     ^

        char* name = calloc(i + 1, sizeof(char));
        strncpy(name, line, i);
        name[i] = '\0';


        i++; // pass the = 

        char* rhs = calloc(n - i + 1, sizeof(char));
        strncpy(rhs, line + i, n - i);
        rhs[n - i] = '\0';

        printf("name: %s rhs: %s\n", name, rhs);

        Parser* parser = new_parser(rhs);
        Expr* expr = parse_expr(parser);
        free_parser(parser);
        free(rhs);

        Stmt* stmt = malloc(sizeof(Stmt));
        stmt->type = STMT_ASSIGN;
        stmt->data.assign = malloc(sizeof(Assign));
        stmt->data.assign->name = trim(name);
        stmt->data.assign->expr = expr;


        append_statement(stmts, stmt);
        free(line);
        line = next_line(src);
        continue;
    }
    // End of while loop

    free(line);

    Interpreter* interpreter = new_interpreter(stmts, labels);
    return interpreter;
}

Interpreter* new_interpreter(Statements *stmts, HashMap *labels) {
    Interpreter* interpreter = malloc(sizeof(Interpreter));
    interpreter->stmts = stmts;
    interpreter->labels = labels;
    interpreter->vars = new_hashmap_v();
    return interpreter;
}

int lookup_label(Interpreter* interpreter, char* label) {
    Pair* pair = search_hashmap(interpreter->labels, label);
    if(pair == NULL) {
        printf("Unknown Label, exiting program\n");
        exit(EXIT_FAILURE);
    }
    return pair->value;
}


Value eval_expr(Interpreter* interpreter, Expr* expr){
    switch(expr->type){
        case EXPR_BOOL:
        {
            Value val;
            val.type = VAL_BOOL;
            val.data.b = expr->data.b;
            return val;
        }
        case EXPR_VAR:
        {
            Pair_v* pair= search_hashmap_v(interpreter->vars, expr->data.var);
            if(pair == NULL){
                printf("Unknown variable name\n");
                exit(EXIT_FAILURE);
            }

            return pair->value;
        }
        case EXPR_INT:
        {
            Value val;
            val.type = VAL_INT;
            val.data.i = expr->data.i;
            return val;
        }
        case EXPR_UNARY:
        {
            Value rhs = eval_expr(interpreter, expr->data.unary->expr);
            // potential source of error: eval_expr 
            // returns a bool type when expecting int
            if(expr->data.unary->unaryOp == UNARY_NEG){
                rhs.data.i *= -1;
                return rhs;
            }
            else {
                rhs.data.b ^= 1; // Flips the bit from 0 to 1 and vice versa
                return rhs;
            }
            printf("This is never supposed to run");
        }
        case EXPR_BINARY:
        {
            Value lhs = eval_expr(interpreter, expr->data.binary->lexpr);
            Value rhs = eval_expr(interpreter, expr->data.binary->rexpr);
            BinaryOp op = expr->data.binary->binaryOp;
            switch(op){
                case BINARY_ADD:
                case BINARY_MUL:
                case BINARY_SUB:
                case BINARY_DIV:
                {
                    if(lhs.type != VAL_INT || rhs.type != VAL_INT){
                        printf("Arithmetic requires ints\n");
                        exit(EXIT_FAILURE);
                    }

                    if(op == BINARY_ADD){
                        lhs.data.i += rhs.data.i;
                        return lhs;
                    }
                    else if (op == BINARY_SUB) {
                        lhs.data.i -= rhs.data.i;
                        return lhs;
                    }
                    else if (op == BINARY_MUL) {
                        lhs.data.i *= rhs.data.i;
                        return lhs;
                    }
                    else if (op == BINARY_DIV) {
                        lhs.data.i /= rhs.data.i;
                        return lhs;
                    }
                    else {
                        printf("Unreachable code\n");
                        exit(EXIT_FAILURE);
                    }
                }
                case BINARY_LE:
                case BINARY_LT:
                case BINARY_GE:
                case BINARY_GT:
                {
                    if(lhs.type != VAL_INT || rhs.type != VAL_INT){
                        printf("Comparision requires ints\n");
                        exit(EXIT_FAILURE);
                    }
                    Value val;
                    val.type = VAL_BOOL;
                    if(op == BINARY_LE){
                        val.data.b = (lhs.data.i <= rhs.data.i);
                        return val;
                    }
                    if(op == BINARY_LT){
                        val.data.b = (lhs.data.i < rhs.data.i);
                        return val;
                    }
                    if(op == BINARY_GE){
                        val.data.b = (lhs.data.i >= rhs.data.i);
                        return val;
                    }
                    if(op == BINARY_GT){
                        val.data.b = (lhs.data.i > rhs.data.i);
                        return val;
                    }
                }
                case BINARY_EQ:
                case BINARY_NE:
                {
                    if(rhs.type == lhs.type){
                        Value val;
                        val.type = VAL_BOOL;
                        if(op == BINARY_EQ){
                            if(rhs.type == VAL_INT){
                                val.data.b = (rhs.data.i == lhs.data.i); 
                            }
                            else {
                                val.data.b = (rhs.data.b == lhs.data.b);
                            }
                        }
                        else if (op == BINARY_NE){
                            if(rhs.type == VAL_INT){
                                val.data.b = (rhs.data.i != lhs.data.i); 
                            }
                            else {
                                val.data.b = (rhs.data.b != lhs.data.b);
                            }
                        }
                        return val;
                    }
                    else {
                        printf("Comparision must be between same types\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            // End of switch for Binary
        }
    }
    // End of switch
}

void run_interpreter(Interpreter* inter) {
    printf("----Start of run interpreter----\n");
    int inter_pos = 0; // points to the current statement of interpreter

    while(inter_pos >= 0 && inter_pos < inter->stmts->count) {
        Stmt curr_stmt = inter->stmts->items[inter_pos];
        switch(curr_stmt.type) {
            case STMT_ASSIGN:
            {
                Value val = eval_expr(inter, curr_stmt.data.assign->expr);
                Pair_v* pair = malloc(sizeof(Pair_v)); // Freed in the append_hashmap_v fn

                pair->key = (curr_stmt.data.assign->name);
                pair->value = val;
                append_hashmap_v(inter->vars, pair);
                inter_pos += 1;
                break;
            }
            case STMT_JUMP:
            {
                Pair* new_pos = search_hashmap(inter->labels, curr_stmt.data.jump->label);
                if(new_pos == NULL){
                    printf("Label not found in jump statement \n");
                    exit(EXIT_FAILURE);
                }
                inter_pos = new_pos->value;
                break;
            }
            case STMT_IFJUMP:
            {
                Pair* new_pos = search_hashmap(inter->labels, curr_stmt.data.ifjump->label);
                if(new_pos == NULL){
                    printf("Label not found in ifjump statement\n");
                    exit(EXIT_FAILURE);
                }
                Value val = eval_expr(inter, curr_stmt.data.ifjump->cond);
                if(val.type == VAL_BOOL){
                    if(val.data.b) {
                        inter_pos = new_pos->value;
                    } else {
                        inter_pos += 1;
                    }
                } else {
                    printf("Ifjump condition must be a valid boolean");
                    exit(EXIT_FAILURE);
                }
                break;
            }
            case STMT_PRINT:
            {
                Pair_v* var = search_hashmap_v(inter->vars, curr_stmt.data.print->name);
                if(var == NULL){
                    printf("%s\n", curr_stmt.data.print->name);
                    inter_pos += 1;
                    break;
                }
                Value val = var->value;
                if(val.type == VAL_INT){
                    printf("%d\n", val.data.i);
                }
                if(val.type == VAL_BOOL){
                    if(val.data.b){
                        printf("true\n");
                    } else{
                        printf("false\n");
                    }
                }
                inter_pos += 1;
                break;
            }
            case STMT_SLEEP:
            {
                Value val = eval_expr(inter, curr_stmt.data.sleep->expr);
                if(val.type != VAL_INT){
                    printf("Sleep requires integer");
                    exit(EXIT_FAILURE);
                }
                int ms = val.data.i;
                sleep(ms);
                inter_pos += 1;
                break;
            }
        }
    }
    printf("-------script execution successful--------\n");
}



// File handling

Interpreter* new_interpreter_from_file(char* path){
    char* file_str = file_to_string(path);
    Source* s = new_source(file_str);

    Interpreter* interpreter = new_interpreter_from_source(s);

    free(s);
    free(file_str);

    return interpreter;
}

char* file_to_string(char* path) {
    FILE *file;
    file = fopen(path, "r");

    if(file == NULL) return NULL;

    fseek(file, 0, SEEK_END);

    int length = ftell(file);

    fseek(file, 0, SEEK_SET);

    char* str = malloc((length + 1) * sizeof(char));
    char c;
    int i = 0;
    while((c = fgetc(file)) != EOF){
        str[i] = c;
        i++;
    }

    str[i] = '\0';

    fclose(file);
    return str;
}


int main() {
    char* path = "../script.txt";
    Interpreter* i = new_interpreter_from_file(path);
    run_interpreter(i);
    return 0;
}

