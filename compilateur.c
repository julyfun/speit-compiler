#include "compilateur.h"

#include <string.h>

#include "anasem.h"
#include "anasynt.h"
#include "constants.h"
#include "runtime.h"
#include "rust.h"

void print_vm_and_sym_tb(Vector* vm, Vector* sym_tb) {
    printf("Virtual machine:\n");
    for (size_t i = 0; i < vm->size; i++) {
        printf("%4zu", i);
    }
    puts("");
    for (size_t i = 0; i < vm->size; i++) {
        printf("%4d", *(int*)vector_get(vm, i));
    }
    printf("\n");
    printf("Symbol table:\n");

    for (size_t i = 0; i < sym_tb->size; i++) {
        printf("%4zu", i);
    }
    puts("");

    for (size_t i = 0; i < sym_tb->size; i++) {
        if (is_alpha(*(char*)vector_get(sym_tb, i))) {
            printf("%4c", *(char*)vector_get(sym_tb, i));
            continue;
        }
        printf("%4d", *(int*)vector_get(sym_tb, i));
    }
    printf("\n");
}

void add_builtin(
    Vector* sym_tb,
    Vector* vm,
    Vector* processeur,
    Builtin func,
    int param_num,
    const char* func_name
) {
    const int processeur_addr = processeur->size;
    vector_push_back(processeur, &func);

    const int vm_addr = vm->size;
    const int sym_tb_start_idx = sym_tb->size;

    // [vm]
    const int extern_func_flag = BUILTIN_FUNCTION_FLAG;
    vector_push_back(vm, &extern_func_flag);
    vector_push_back(vm, &sym_tb_start_idx); // find back some information
    vector_push_back(vm, &processeur_addr);

    // [sym]
    const int func_name_len = strlen(func_name);
    vector_push_back(sym_tb, &func_name_len);
    for (int i = 0; i < func_name_len; i++) {
        int value = func_name[i];
        vector_push_back(sym_tb, &value);
    }
    vector_push_back(sym_tb, &param_num);
    vector_push_back(sym_tb, &vm_addr);
    vector_push_back(sym_tb, &sym_tb_start_idx);
}

ParseSymbolTuple parse_symbol(int sym_tb_idx, Vector* sym_tb) {
    ParseSymbolTuple tuple;
    const int symbol_len = *(int*)vector_get(sym_tb, sym_tb_idx);
    for (int i = 1; i <= symbol_len; i++) {
        tuple.name[i - 1] = (char)*(int*)vector_get(sym_tb, sym_tb_idx + i);
    }
    tuple.name[symbol_len] = '\0';
    tuple.param_num = *(int*)vector_get(sym_tb, sym_tb_idx + symbol_len + 1);
    tuple.vm_addr = *(int*)vector_get(sym_tb, sym_tb_idx + symbol_len + 2);
    return tuple;
}

/// # Docs
/// returns `-1` if not found.
int get_func_vm_addr_from_symbol_table(char* func_name, Vector* symbol_table) {
    int op_addr = -1;
    int cur = *(int*)vector_get(symbol_table, symbol_table->size - 1);
    while (cur != 0) {
        ParseSymbolTuple sym_info = parse_symbol(cur, symbol_table);
        if (strcmp(sym_info.name, func_name) == 0) {
            op_addr = sym_info.vm_addr;
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

typedef struct {
    enum {
        COMPILE_FUNC_OK,
        COMPILE_FUNC_ERR,
    } type;

    char error[MSG_LENGTH];
} CompileFuncResult;

CompileFuncResult
compile_add_func(char* input, Vector* virtual_machine, Vector* symbol_table, char* func_name) {
    LexicalResult lex_res = analyseur_lexical(input);
    if (lex_res.type == LEX_ERR) {
        CompileFuncResult result = (CompileFuncResult) { COMPILE_FUNC_ERR, .error = "" };
        strcpy(result.error, lex_res.error);
        return result;
    }
    size_t index = 0;
    ParseResult parse_res = parse_expr(lex_res.value, &index);
    if (parse_res.type == PARSE_ERR) {
        CompileFuncResult result = (CompileFuncResult) { COMPILE_FUNC_ERR, .error = "" };
        strcpy(result.error, parse_res.error);
        return result;
    }
    SemanticResult sem_res = analyseur_semantique(parse_res.value);
    if (sem_res.type == SEMANTIC_ERR) {
        CompileFuncResult result = (CompileFuncResult) { COMPILE_FUNC_ERR, .error = "" };
        strcpy(result.error, sem_res.error);
        return result;
    }

    // [debug]
    print_syntax_tree(sem_res.value, 0, vector_new(sizeof(int)));

    const int vm_addr = virtual_machine->size;
    const int sym_tb_start_idx = symbol_table->size;

    // [vm]
    Vector* func_prop_dict = vector_new(sizeof(DictItem)); // this is in fact var.
    const int external_func_flag = EXTERN_FUNCTION_FLAG;
    vector_push_back(virtual_machine, &external_func_flag);
    vector_push_back(virtual_machine, &sym_tb_start_idx);
    dfs_get_vm_func(sem_res.value, virtual_machine, symbol_table, func_prop_dict);
    const int fin_addr = get_func_vm_addr_from_symbol_table("FIN", symbol_table);
    vector_push_back(virtual_machine, &fin_addr);

    // [sym]
    const int param_num = func_prop_dict->size;

    const int func_name_len = strlen(func_name);
    vector_push_back(symbol_table, &func_name_len);
    for (int i = 0; i < func_name_len; i++) {
        int value = func_name[i];
        vector_push_back(symbol_table, &value);
    }
    vector_push_back(symbol_table, &param_num);
    vector_push_back(symbol_table, &vm_addr);
    vector_push_back(symbol_table, &sym_tb_start_idx);

    return (CompileFuncResult) { COMPILE_FUNC_OK, .error = "" };
}

CompileResult compile(char* filename) {
    Vector* symbol_table = vector_new(sizeof(int));
    Vector* virtual_machine = vector_new(sizeof(int));
    Vector* processeur = vector_new(sizeof(Builtin));
    vector_push_back(symbol_table, &(int) { 0 });
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Non, 1, "NON");
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Ou, 2, "OU");
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Et, 2, "ET");
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Implique, 2, "IMPLIQUE");
    add_builtin(symbol_table, virtual_machine, processeur, (Builtin)&Fin, 0, "FIN");

    FILE* file = fopen(filename, "r");
    char line[256];
    int line_num = 0;
    while (fgets(line, sizeof(line), file)) {
        char func_name[SYMBOL_LENGTH];
        sprintf(func_name, "P%d", line_num);
        CompileFuncResult res = compile_add_func(line, virtual_machine, symbol_table, func_name);
        if (res.type == COMPILE_FUNC_ERR) {
            // destroy
            vector_destroy(symbol_table);
            vector_destroy(virtual_machine);
            vector_destroy(processeur);
            CompileResult ret_res = (CompileResult) { COMPILE_ERR, .error = "" };
            strcpy(ret_res.error, res.error);
            return ret_res;
        }
        line_num++;
    }
    return (CompileResult
    ) { COMPILE_OK,
        .value = (CompileTuple
        ) { .vm = virtual_machine, .sym_tb = symbol_table, .processeur = processeur } };
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
        int op_addr = get_func_vm_addr_from_symbol_table(node->lexeme.value, symbol_table);
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