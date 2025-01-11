#include "analex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            case PO:
                printf("PO");
                break;
            case PF:
                printf("PF");
                break;
            case PROP:
                printf("PROP(\"%s\")", lexeme->value);
                break;
            case OP:
                printf("OP(\"%s\")", lexeme->value);
                break;
            case PRODUIT:
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

void add_buffer_as_lexeme(Vector* lexeme_list, char* buffer, int* buffer_index) {
    if (*buffer_index > 0) {
        buffer[*buffer_index] = '\0';
        Lexeme lexeme = { PROP, "" }; // adding new one
        strcpy(lexeme.value, buffer);
        add_lexeme(lexeme_list, lexeme);
        *buffer_index = 0;
    }
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
        if (isspace(input[i])) {
            i++;
            continue;
        }
        if (isalpha(input[i]) || isdigit(input[i])) {
            buffer[buffer_index++] = input[i];
            i++;
            continue;
        }
        add_buffer_as_lexeme(lexeme_list, buffer, &buffer_index);

        // check if the current character is a UTF-8 character
        if (input[i] == (char)0xE2 || input[i] == (char)0xC2) {
            int flag = 0;
            for (int j = 0; j < sizeof(UTF8_CHARS) / sizeof(Utf8Char); j++) {
                if (memcmp(input + i, UTF8_CHARS[j].bytes, UTF8_CHARS[j].length) == 0) {
                    // special: PRODUIT
                    if (UTF8_CHARS[j].name == "PRODUIT") {
                        Lexeme lexeme = { PRODUIT, "" };
                        add_lexeme(lexeme_list, lexeme);
                    } else {
                        Lexeme lexeme = { OP, "" };
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
            char msg[MSG_LENGTH];
            sprintf(msg, "Invalid UTF-8 character %d at position %d", (int)input[i], i);
            LexicalResult res = (LexicalResult) { ERR, .error = "" };
            memcpy(res.error, msg, strlen(msg));
            return res;
        }
        if (input[i] == '(') {
            Lexeme lexeme = { PO, "" };
            add_lexeme(lexeme_list, lexeme);
            i++;
            continue;
        }
        if (input[i] == ')') {
            Lexeme lexeme = { PF, "" };
            add_lexeme(lexeme_list, lexeme);
            i++;
            continue;
        }
        // return and show the what is the error character and position
        char error[MSG_LENGTH];
        sprintf(
            error,
            "Invalid character %c (ascii %d) at position %d",
            input[i],
            (int)input[i],
            i
        );
        LexicalResult res = (LexicalResult) { ERR, .error = "" };
        memcpy(res.error, error, strlen(error));
        return res;
    }
    add_buffer_as_lexeme(lexeme_list, buffer, &buffer_index);
    return (LexicalResult) { OK, .value = lexeme_list };
}