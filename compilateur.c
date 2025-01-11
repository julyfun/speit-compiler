#include "compilateur.h"

#include <string.h>

#include "anasem.h"
#include "anasynt.h"
#include "constants.h"
#include "runtime.h"

void add_builtin(Vector* sym_tb, Vector* vm, Builtin func, int param_num, const char* name) {
    const int addr = vm->size;
    const int extern_func_flag = EXTERN_FUNCTION_FLAG;
    vector_push_back(vm, &extern_func_flag);
    vector_push_back(vm, &func);

    const int start_idx = sym_tb->size;
    const int len = strlen(name);
    vector_push_back(sym_tb, &len);
    for (int i = 0; i < len; i++) {
        vector_push_back(sym_tb, &name[i]);
    }
    vector_push_back(sym_tb, &param_num);
    vector_push_back(sym_tb, &addr);
    vector_push_back(sym_tb, &start_idx);
}

typedef struct {
    char k[SYMBOL_LENGTH];
    int v;
} DictItem;

CompileResult compile(char* input) {
    LexicalResult lex_res = analyseur_lexical(input);
    if (lex_res.type == LEX_ERR) {
        CompileResult result = (CompileResult) { COMPILE_ERR, .error = "" };
        strcpy(result.error, lex_res.error);
        return result;
    }
    size_t index = 0;
    ParseResult parse_res = parse_expr(lex_res.value, &index);
    if (parse_res.type == PARSE_ERR) {
        CompileResult result = (CompileResult) { COMPILE_ERR, .error = "" };
        strcpy(result.error, parse_res.error);
        return result;
    }
    SemanticResult sem_res = analyseur_semantique(parse_res.value);
    if (sem_res.type == SEMANTIC_ERR) {
        CompileResult result = (CompileResult) { COMPILE_ERR, .error = "" };
        strcpy(result.error, sem_res.error);
        return result;
    }

    Vector* symbol_table = vector_new(sizeof(int));
    Vector* virtual_machine = vector_new(sizeof(int));
    Vector* prop_dict = vector_new(sizeof(DictItem)); // this is in fact var.
    add_builtin(symbol_table, virtual_machine, (Builtin)&Et, 2, "ET");
    add_builtin(symbol_table, virtual_machine, (Builtin)&Ou, 2, "OU");
    add_builtin(symbol_table, virtual_machine, (Builtin)&Non, 1, "NON");
    add_builtin(symbol_table, virtual_machine, (Builtin)&Implique, 2, "IMPLIQUE");
    add_builtin(symbol_table, virtual_machine, (Builtin)&Fin, 0, "FIN");
    dfs_get_vm_func(sem_res.value, virtual_machine, symbol_table, prop_dict);
    return (CompileResult
    ) { COMPILE_OK, .value = (CompileTuple) { .vm = virtual_machine, .sym_tb = symbol_table } };
}

// post-order traversal
void dfs_get_vm_func(
    SyntaxNode* node,
    Vector* virtual_machine,
    Vector* symbol_table,
    Vector* prop_dict
) {
    if (node == NULL) {
        return;
    }
    dfs_get_vm_func(node->left, symbol_table, virtual_machine, prop_dict);
    dfs_get_vm_func(node->right, symbol_table, virtual_machine, prop_dict);

    if (node->lexeme.type == LEX_PROP) {
        // try to find in the prop_dict
        int stack_pos = 0;
        for (size_t i = 0; i < prop_dict->size; i++) {
            const DictItem* item = (DictItem*)vector_get(prop_dict, i);
            if (strcmp(item->k, node->lexeme.value) == 0) {
                stack_pos = item->v;
                break;
            }
        }
        if (!stack_pos) {
            stack_pos = -(virtual_machine->size + 1);
            DictItem item = { .v = stack_pos, .k = "" };
            strcpy(item.k, node->lexeme.value);
            vector_push_back(prop_dict, &item);
        }
        vector_push_back(virtual_machine, &stack_pos);
    } else if (node->lexeme.type == LEX_OP) {
        // must be found in table. There are only 4 builtin functions
        int op_addr = 0;
        int cur = *(int*)vector_get(symbol_table, symbol_table->size - 1);
        while (cur != 0) {
            const int symbol_len = *(int*)vector_get(symbol_table, cur);
            char symbol[SYMBOL_LENGTH];
            for (int i = 1; i <= symbol_len; i++) {
                symbol[i - 1] = (char)*(int*)vector_get(symbol_table, cur + i);
            }
            symbol[symbol_len] = '\0';
            const int symbol_addr = *(int*)vector_get(symbol_table, cur + symbol_len + 2);
            if (strcmp(symbol, node->lexeme.value) == 0) {
                op_addr = symbol_addr;
                break;
            }
        }
        if (!op_addr) {
            // something went wrong
            fprintf(stderr, "Error: operator %s not found in the table\n", node->lexeme.value);
            exit(1);
        }
        vector_push_back(virtual_machine, &op_addr);
    }
}