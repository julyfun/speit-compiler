#include "vector.h"

int main() {
    Vector* int_vec = vector_new(sizeof(int));

    int value;

    for (int i = 0; i < 10; i++) {
        value = i + 1;
        vector_push_back(int_vec, &value);
        printf("Added: %d\n", value);
    }

    printf("Vector contents:\n");
    for (size_t i = 0; i < int_vec->size; i++) {
        printf("%d ", *(int*)vector_get(int_vec, i));
    }

    printf("\n");

    vector_pop_back(int_vec);

    printf("After pop_back:\n");

    for (size_t i = 0; i < int_vec->size; i++) {
        printf("%d ", *(int*)vector_get(int_vec, i));
    }

    printf("\n");

    vector_destroy(int_vec);
    return 0;
}
