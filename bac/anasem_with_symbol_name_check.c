#include "anasem.h"

#include "anasynt.h"
#include "constants.h"
#include "runtime.h"

// Op and Prop allowed.

SemanticResult analyseur_semantique(Vector* lexeme_list, Vector* symbols_table) {
    size_t index = 0;
    ParseResult res = parse_expr(lexeme_list, &index);
    if (res.type == PARSE_ERR) {
        SemanticResult result = (SemanticResult) { SEMANTIC_ERR, .error = "" };
        sprintf(result.error, "Syntax error: %s", res.error);
        return result;
    }
    // go through the tree and check the semantics
    SemanticResult sem_res = check_semantics(res.value, symbols_table);
    if (sem_res.type == SEMANTIC_ERR) {
        destroy_syntax_tree(res.value);
        return sem_res;
    }
}

// PRODUIT is not allowed.
/*
| Indices |   0 |   1 |   2 |   3 |   4 |   5 |   6 |   7 |   8 |   9 |  10 |  11 |  12 |
|---------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Table   |   0 |   3 |  78 |  79 |  78 |   1 |   0 |   1 |   2 |  79 |  85 |   2 |   2 |
| Signification | l1  | N   | O   | N   | n1  | c1  | ad  | l2  | O   | U   | n2  | c2  |

où
- l1 et l2 désignent les longueurs des noms NON et OU, chaque lettre étant exprimée par son code ascii;
- n1 et n2 désignent le nombre de paramètres des fonctions concernées;
- c1 et c2 désignent les codes Cfa où trouver les exécutables des fonctions, ils correspondent à un numéro dans la machine virtuelle VM;
- ad désigne l'adresse où commence le précédent mot (ici le mot avant OU est NON qui commence à l'adresse 1);
- le 0 de la première case permettra de savoir que la recherche est terminée (et
  échouée, c'est-à-dire que la fonction demandée n'a pas été trouvée dans la table des symboles).
*/
SemanticResult check_semantics(SyntaxNode* node, Vector* symbols_table) {
    if (node->lexeme.type == LEX_PRODUIT) {
        SemanticResult result = (SemanticResult) { SEMANTIC_ERR, .error = "" };
        sprintf(result.error, "Semantics error: PRODUIT is not allowed");
        return result;
    }

    if (node->lexeme.type == LEX_PROP) {
        // check if the symbol is in the table
        // symbol is
        int found = 0;
        int cur = *(int*)vector_get(symbols_table, symbols_table->size - 1);
        while (cur != 0) {
            const int len = *(int*)vector_get(symbols_table, cur);
            char symbol[SYMBOL_LENGTH];
            for (int i = 1; i <= len; i++) {
                symbol[i - 1] = (char)*(int*)vector_get(symbols_table, cur + i);
            }
            symbol[len] = '\0';
            if (strcmp(symbol, node->lexeme.value) == 0) {
                found = 1;
                break;
            }
            cur = *(int*)vector_get(symbols_table, cur - 1);
        }
        if (!found) {
            // add this to the table
            const int start_idx = symbols_table->size;

            const int len = strlen(node->lexeme.value);
            vector_push_back(symbols_table, &len);
            for (int i = 0; i < len; i++) {
                int c = node->lexeme.value[i];
                vector_push_back(symbols_table, &c);
            }

            // at last put a idx to the last symbol
            vector_push_back(symbols_table, &start_idx);
        }
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
