#include "compilateur.h"

#include "rust.h"

void test_vm_and_tb(char* filename) {
    printf("---\nfilename: ");
    for (size_t i = 0; i < strlen(filename); i++) {
        printf("%c", filename[i]);
    }
    puts("");
    CompileResult res = compile(filename);
    if (res.type == COMPILE_ERR) {
        printf("%s\n", res.error);
        return;
    }
    Vector* vm = res.value.vm;
    Vector* sym_tb = res.value.sym_tb;
    print_vm_and_sym_tb(vm, sym_tb);
}

int main() {
    test_vm_and_tb("assets/simple.txt");
    test_vm_and_tb("assets/Init.txt");
    test_vm_and_tb("assets/Regles.txt");
    return 0;
}
