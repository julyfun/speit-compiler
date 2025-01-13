#include "runtime.h"

#include "compilateur.h"
#include "vector.h"

int Non(int x) {
    return !x;
}

int Et(int x, int y) {
    return x && y;
}

int Ou(int x, int y) {
    return x || y;
}

int Implique(int x, int y) {
    return !x || y;
}

int Fin() {
    return -1;
}

/// function process is a stack.
/// should not run with symbol table.
void execute(int vm_addr, Vector* vm, Vector* sym_tb, Vector* mem, Vector* processeur) {
    const int reg_pos = mem->size;
    const int sym_tb_idx = *(int*)vector_get(vm, vm_addr + 1);
    const ParseSymbolTuple sym_info = parse_symbol(sym_tb_idx, sym_tb);

    // can manipulate mem
    if (*(int*)vector_get(vm, vm_addr) == BUILTIN_FUNCTION_FLAG) {
        const int addr_in_processeur = *(int*)vector_get(vm, vm_addr + 2);
        Builtin builtin = *(Builtin*)vector_get(processeur, addr_in_processeur);
        switch (sym_info.param_num) {
            case 0: {
                const int res = ((int (*)(void))builtin)();
                vector_push_back(mem, &res);
                return;
            }
            case 1: {
                const int sz = mem->size;
                const int res = ((int (*)(int))builtin)(*(int*)vector_get(mem, sz - 1));
                vector_pop_back(mem);
                vector_push_back(mem, &res);
                return;
            }
            case 2: {
                const int sz = mem->size;
                const int res = ((int (*)(int, int)
                )builtin)(*(int*)vector_get(mem, sz - 2), *(int*)vector_get(mem, sz - 1));
                vector_pop_back(mem);
                vector_pop_back(mem);
                vector_push_back(mem, &res);
                return;
            }
            default:
                // panic
                fprintf(
                    stderr,
                    "Runtime panic: too many params for builtin function at vm_addr %d\n",
                    vm_addr
                );
                exit(1);
        }
    }
    // else it is a user-defined function
    vm_addr += 2;
    while (1) {
        const int vm_data = *(int*)vector_get(vm, vm_addr);
        if (vm_data < 0) {
            // param is reversely stacked here
            // vm_data = -1 => (reg_pos - param_num - 1 + 1) which is the first param
            // real_value = 1 for true, 0 for false (as designated in Projet.pdf)
            const int real_value =
                -*(int*)vector_get(mem, reg_pos - sym_info.param_num - 1 - vm_data);
            vector_push_back(mem, &real_value);
        } else {
            // is a func, jump to the func
            // a builtin func will consume the params and
            // a external func will
            execute(vm_data, vm, sym_tb, mem, processeur);
            // could be -1 on the top, which means this function is finished.
            // Then below the top is the return value of this function.
            if (*(int*)vector_get(mem, mem->size - 1) == -1) {
                vector_pop_back(mem); // -1
                const int res = *(int*)vector_get(mem, mem->size - 1); // last func res
                vector_pop_back(mem); // pop res
                for (int i = 0; i < sym_info.param_num; i++) {
                    vector_pop_back(mem); // pop params
                }
                vector_push_back(mem, &res);
                break;
            }
        }
        vm_addr++;
    }
}