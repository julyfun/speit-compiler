#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct {
    void* data;
    size_t size;
    size_t capacity;
    size_t elem_size;
} Vector;

Vector* vector_new(size_t elem_size);
int vector_push_back(Vector* vec, const void* elem);
void* vector_get(Vector* vec, size_t index);
int vector_pop_back(Vector* vec);
void vector_destroy(Vector* vec);

// int vector_resize(Vector* vec, size_t new_capacity);

#endif // VECTOR_H
