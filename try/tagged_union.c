#define DEFINE_TAGGED_UNION(Name, EnumBody, UnionBody) \
    typedef enum EnumBody Name##Tag; \
    typedef struct { \
        Name##Tag tag; \
        union { \
            UnionBody \
        } data; \
    } Name;

/* 示例用法 */
DEFINE_TAGGED_UNION(MyValue, { INT_TYPE, FLOAT_TYPE, CHAR_TYPE }, int i; float f; char* s;)
