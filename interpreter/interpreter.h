#ifndef INTERPRETER_H_
#define INTERPRETER_H_



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
typedef struct {UnaryOp unaryOp; Expr* expr;} unary;
typedef struct {Expr* lexpr; BinaryOp binaryOp; Expr* rexpr;} binary;

typedef struct e {
    ExprType type;
    union {
        int i;
        int b;
        char* var;
        unary* unary;
        binary* binary;
    } data;
} Expr;

// Instead of using an explicit AST structure, we are using a recursive expression 
// struct which means that it contains other expressions (look at unary and
// binary unions), also we will need to free the expression *recursively*


// Statements

typedef enum {
    STMT_ASSIGN,
    STMT_JUMP,
    STMT_IFJUMP,
    STMT_PRINT,
    STMT_SLEEP
} StmtType;


typedef struct {char* name; Expr* expr;} assign;
typedef struct {char* label;} jump;
typedef struct {char* label; Expr* cond;} ifjump;
typedef struct {char* name;} print;
typedef struct {Expr* expr;} sleep;



typedef struct {
    StmtType type;
    union {
        assign* assign;
        jump* jump;
        ifjump* ifjump;
        print* print;
        sleep* sleep;
    } data; // Must be malloc'd at the time of creation
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

Lexer* new_lexer(char* src);
Token* next_token(Lexer* lexer);
void free_lexer(Lexer* l);

// Parser
typedef struct {
    Lexer* lexer;
    Token* current;
} Parser;

Parser* new_parser(char* src);
Expr* parse_expr(Parser* parser);
void free_parser(Parser* parser);


typedef struct {
    Stmt* items;
    int count;
    int capacity;
} Statements;



void free_statement(Stmt* statement);
void append_statement(Statements* statements, Stmt* statement);


// Hashmap 


#define HASH_SIZE 4096


/* The HashMap struct is a generic key-value hashmap with
 * max capacity of HASH_SIZE, there is currently no way to
 * delete elements. Collisions are linearly probed. There
 * is another hashmap HashMap_v which has the exact same
 * functions prefixed suffixed with _v, which is for a 
 * char* - Value pair (from interpreter.h) */


unsigned long hash_function(const char* str);
typedef struct {
    char* key;
    int value;
} Pair;

typedef struct {
    Pair* items;
    int count; 
    int capacity;
} HashMap;

HashMap* new_hashmap();


void append_hashmap(HashMap* map, Pair* pair); 

Pair* search_hashmap(HashMap* map, char* key);

typedef struct {
    char* key;
    Value value;
} Pair_v;

typedef struct {
    Pair_v* items;
    int count; 
    int capacity;
} HashMap_v;

HashMap_v* new_hashmap_v();


void append_hashmap_v(HashMap_v* map, Pair_v* pair);


Pair_v* search_hashmap_v(HashMap_v* map, char* key);


typedef struct {
    Statements* stmts;
    HashMap* labels;
    HashMap_v* vars;
} Interpreter;


#endif //INTERPRETER_H_
