#ifndef ANASEM_H
#define ANASEM_H

#include "anasynt.h"

typedef struct {
    enum { SEMANTIC_OK, SEMANTIC_ERR } type;
    union {
        SyntaxNode* value;
        char error[MSG_LENGTH];
    };
} SemanticResult;

SemanticResult analyseur_semantique(SyntaxNode* node);

#endif
