#include <stddef.h>

void *memcpy(void *destination, const void *source, size_t count) {
    unsigned char *d = destination;
    const unsigned char *s = source;
    for (size_t i = 0; i < count; ++i) d[i] = s[i];
    return destination;
}

void *memset(void *destination, int value, size_t count) {
    unsigned char *d = destination;
    for (size_t i = 0; i < count; ++i) d[i] = (unsigned char)value;
    return destination;
}

int memcmp(const void *left, const void *right, size_t count) {
    const unsigned char *a = left;
    const unsigned char *b = right;
    for (size_t i = 0; i < count; ++i) if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
    return 0;
}
