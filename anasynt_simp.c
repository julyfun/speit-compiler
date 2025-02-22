#include "anasynt_simp.h"
#include "analex.h"

#include <stddef.h>
#include <string.h>

/*
```
<digit> ::= "0" | ... | "9"
<letter> ::= "a" | ... | "z"
<id> ::= <lettre> { <lettre> | <chiffre> }

<primary_expr> ::= <id> | "(" <expr> ")"
<expr> ::= <primary_expr> | ¬ <expr> | <primary_expr> ∧ <expr> | <primary_expr> ∨ <expr> | <primary_expr> → <expr> | <primary_expr> ⇒ <expr>
*/

void print_syntax_tree(SyntaxNode* node, int level, Vector* stack_is_last_son) {
    if (node == NULL) {
        return;
    }
    for (int i = 1; i <= level; i++) {
        if (i == level) {
            printf(*(int*)vector_get(stack_is_last_son, i - 1) == 1 ? "└── " : "├── ");
        } else {
            printf(*(int*)vector_get(stack_is_last_son, i - 1) == 1 ? "    " : "│   ");
        }
    }
    switch (node->lexeme.type) {
        case LEX_PO:
            printf("ERROR! (\n");
            break;
        case LEX_PF:
            printf("ERROR! )\n");
            break;
        case LEX_PROP:
            printf("Prop(%s)\n", node->lexeme.value);
            break;
        case LEX_OP:
            printf("Op(%s)\n", node->lexeme.value);
            break;
        case LEX_PRODUIT:
            printf("PRODUIT\n");
            break;
    }
    vector_push_back(stack_is_last_son, &(int) { node->right == NULL });
    print_syntax_tree(node->left, level + 1, stack_is_last_son);
    vector_pop_back(stack_is_last_son);

    vector_push_back(stack_is_last_son, &(int) { 1 });
    print_syntax_tree(node->right, level + 1, stack_is_last_son);
    vector_pop_back(stack_is_last_son);
}

ParseResult parse_expr(Vector* lexeme_list, size_t* index) {
    size_t start_index = *index;
    if (*index >= lexeme_list->size) {
        ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
        sprintf(res.error, "Syntax error: Unexpected end of input at position %zu", *index);
        return res;
    }
    Lexeme* lexeme = (Lexeme*)vector_get(lexeme_list, *index);
    if (lexeme->type == LEX_OP) {
        if (strcmp(lexeme->value, "NON") == 0){
            (*index)++;
            ParseResult res = parse_expr(lexeme_list, index);
            if (res.type == PARSE_ERR) {
                return res;
            }
            SyntaxNode* node = (SyntaxNode*)malloc(sizeof(SyntaxNode));
            node->lexeme = *lexeme;
            node->left = res.value;
            node->right = NULL;
            return (ParseResult) { PARSE_OK, .value = node };
        }
        else {
            ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
            sprintf(res.error, "Syntax error: Unexpected operator at position %zu", *index);
            return res;
        }
    }
    if (lexeme->type == LEX_PROP) {
        ParseResult left_res = parse_primary_expr(lexeme_list, index);
        if (left_res.type == PARSE_ERR) {
            *index = start_index;
            return left_res;
        }
        if (*index >= lexeme_list->size) {
            destroy_syntax_tree(left_res.value);
            *index = start_index;
            return (ParseResult) { PARSE_ERR, .error = "Syntax error: Unexpected end of input at position" };
        }
        Lexeme* lexeme = (Lexeme*)vector_get(lexeme_list, *index);
        if (strcmp(lexeme->value, "NON") == 0 | lexeme->type != LEX_OP | lexeme->type != LEX_PRODUIT) {
            destroy_syntax_tree(left_res.value);
            *index = start_index;
            return (ParseResult) { PARSE_ERR, .error = "Syntax error: Expected operator or produit" };
        }
        (*index)++;
        ParseResult right_res = parse_expr(lexeme_list, index);
        if (right_res.type == PARSE_ERR) {
            destroy_syntax_tree(left_res.value);
            *index = start_index;
            return right_res;
        }
        SyntaxNode* node = (SyntaxNode*)malloc(sizeof(SyntaxNode));
        node->lexeme = *lexeme;
        node->left = left_res.value;
        node->right = right_res.value;
        return (ParseResult) { PARSE_OK, .value = node };
    }
    // else suppose to be primary_expr
    return parse_primary_expr(lexeme_list, index);
}

ParseResult parse_primary_expr(Vector* lexeme_list, size_t* index) {
    if (*index >= lexeme_list->size) {
        ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
        sprintf(res.error, "Syntax error: Unexpected end of input at position %zu", *index);
        return res;
    }
    Lexeme* lexeme = (Lexeme*)vector_get(lexeme_list, *index);
    if (lexeme->type == LEX_PROP) {
        (*index)++;
        SyntaxNode* node = (SyntaxNode*)malloc(sizeof(SyntaxNode));
        node->lexeme = *lexeme;
        node->left = NULL;
        node->right = NULL;
        return (ParseResult) { PARSE_OK, .value = node };
    }
    if (lexeme->type == LEX_PO) {
        (*index)++;
        // if you get a (, you can go from above
        ParseResult res = parse_expr(lexeme_list, index);
        if (res.type == PARSE_ERR) {
            return res;
        }
        if (*index >= lexeme_list->size
            || ((Lexeme*)vector_get(lexeme_list, *index))->type != LEX_PF)
        {
            destroy_syntax_tree(res.value);
            ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
            sprintf(res.error, "Syntax error: Expected ')' at position %zu", *index);
            return res;
        }
        (*index)++;
        return res;
    }
    ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
    sprintf(res.error, "Syntax error: Unexpected lexeme at position %zu", *index);
    return res;
}

void destroy_syntax_tree(SyntaxNode* node) {
    if (node == NULL) {
        return;
    }
    destroy_syntax_tree(node->left);
    destroy_syntax_tree(node->right);
    free(node);
}