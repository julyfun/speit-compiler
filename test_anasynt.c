#include "anasynt.h"

void test1() {
    Vector* lexeme_list = init_lexeme_list();
    Lexeme lexeme1 = { LEX_PROP, "a" };
    Lexeme lexeme2 = { LEX_OP, "ET" };
    Lexeme lexeme3 = { LEX_PROP, "b" };
    add_lexeme(lexeme_list, lexeme1);
    add_lexeme(lexeme_list, lexeme2);
    add_lexeme(lexeme_list, lexeme3);
    size_t index = 0;
    ParseResult res = parse_expr(lexeme_list, &index);
    // assert(res.type == PARSE_OK);
    Vector* stack_is_last_son = vector_new(sizeof(int));
    print_syntax_tree(res.value, 0, stack_is_last_son);
}

void test2() {
    Vector* lexeme_list = init_lexeme_list();
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "a" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "OU" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "b" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "ET" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "c" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "IMPLIQUE" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "d" });
    size_t index = 0;
    ParseResult res = parse_expr(lexeme_list, &index);
    // assert(res.type == PARSE_OK);
    Vector* stack_is_last_son = vector_new(sizeof(int));
    print_syntax_tree(res.value, 0, stack_is_last_son);
}

void test3() {
    Vector* lexeme_list = init_lexeme_list();
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "a" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "OU" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "b" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "ET" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "c" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "IMPLIQUE" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "d" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PRODUIT, "" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "a" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "OU" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "b" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "ET" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "c" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "IMPLIQUE" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "d" });
    size_t index = 0;
    ParseResult res = parse_expr(lexeme_list, &index);
    // assert(res.type == PARSE_OK);
    Vector* stack_is_last_son = vector_new(sizeof(int));
    print_syntax_tree(res.value, 0, stack_is_last_son);
}

void test4() {
    Vector* lexeme_list = init_lexeme_list();
    // [PO, PO, Prop("q12"), Op("ET"), Prop("r5"), PF,
    // Op("OU"), PO, PO, Op("NON"), Prop("a"), PF, Op("ET"),
    // Prop("c"), PF, PF, Op("IMPLIQUE"), PO, Prop("r5"), Op("IMPLIQUE"), Prop("c"), PF]
    add_lexeme(lexeme_list, (Lexeme) { LEX_PO });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PO });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "q12" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "ET" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "r5" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PF });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "OU" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PO });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PO });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "NON" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "a" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PF });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "ET" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "c" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PF });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PF });

    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "IMPLIQUE" });

    add_lexeme(lexeme_list, (Lexeme) { LEX_PO });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "r5" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_OP, "IMPLIQUE" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PROP, "c" });
    add_lexeme(lexeme_list, (Lexeme) { LEX_PF });
    print_lexemes(lexeme_list);

    size_t index = 0;
    ParseResult res = parse_expr(lexeme_list, &index);
    if (res.type == PARSE_ERR) {
        printf("Error: %s\n", res.error);
        return;
    }
    Vector* stack_is_last_son = vector_new(sizeof(int));
    print_syntax_tree(res.value, 0, stack_is_last_son);
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
            continue;
        }
        print_lexemes(res.value);
        size_t index = 0;
        ParseResult parse_res = parse_expr(res.value, &index);
        if (parse_res.type == PARSE_ERR) {
            printf("Error: %s\n", parse_res.error);
            continue;
        }
        Vector* stack_is_last_son = vector_new(sizeof(int));
        print_syntax_tree(parse_res.value, 0, stack_is_last_son);
    }
}

int main() {
    test1();
    test2();
    test3();
    test4();
    test_read_lexeme("assets/Init.txt");
    test_read_lexeme("assets/Regles.txt");
    return 0;
}
