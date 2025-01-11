#include "anasem.h"

#include "anasynt.h"
#include "constants.h"
#include "runtime.h"

// Op and Prop allowed.

SemanticResult analyseur_semantique(Vector* lexeme_list) {
    size_t index = 0;
    ParseResult res = parse_expr(lexeme_list, &index);
    if (res.type == PARSE_ERR) {
        SemanticResult result = (SemanticResult) { SEMANTIC_ERR, .error = "" };
        sprintf(result.error, "Syntax error: %s", res.error);
        return result;
    }
    // go through the tree and check the semantics
    SemanticResult sem_res = check_semantics(res.value);
    if (sem_res.type == SEMANTIC_ERR) {
        destroy_syntax_tree(res.value);
        return sem_res;
    }
    // add this to symbols table
}

// PRODUIT is not allowed.
SemanticResult check_semantics(SyntaxNode* node) {
    if (node->lexeme.type == LEX_PRODUIT) {
        SemanticResult result = (SemanticResult) { SEMANTIC_ERR, .error = "" };
        sprintf(result.error, "Semantics error: PRODUIT is not allowed");
        return result;
    }

    if (node->left) {
        SemanticResult left_res = check_semantics(node->left);
        if (left_res.type == SEMANTIC_ERR) {
            return left_res;
        }
    }
    if (node->right) {
        SemanticResult right_res = check_semantics(node->right);
        if (right_res.type == SEMANTIC_ERR) {
            return right_res;
        }
    }
    return (SemanticResult) { SEMANTIC_OK, .value = node };
}
