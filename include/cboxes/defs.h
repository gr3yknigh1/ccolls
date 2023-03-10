#ifndef DEFS_H_
#define DEFS_H_

#include <stdio.h>

#define CS_ASSERT(X, ...)                                                      \
    do {                                                                       \
        if (!(X)) {                                                            \
            fprintf(stderr, "Error: " #X "!= true");                           \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

#endif // DEFS_H_
