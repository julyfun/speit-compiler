#include "analex.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rust.h"

Vector* init_lexeme_list() {
    return vector_new(sizeof(Lexeme));
}

void add_lexeme(Vector* list, Lexeme lexeme) {
    vector_push_back(list, &lexeme);
}

void print_lexemes(Vector* list) {
    printf("[");
    for (size_t i = 0; i < list->size; i++) {
        Lexeme* lexeme = (Lexeme*)vector_get(list, i);
        switch (lexeme->type) {
            // print with type and value
            case LEX_PO:
                printf("PO");
                break;
            case LEX_PF:
                printf("PF");
                break;
            case LEX_PROP:
                printf("Prop(\"%s\")", lexeme->value);
                break;
            case LEX_OP:
                printf("Op(\"%s\")", lexeme->value);
                break;
            case LEX_PRODUIT:
                printf("PRODUIT");
                break;
        }
        if (i < list->size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void destroy_lexeme_list(Vector* list) {
    vector_destroy(list);
}

// <digit> ::= "0" | ... | "9"
// <letter> ::= "a" | ... | "z"
// <id> ::= <lettre> { <lettre> | <chiffre> }
typedef enum {
    TRY_ADD_OK,
    TRY_ADD_NOT_ID,
} TryAddResult;

TryAddResult try_add_buffer_as_prop(Vector* lexeme_list, char* buffer, int* buffer_index) {
    if (*buffer_index > 0) {
        // word in buffer not <id>? use regex
        regex_t regex;
        int reti = regcomp(&regex, "^[a-zA-Z][a-zA-Z0-9]*$", 0);
        reti = regexec(&regex, buffer, 0, NULL, 0);
        regfree(&regex);
        if (reti != 0) {
            return TRY_ADD_NOT_ID;
        }

        buffer[*buffer_index] = '\0';
        Lexeme lexeme = { LEX_PROP, "" }; // adding new one
        strcpy(lexeme.value, buffer);
        add_lexeme(lexeme_list, lexeme);
        *buffer_index = 0;
        return TRY_ADD_OK;
    }
    return TRY_ADD_OK;
}

typedef struct {
    const char* name; // Unicode 名称
    const char bytes[4]; // UTF-8 编码字节
    size_t length; // 字节长度
} Utf8Char;

// 定义需要匹配的字符
Utf8Char UTF8_CHARS[] = {
    { "IMPLIQUE", { (char)0xE2, (char)0x87, (char)0x92 }, 3 }, // ⇒
    { "PRODUIT", { (char)0xE2, (char)0x86, (char)0x92 }, 3 }, // →
    { "NON", { (char)0xC2, (char)0xAC }, 2 }, // ¬
    { "ET", { (char)0xE2, (char)0x88, (char)0xA7 }, 3 }, // ∧
    { "OU", { (char)0xE2, (char)0x88, (char)0xA8 }, 3 } // ∨
};

LexicalResult analyseur_lexical(char* input) {
    Vector* lexeme_list = init_lexeme_list();
    char buffer[10];
    int buffer_index = 0;
    for (int i = 0; input[i] != '\0';) {
        if (is_space(input[i])) {
            if (try_add_buffer_as_prop(lexeme_list, buffer, &buffer_index) == TRY_ADD_NOT_ID) {
                LexicalResult res = (LexicalResult) { LEX_ERR, .error = "" };
                sprintf(res.error, "Invalid identifier %s at position %d", buffer, i);
                return res;
            }
            i++;
            continue;
        }
        if (is_alpha(input[i]) || is_digit(input[i])) {
            buffer[buffer_index++] = input[i];
            i++;
            continue;
        }
        if (try_add_buffer_as_prop(lexeme_list, buffer, &buffer_index) == TRY_ADD_NOT_ID) {
            LexicalResult res = (LexicalResult) { LEX_ERR, .error = "" };
            sprintf(res.error, "Invalid identifier %s at position %d", buffer, i);
            return res;
        }

        // check if the current character is a UTF-8 character
        if (input[i] == (char)0xE2 || input[i] == (char)0xC2) {
            int flag = 0;
            for (int j = 0; j < sizeof(UTF8_CHARS) / sizeof(Utf8Char); j++) {
                if (memcmp(input + i, UTF8_CHARS[j].bytes, UTF8_CHARS[j].length) == 0) {
                    // special: PRODUIT
                    if (strcmp(UTF8_CHARS[j].name, "PRODUIT") == 0) {
                        Lexeme lexeme = { LEX_PRODUIT, "" };
                        add_lexeme(lexeme_list, lexeme);
                    } else {
                        Lexeme lexeme = { LEX_OP, "" };
                        strcpy(lexeme.value, UTF8_CHARS[j].name);
                        add_lexeme(lexeme_list, lexeme);
                    }
                    i += UTF8_CHARS[j].length;
                    flag = 1;
                    break;
                }
            }
            if (flag) {
                continue;
            }
            LexicalResult res = (LexicalResult) { LEX_ERR, .error = "" };
            sprintf(res.error, "Invalid UTF-8 character %d at position %d", (int)input[i], i);
            return res;
        }
        if (input[i] == '(') {
            Lexeme lexeme = { LEX_PO, "" };
            add_lexeme(lexeme_list, lexeme);
            i++;
            continue;
        }
        if (input[i] == ')') {
            Lexeme lexeme = { LEX_PF, "" };
            add_lexeme(lexeme_list, lexeme);
            i++;
            continue;
        }
        // return and show the what is the error character and position
        LexicalResult res = (LexicalResult) { LEX_ERR, .error = "" };
        sprintf(
            res.error,
            "Invalid character %c (ascii %d) at position %d",
            input[i],
            (int)input[i],
            i
        );
        return res;
    }
    if (try_add_buffer_as_prop(lexeme_list, buffer, &buffer_index) == TRY_ADD_NOT_ID) {
        LexicalResult res = (LexicalResult) { LEX_ERR, .error = "" };
        sprintf(res.error, "Invalid identifier %s at position %zu", buffer, strlen(input));
        return res;
    }
    return (LexicalResult) { LEX_OK, .value = lexeme_list };
}