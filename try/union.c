#include <stdio.h>

typedef enum { INT_TYPE, FLOAT_TYPE, CHAR_TYPE } DataType;
// 使用struct结合union实现tagged union
typedef struct {
    DataType type; // 标记当前存储的数据类型
    union {
        int i;
        float f;
        char* s;
    } data;
} Tag;

int main() {
    Tag tags[3] = { { INT_TYPE, .data.i = 42 },
                    { FLOAT_TYPE, .data.f = 3.14 },
                    { CHAR_TYPE, .data.s = "Hello, world!" } };
    for (int i = 0; i < 3; i++) {
        switch (tags[i].type) {
            case INT_TYPE:
                printf("Integer: %d\n", tags[i].data.i);
                break;
            case FLOAT_TYPE:
                printf("Float: %f\n", tags[i].data.f);
                break;
            case CHAR_TYPE:
                printf("String: %s\n", tags[i].data.s);
                break;
        }
    }
    return 0;
}