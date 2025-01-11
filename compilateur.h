#ifndef COMPILATEUR_H
#define COMPILATEUR_H

#include "anasynt.h"
#include "vector.h"

typedef void (*Builtin)(void);

void add_builtin(Vector* sym_tb, Vector* vm, Builtin func, int param_num, const char* name);

typedef struct {
    Vector* vm;
    Vector* sym_tb;
} CompileTuple;

typedef struct {
    enum {
        COMPILE_OK,
        COMPILE_ERR,
    } type;
    union {
        CompileTuple value;
        char error[MSG_LENGTH];
    };

} CompileResult;

CompileResult compile(char* input);
void dfs_get_vm_func(
    SyntaxNode* node,
    Vector* virtual_machine,
    Vector* symbol_table,
    Vector* prop_dict
);

#endif // COMPILATEUR_H