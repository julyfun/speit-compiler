#ifndef ANALEX_H
#define ANALEX_H

#include "constants.h"
#include "vector.h"

typedef enum { LEX_PO, LEX_PF, LEX_PROP, LEX_OP, LEX_PRODUIT } LexemeType;

typedef struct {
    LexemeType type;
    char value[10];
} Lexeme;

Vector* init_lexeme_list();
void add_lexeme(Vector* list, Lexeme lexeme);
void print_lexemes(Vector* list);
void destroy_lexeme_list(Vector* list);

typedef struct {
    enum { LEX_OK, LEX_ERR } type;
    union {
        Vector* value;
        char error[MSG_LENGTH];
    };
} LexicalResult;

LexicalResult analyseur_lexical(char* input);

#endif // ANALEX_H
