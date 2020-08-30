#include <stddef.h>

void* memcpy (void* destination, const void* source, size_t num) {
    long *d = destination;
    const long *s = source;
    for (size_t i = 0; i < num; i++)
    {
        d[i] = s[i];
    }
    return destination;
}