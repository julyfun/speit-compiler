#include "compilateur.h"

#include <string.h>

#include "anasem.h"
#include "anasynt.h"
#include "constants.h"
#include "runtime.h"

void add_builtin(
    Vector* sym_tb,
    Vector* vm,
    Vector* processeur,
    Builtin func,
    int param_num,
    const char* name
) {
    const int processeur_addr = processeur->size;
    vector_push_back(processeur, &func);

    const int vm_addr = vm->size;
    const int extern_func_flag = BUILTIN_FUNCTION_FLAG;
    vector_push_back(vm, &extern_func_flag);
    vector_push_back(vm, &processeur_addr);

    const int start_idx = sym_tb->size;
    const int len = strlen(name);
    vector_push_back(sym_tb, &len);
    for (int i = 0; i < len; i++) {
        int value = name[i];
        vector_push_back(sym_tb, &value);
    }
    vector_push_back(sym_tb, &param_num);
    vector_push_back(sym_tb, &vm_addr);
    vector_push_back(sym_tb, &start_idx);
}

int get_func_addr_from_symbol_table(char* func_name, Vector* symbol_table) {
    int op_addr = -1;
    int cur = *(int*)vector_get(symbol_table, symbol_table->size - 1);
    while (cur != 0) {
        const int symbol_len = *(int*)vector_get(symbol_table, cur);
        char symbol[SYMBOL_LENGTH];
        for (int i = 1; i <= symbol_len; i++) {
            symbol[i - 1] = (char)*(int*)vector_get(symbol_table, cur + i);
        }
        symbol[symbol_len] = '\0';
        const int symbol_addr = *(int*)vector_get(symbol_table, cur + symbol_len + 2);
        if (strcmp(symbol, func_name) == 0) {
            op_addr = symbol_addr;
            break;
        }
        cur = *(int*)vector_get(symbol_table, cur - 1);
    }
    return op_addr;
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

    // [debug]
    print_syntax_tree(sem_res.value, 0, vector_new(sizeof(int)));

    Vector* symbol_table = vector_new(sizeof(int));
    Vector* virtual_machine = vector_new(sizeof(int));
    Vector* processeur = vector_new(sizeof(Builtin));
    Vector* prop_dict = vector_new(sizeof(DictItem)); // this is in fact var.
    int zero = 0;
    vector_push_back(symbol_table, &zero);
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Non, 1, "NON");
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Ou, 2, "OU");
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Et, 2, "ET");
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Implique, 2, "IMPLIQUE");
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Fin, 0, "FIN");

    const int external_func_flag = EXTERN_FUNCTION_FLAG;
    vector_push_back(virtual_machine, &external_func_flag);
    dfs_get_vm_func(sem_res.value, virtual_machine, symbol_table, prop_dict);
    const int fin_addr = get_func_addr_from_symbol_table("FIN", symbol_table);
    vector_push_back(virtual_machine, &fin_addr);
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
    dfs_get_vm_func(node->left, virtual_machine, symbol_table, prop_dict);
    dfs_get_vm_func(node->right, virtual_machine, symbol_table, prop_dict);

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
            stack_pos = -(prop_dict->size + 1);
            DictItem item = { .v = stack_pos, .k = "" };
            strcpy(item.k, node->lexeme.value);
            vector_push_back(prop_dict, &item);
        }
        vector_push_back(virtual_machine, &stack_pos);
    } else if (node->lexeme.type == LEX_OP) {
        // must be found in table. There are only 4 builtin functions
        int op_addr = get_func_addr_from_symbol_table(node->lexeme.value, symbol_table);
        if (op_addr == -1) {
            // something went wrong
            fprintf(
                stderr,
                "Compile panic: operator %s not found in the table\n",
                node->lexeme.value
            );
            exit(1);
        }
        vector_push_back(virtual_machine, &op_addr);
    }
}