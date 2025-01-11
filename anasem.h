#ifndef ANASEM_H
#define ANASEM_H

typedef struct {
    enum { SYNTAX_OK, SYNTAX_ERR } type;
    union {
        SyntaxNode* value;
        char error[MSG_LENGTH];
    };
} SyntaxResult;

typedef struct {
    enum { SEMANTIC_OK, SEMANTIC_ERR } type;
    union {
        SyntaxNode* value;
        char error[MSG_LENGTH];
    };
} SemanticResult;

SemanticResult analyseur_semantique(Vector* lexeme_list);

#endif
