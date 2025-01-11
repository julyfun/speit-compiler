#include "anasynt.h"

typedef struct {
    enum { PARSE_OK, PARSE_ERR } type;
    union {
        SyntaxNode* value;
        char error[MSG_LENGTH];
    };
} ParseResult;

ParseResult parse_and_expr(Vector* lexeme_list, size_t* index) {
    ParseResult left_res = parse_unary_expr(lexeme_list, index);
    if (left_res.type == PARSE_ERR) {
        return left_res;
    }
    while (*index < lexeme_list->size) {
        Lexeme* lexeme = (Lexeme*)vector_get(lexeme_list, *index);
        if (lexeme->type != LEX_OP || strcmp(lexeme->value, "ET") != 0) {
            break;
        }
        (*index)++;
        ParseResult right_res = parse_unary_expr(lexeme_list, index);
        if (right_res.type == PARSE_ERR) {
            destroy_syntax_tree(left_res.value);
            return right_res;
        }
        SyntaxNode* node = (SyntaxNode*)malloc(sizeof(SyntaxNode));
        node->lexeme = *lexeme;
        node->left = left_res.value;
        node->right = right_res.value;
        left_res.value = node;
    }
}

ParseResult parse_unary_expr(Vector* lexeme_list, size_t* index) {
    if (*index >= lexeme_list->size) {
        ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
        sprintf(res.error, "Unexpected end of input at position %zu", *index);
        return res;
    }
    Lexeme* lexeme = (Lexeme*)vector_get(lexeme_list, *index);
    if (lexeme->type == LEX_OP && strcmp(lexeme->value, "NON") == 0) {
        (*index)++;
        ParseResult res = parse_unary_expr(lexeme_list, index);
        if (res.type == PARSE_ERR) {
            return res;
        }
        SyntaxNode* node = (SyntaxNode*)malloc(sizeof(SyntaxNode));
        node->lexeme = *lexeme;
        node->left = res.value;
        node->right = NULL;
        return (ParseResult) { PARSE_OK, .value = node };
    }
    // else suppose to be primary_expr
    return parse_primary_expr(lexeme_list, index);
}

ParseResult parse_primary_expr(Vector* lexeme_list, size_t* index) {
    if (*index >= lexeme_list->size) {
        ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
        sprintf(res.error, "Unexpected end of input at position %zu", *index);
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
        ParseResult res = parse_arrow_expr(lexeme_list, index);
        if (res.type == PARSE_ERR) {
            return res;
        }
        if (*index >= lexeme_list->size
            || ((Lexeme*)vector_get(lexeme_list, *index))->type != LEX_PF)
        {
            destroy_syntax_tree(res.value);
            ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
            sprintf(res.error, "Expected ')' at position %zu", *index);
            return res;
        }
        (*index)++;
        return res;
    }
}

void distory_syntax_tree(SyntaxNode* node) {
    if (node == NULL) {
        return;
    }
    destroy_syntax_tree(node->left);
    destroy_syntax_tree(node->right);
    free(node);
}