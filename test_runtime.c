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

    Vector* mem = vector_new(sizeof(int));
    for (int i = params->size - 1; i >= 0; i--) {
        int value = *(int*)vector_get(params, i);
        vector_push_back(mem, &value);
    }
    const int func_vm_addr = get_func_vm_addr_from_symbol_table(main_func_name, sym_tb);
    execute(func_vm_addr, vm, sym_tb, mem, processeur);
    printf("Result: %d\n", *(int*)vector_get(mem, mem->size - 1));
}

int main() {
    Vector* params = vector_new(sizeof(int));
    vector_push_back(params, &(int) { -1 });
    vector_push_back(params, &(int) { 0 });
    vector_push_back(params, &(int) { 0 });
    vector_push_back(params, &(int) { 0 });
    // test_runtime("assets/simple.txt", "P1", params);
    test_runtime("assets/Init.txt", "P0", params);
    return 0;
}