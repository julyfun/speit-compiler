#include "compilateur.h"

#include "rust.h"

void test_vm_and_tb() {
    char input[256];
    FILE* file = fopen("assets/simple.txt", "r");
    fgets(input, sizeof(input), file);
    CompileResult res = compile(input);
    if (res.type == COMPILE_ERR) {
        printf("%s\n", res.error);
        return;
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

int main() {
    test_vm_and_tb();
    return 0;
}
