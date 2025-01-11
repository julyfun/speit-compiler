#ifndef RUST_H
#define RUST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_digit(char c) {
    return c >= '0' && c <= '9';
}

int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_space(char c) {
    return c == ' ' || c == '\t';
}

// [deprecated, containers]
#define GENERATE_VECTOR_OF(type, name) \
    typedef struct { \
        type* data; \
        size_t size; \
        size_t capacity; \
        void (*push_back)(struct name*, type); \
        void (*pop_back)(struct name*); \
        void (*free)(struct name*); \
        type (*at)(struct name*, size_t); \
    } name; \
\
    inline void name##_push_back(name* vec, type value); \
    inline void name##_pop_back(name* vec); \
    inline void name##_free(name* vec); \
    inline type name##_at(name* vec, size_t index); \
\
    inline void name##_init(name* vec) { \
        vec->data = NULL; \
        vec->size = 0; \
        vec->capacity = 0; \
        vec->push_back = name##_push_back; \
        vec->free = name##_free; \
        vec->at = name##_at; \
    } \
\
    inline void name##_push_back(name* vec, type value) { \
        if (vec->size == vec->capacity) { \
            vec->capacity = vec->capacity == 0 ? 1 : vec->capacity * 2; \
            vec->data = realloc(vec->data, vec->capacity * sizeof(type)); \
        } \
        vec->data[vec->size++] = value; \
    } \
\
    inline void name##_pop_pack(name* vec) { \
        if (vec->size > 0) { \
            vec->size--; \
        } \
    } \
    \ 
\
    inline void name##_free(name* vec) { \
        free(vec->data); \
        vec->data = NULL; \
        vec->size = 0; \
        vec->capacity = 0; \
    } \
\
    inline type name##_at(name* vec, size_t index) { \
        return vec->data[index]; \
    }

GENERATE_VECTOR_OF(int, VectorInt)
GENERATE_VECTOR_OF(char, VectorChar)

#endif