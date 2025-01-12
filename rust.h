#ifndef RUST_H
#define RUST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// not rust style
static inline int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

#endif
