#ifndef TYPE_H
#define TYPE_H

#include <stdlib.h>
#include <stdbool.h>

typedef void (*cs_CopyFunc)(void *dest, const void *src, size_t count);
typedef void (*cs_FreeFunc)(void *ptr);

typedef struct cs_Type {
    size_t size;
    bool isReference;
    cs_CopyFunc copy;
    cs_FreeFunc free;
} cs_Type;

#endif // TYPE_H