#include "analex.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_init_lexeme_list() {
    Vector* list = init_lexeme_list();
    assert(list != NULL);
    assert(list->size == 0);
    assert(list->capacity > 0);
    destroy_lexeme_list(list);
    printf("test_init_lexeme_list passed\n");
}

void test_add_lexeme() {
    Vector* list = init_lexeme_list();
    Lexeme lexeme = { LEX_OP, "example" };
    add_lexeme(list, lexeme);
    assert(list->size == 1);
    Lexeme* retrieved_lexeme = (Lexeme*)vector_get(list, 0);
    assert(retrieved_lexeme->type == LEX_OP);
    assert(strcmp(retrieved_lexeme->value, "example") == 0);
    destroy_lexeme_list(list);
    printf("test_add_lexeme passed\n");
}

void test_print_lexemes() {
    Vector* list = init_lexeme_list();
    Lexeme lexeme1 = { LEX_PO, "" };
    Lexeme lexeme2 = { LEX_PF, "" };
    add_lexeme(list, lexeme1);
    add_lexeme(list, lexeme2);
    printf("Expected output: PO PF\nActual output: ");
    print_lexemes(list);
    destroy_lexeme_list(list);
    printf("test_print_lexemes passed\n");
}

void test_destroy_lexeme_list() {
    Vector* list = init_lexeme_list();
    destroy_lexeme_list(list);
    printf("test_destroy_lexeme_list passed\n");
}

void test_read_lexeme(char* filename) {
    FILE* file = fopen(filename, "r");
    // read a line
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("---\n");
        printf("%s", line);
        LexicalResult res = analyseur_lexical(line);
        if (res.type == LEX_ERR) {
            printf("Error: %s\n", res.error);
        } else {
            print_lexemes(res.value);
        }
    }
}

void test_illegal() {
    char* input = "a b c * d";
    LexicalResult res = analyseur_lexical(input);
    assert(res.type == LEX_ERR);
    printf(res.error);
    puts("");
}

int main() {
    test_init_lexeme_list();
    test_add_lexeme();
    test_print_lexemes();
    test_destroy_lexeme_list();
    test_read_lexeme("assets/Init.txt");
    test_read_lexeme("assets/Regles.txt");
    test_illegal();
    printf("All tests passed\n");
    return 0;
}