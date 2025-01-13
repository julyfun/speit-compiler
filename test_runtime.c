#include "compilateur.h"
#include "runtime.h"
#include "vector.h"

void test_runtime(char* filename, char* main_func_name, Vector* params) {
    CompileResult res = compile(filename);
    if (res.type == COMPILE_ERR) {
        printf("%s\n", res.error);
        return;
    }
    Vector* vm = res.value.vm;
    Vector* sym_tb = res.value.sym_tb;
    Vector* processeur = res.value.processeur;

    // [debug]
    // print_vm_and_sym_tb(vm, sym_tb);
    // [debug]
    printf("---\n");

    const int func_vm_addr = get_func_vm_addr_from_symbol_table(main_func_name, sym_tb);

    // [debug]
    const int sym_tb_idx = *(int*)vector_get(vm, func_vm_addr + 1);
    ParseSymbolTuple sym_info = parse_symbol(sym_tb_idx, sym_tb);
    printf("Entry function information:\n");
    printf("    Name: %s\n", sym_info.name);
    printf("    Param num: %d\n", sym_info.param_num);
    printf("    VM addr: %d\n", sym_info.vm_addr);
    printf("Params:");
    for (size_t i = 0; i < params->size; i++) {
        printf(" %d", *(int*)vector_get(params, i));
    }
    puts("");

    execute(func_vm_addr, vm, sym_tb, params, processeur);

    // [debug]
    printf("Result: %d\n", *(int*)vector_get(params, params->size - 1));
}

int main() {
    {
        Vector* params = vector_new(sizeof(int));
        vector_push_back(params, &(int) { -1 });
        vector_push_back(params, &(int) { 0 });
        vector_push_back(params, &(int) { 0 });
        vector_push_back(params, &(int) { 0 });
        // test_runtime("assets/simple.txt", "P1", params);
        test_runtime("assets/Init.txt", "P0", params);
        vector_destroy(params);
    }

    {
        Vector* params2 = vector_new(sizeof(int));
        vector_push_back(params2, &(int) { -1 });
        vector_push_back(params2, &(int) { 0 });
        test_runtime("assets/simple.txt", "P1", params2);
        vector_destroy(params2);
    }
    return 0;
}
