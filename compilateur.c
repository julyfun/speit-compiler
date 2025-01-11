#include "compilateur.h"

#include "anasem.h"
#include "anasynt.h"
#include "constants.h"

void compile(SyntaxNode* tree) {
    Vector* symbol_table = vector_new(sizeof(int));
    Vector* virtual_machine = vector_new(sizeof(int));
}

void dfs_get_vm_func(SyntaxNode* node, Vector* virtual_machine, Vector* symbol_table, Vector*) {
    if (node == NULL) {
        return;
    }
    if (node->lexeme.type == LEX_PROP) {
        // add to symbols table
        // add to virtual machine
    }
    dfs_and_add_to_symbols_table(node->left, symbol_table, virtual_machine);
    dfs_and_add_to_symbols_table(node->right, symbol_table, virtual_machine);
}