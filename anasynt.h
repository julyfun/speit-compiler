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
        SyntaxNode* tree;
        char error[MSG_LENGTH];
    };
} SyntaxResult;
