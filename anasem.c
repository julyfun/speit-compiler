#include "anasem.h"

#include "anasynt.h"
#include "constants.h"
#include "runtime.h"

// Op and Prop allowed.

SemanticResult check_semantics(SyntaxNode* node);

SemanticResult analyseur_semantique(SyntaxNode* node) {
    // [own]
    // - node

    // go through the tree and check the semantics
    SemanticResult sem_res = check_semantics(node);
    if (sem_res.type == SEMANTIC_ERR) {
        destroy_syntax_tree(node);
        return sem_res;
    }
    // add this to symbols table
    return (SemanticResult) { SEMANTIC_OK, .value = node };
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
