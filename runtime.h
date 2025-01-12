#ifndef RUNTIME_H
#define RUNTIME_H

#include "vector.h"

int Non(int x);
int Et(int x, int y);
int Ou(int x, int y);
int Implique(int x, int y);
int Fin();

void execute(int vm_addr, Vector* vm, Vector* sym_tb, Vector* mem, Vector* processeur);

#endif // RUNTIME_H
