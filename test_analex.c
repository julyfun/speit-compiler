#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { PO, PF, PROP, OP } LexemeType;

typedef struct {
    LexemeType type;
    char value[10];
} Lexeme;

typedef struct {
    Lexeme* items;
    int size;
    int capacity;
} LexemeList;

LexemeList init_lexeme_list() {
    LexemeList list;
    list.size = 0;
    list.capacity = 10;
    list.items = (Lexeme*)malloc(list.capacity * sizeof(Lexeme));
    return list;
}

void add_lexeme(LexemeList* list, Lexeme lexeme) {
    if (list->size == list->capacity) {
        list->capacity *= 2; // just like vector
        list->items = (Lexeme*)realloc(list->items, list->capacity * sizeof(Lexeme));
    }
    list->items[list->size++] = lexeme;
}
