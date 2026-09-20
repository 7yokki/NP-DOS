#ifndef NPDOS_PROCESS_H
#define NPDOS_PROCESS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    PROCESS_UNUSED = 0,
    PROCESS_READY,
    PROCESS_BLOCKED,
    PROCESS_EXITED
} process_state_t;

typedef struct {
    uint32_t pid;
    process_state_t state;
    uint64_t entry;
    char name[16];
} process_t;

void process_init(void);
int process_register(const char *name, uint64_t entry);
const process_t *process_get(size_t index);
size_t process_count(void);

#endif
