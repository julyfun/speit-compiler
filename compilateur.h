#ifndef COMPILATEUR_H
#define COMPILATEUR_H

/*
VM layout:
flag0 sym_tb_start_idx proc_addr
flag1 sym_tb_start_idx [func entity]
*/

#include "anasynt.h"
#include "vector.h"

typedef void (*Builtin)(void);

void print_vm_and_sym_tb(Vector* vm, Vector* sym_tb);

int get_func_vm_addr_from_symbol_table(char* func_name, Vector* symbol_table);

void add_builtin(
    Vector* sym_tb,
    Vector* vm,
    Vector* processeur,
    Builtin func,
    int param_num,
    const char* name
);

typedef struct {
    Vector* vm;
    Vector* sym_tb;
    Vector* processeur;
} CompileTuple;

typedef struct {
    char name[SYMBOL_LENGTH];
    int param_num;
    int vm_addr;
} ParseSymbolTuple;

ParseSymbolTuple parse_symbol(int sym_tb_addr, Vector* sym_tb);

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

CompileResult compile(char* filename);

void dfs_get_vm_func(
    SyntaxNode* node,
    Vector* virtual_machine,
    Vector* symbol_table,
    Vector* prop_dict
);

#endif // COMPILATEUR_H