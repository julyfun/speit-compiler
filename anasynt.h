#include <stdio.h>
#include <stdlib.h>

#include "analex.h"
#include "constants.h"
#include "vector.h"

// Define the structure for the syntax tree
typedef struct SyntaxNode {
    Lexeme lexeme;
    struct SyntaxNode* left;
    struct SyntaxNode* right;
} SyntaxNode;

typedef struct {
    enum { SYNTAX_OK, SYNTAX_ERR } type;
    union {
        SyntaxNode* value;
        char error[MSG_LENGTH];
    };
} SyntaxResult;

typedef struct {
    enum { PARSE_OK, PARSE_ERR } type;
    union {
        SyntaxNode* value;
        char error[MSG_LENGTH];
    };
} ParseResult;

// Function prototypes
void print_syntax_tree(SyntaxNode* node, int indent, Vector* stack_is_last_son);
ParseResult parse_expr(Vector* lexeme_list, size_t* index);
ParseResult parse_rule(Vector* lexeme_list, size_t* index);
ParseResult parse_arrow_expr(Vector* lexeme_list, size_t* index);
ParseResult parse_or_expr(Vector* lexeme_list, size_t* index);
ParseResult parse_and_expr(Vector* lexeme_list, size_t* index);
ParseResult parse_unary_expr(Vector* lexeme_list, size_t* index);
ParseResult parse_primary_expr(Vector* lexeme_list, size_t* index);
void destroy_syntax_tree(SyntaxNode* node);
