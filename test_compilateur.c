#include "compilateur.h"

#include "rust.h"

void test_vm_and_tb(char* filename) {
    FILE* file = fopen(filename, "r");
    char input[256];
    while (fgets(input, sizeof(input), file)) {
        CompileResult res = compile(input);
        if (res.type == COMPILE_ERR) {
            printf("%s\n", res.error);
            continue;
        }
        Vector* vm = res.value.vm;
        Vector* sym_tb = res.value.sym_tb;
        printf("Virtual machine:\n");
        for (size_t i = 0; i < vm->size; i++) {
            printf("%d ", *(int*)vector_get(vm, i));
        }
        printf("\n");
        printf("Symbol table:\n");
        for (size_t i = 0; i < sym_tb->size; i++) {
            if (is_alpha(*(char*)vector_get(sym_tb, i))) {
                printf("%c ", *(char*)vector_get(sym_tb, i));
                continue;
            }
            printf("%d ", *(int*)vector_get(sym_tb, i));
        }
        printf("\n");
    }
}

int main() {
    test_vm_and_tb("assets/simple.txt");
    test_vm_and_tb("assets/Init.txt");
    test_vm_and_tb("assets/Regles.txt");
    return 0;
}
