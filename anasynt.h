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

// Function prototypes
SyntaxResult analyse_syntaxique(Vector* lexeme_list);
SyntaxNode* parse_expr(Vector* lexeme_list, size_t* index);
SyntaxNode* parse_regle(Vector* lexeme_list, size_t* index);
SyntaxNode* parse_arrow_expr(Vector* lexeme_list, size_t* index);
SyntaxNode* parse_or_expr(Vector* lexeme_list, size_t* index);
SyntaxNode* parse_and_expr(Vector* lexeme_list, size_t* index);
SyntaxNode* parse_unary_expr(Vector* lexeme_list, size_t* index);
SyntaxNode* parse_primary_expr(Vector* lexeme_list, size_t* index);
void destroy_syntax_tree(SyntaxNode* node);
