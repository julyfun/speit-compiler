#include "anasem.h"

void test_read_lexeme(char* filename) {
    FILE* file = fopen(filename, "r");
    // read a line
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("---\n");
        printf("Input: %s", line);
        LexicalResult res = analyseur_lexical(line);
        if (res.type == LEX_ERR) {
            printf("%s\n", res.error);
            continue;
        }
        ParseResult parse_res = analyseur_syntaxique(res.value);
        if (parse_res.type == PARSE_ERR) {
            printf("%s\n", parse_res.error);
            continue;
        }
        SemanticResult sem_res = analyseur_semantique(parse_res.value);
        if (sem_res.type == SEMANTIC_ERR) {
            printf("%s\n", sem_res.error);
            continue;
        }
        printf("%s\n", "Semantic analysis: OK");
    }
}

int main() {
    test_read_lexeme("assets/Init.txt");
    test_read_lexeme("assets/Regles.txt");
    test_read_lexeme("assets/syntax_error.txt");
    return 0;
}
