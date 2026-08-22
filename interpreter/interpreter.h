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

// Instead of using an AST structure, we are using a recursive expression struct
// which means that it contains other expressions (look at unary and
// binary unions), also we will need to free the expression *recursively*


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

Lexer* new_lexer(char* src);
Token* next_token(Lexer* lexer);
void free_lexer(Lexer* l);

// Parser
typedef struct {
    Lexer* lexer;
    Token* current;
} Parser;

Parser* new_parser(char* src);
int delete_parser(Parser* parser);
#endif //INTERPRETER_H_