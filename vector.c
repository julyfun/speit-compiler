#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector* vector_new(size_t elem_size) {
    Vector* vec = (Vector*)malloc(sizeof(Vector));
    if (!vec)
        return NULL;

    vec->data = malloc(elem_size * 4);
    if (!vec->data) {
        free(vec);
        return NULL;
    }

    vec->size = 0;
    vec->capacity = 4;
    vec->elem_size = elem_size;
    return vec;
}

static int vector_resize(Vector* vec, size_t new_capacity) {
    void* new_data = realloc(vec->data, new_capacity * vec->elem_size);
    if (!new_data)
        return -1;

    vec->data = new_data;
    vec->capacity = new_capacity;
    return 0;
}

int vector_push_back(Vector* vec, const void* elem) {
    if (vec->size == vec->capacity) {
        if (vector_resize(vec, vec->capacity * 2) != 0)
            return -1;
    }

    memcpy((char*)vec->data + vec->size * vec->elem_size, elem, vec->elem_size);
    vec->size++;
    return 0;
}

void* vector_get(Vector* vec, size_t index) {
    if (index >= vec->size)
        return NULL;
    return (char*)vec->data + index * vec->elem_size;
}

int vector_pop_back(Vector* vec) {
    if (vec->size == 0)
        return -1;
    vec->size--;
    return 0;
}

void vector_destroy(Vector* vec) {
    free(vec->data);
    free(vec);
}
