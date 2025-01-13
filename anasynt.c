#include "anasynt.h"

#include <string.h>

/// For BNF see readme.

/// print tree with indents
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

/// Parses binary operator expressions, handling lower precedence parsing functions.
ParseResult parse_binary_op_expr(
    Vector* lexeme_list,
    size_t* index,
    const char* op,
    ParseResult (*lower_precedence_parse)(Vector*, size_t*)
) {
    ParseResult res = lower_precedence_parse(lexeme_list, index);
    if (res.type == PARSE_ERR) {
        return res;
    }
    // Could be more than one op, could be none
    while (*index < lexeme_list->size) {
        Lexeme* lexeme = (Lexeme*)vector_get(lexeme_list, *index);
        if (lexeme->type != LEX_OP || strcmp(lexeme->value, op) != 0) {
            break;
        }
        (*index)++;
        ParseResult right_res = lower_precedence_parse(lexeme_list, index);
        if (right_res.type == PARSE_ERR) {
            destroy_syntax_tree(res.value);
            return right_res;
        }
        SyntaxNode* node = (SyntaxNode*)malloc(sizeof(SyntaxNode));
        node->lexeme = *lexeme;
        node->left = res.value; // previous result
        node->right = right_res.value;
        res.value = node;
    }
    return res;
}

// [grounded parse funcs]

/// Parses a syntax tree from a list of lexemes.
/// Returns a ParseResult with the syntax tree or an error message
/// (If `index` is not at the end of the list)
ParseResult analyseur_syntaxique(Vector* lexeme_list) {
    size_t index = 0;
    ParseResult res = parse_expr(lexeme_list, &index);
    if (res.type == PARSE_ERR) {
        return res;
    }
    if (index < lexeme_list->size) {
        destroy_syntax_tree(res.value);
        ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
        sprintf(res.error, "Syntax error: Unexpected lexeme at position %zu", index);
        return res;
    }
    return res;
}

/// expr ::= rule | arrow_expr
/// Parses an expression, which can be a rule or an arrow expression.
ParseResult parse_expr(Vector* lexeme_list, size_t* index) {
    ParseResult res = parse_rule(lexeme_list, index);
    if (res.type == PARSE_OK) {
        return res;
    }
    return parse_arrow_expr(lexeme_list, index);
}

ParseResult parse_rule(Vector* lexeme_list, size_t* index) {
    size_t start_index = *index;
    ParseResult res = parse_arrow_expr(lexeme_list, index);
    if (res.type == PARSE_ERR) {
        *index = start_index;
        return res;
    }
    if (*index >= lexeme_list->size) {
        destroy_syntax_tree(res.value);
        *index = start_index;
        return (ParseResult) { PARSE_ERR, .error = "Syntax error: Unexpected end of input" };
    }
    Lexeme* lexeme = (Lexeme*)vector_get(lexeme_list, *index);
    if (lexeme->type != LEX_PRODUIT) {
        destroy_syntax_tree(res.value);
        *index = start_index;
        return (ParseResult) { PARSE_ERR, .error = "Syntax error: Expected '→'" };
    }
    (*index)++;
    ParseResult right_res = parse_arrow_expr(lexeme_list, index);
    if (right_res.type == PARSE_ERR) {
        destroy_syntax_tree(res.value);
        *index = start_index;
        return right_res;
    }
    SyntaxNode* node = (SyntaxNode*)malloc(sizeof(SyntaxNode));
    node->lexeme = *lexeme;
    node->left = res.value;
    node->right = right_res.value;
    return (ParseResult) { PARSE_OK, .value = node };
}

ParseResult parse_arrow_expr(Vector* lexeme_list, size_t* index) {
    return parse_binary_op_expr(lexeme_list, index, "IMPLIQUE", parse_or_expr);
}

ParseResult parse_or_expr(Vector* lexeme_list, size_t* index) {
    return parse_binary_op_expr(lexeme_list, index, "OU", parse_and_expr);
}

ParseResult parse_and_expr(Vector* lexeme_list, size_t* index) {
    return parse_binary_op_expr(lexeme_list, index, "ET", parse_unary_expr);
}

ParseResult parse_unary_expr(Vector* lexeme_list, size_t* index) {
    if (*index >= lexeme_list->size) {
        ParseResult res = (ParseResult) { PARSE_ERR, .error = "" };
        sprintf(res.error, "Syntax error: Unexpected end of input at position %zu", *index);
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

/// Parses primary expressions, handling identifiers and parenthesized expressions.
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
        ParseResult res = parse_arrow_expr(lexeme_list, index);
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