#ifndef ANALEX_H
#define ANALEX_H

#include "vector.h"

enum {
    MSG_LENGTH = 128,
};

typedef enum { PO, PF, PROP, OP, PRODUIT } LexemeType;

typedef struct {
    LexemeType type;
    char value[10];
} Lexeme;

Vector* init_lexeme_list();
void add_lexeme(Vector* list, Lexeme lexeme);
void print_lexemes(Vector* list);
void destroy_lexeme_list(Vector* list);

typedef struct {
    enum { OK, ERR } type;
    union {
        Vector* value;
        char error[MSG_LENGTH];
    };
} LexicalResult;

LexicalResult analyseur_lexical(char* input);

#endif // ANALEX_H
