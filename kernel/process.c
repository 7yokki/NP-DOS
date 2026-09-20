#include "process.h"

#define MAX_PROCESSES 16

static process_t table[MAX_PROCESSES];
static size_t used;
static uint32_t next_pid;

void process_init(void) {
    for (size_t i = 0; i < MAX_PROCESSES; ++i) {
        table[i].pid = 0;
        table[i].state = PROCESS_UNUSED;
        table[i].entry = 0;
        table[i].name[0] = 0;
    }
    used = 0;
    next_pid = 1;
    table[0].pid = 0;
    table[0].state = PROCESS_READY;
    table[0].entry = 0;
    table[0].name[0] = 'k'; table[0].name[1] = 'e'; table[0].name[2] = 'r'; table[0].name[3] = 'n';
    table[0].name[4] = 'e'; table[0].name[5] = 'l'; table[0].name[6] = 0;
    used = 1;
}

int process_register(const char *name, uint64_t entry) {
    if (!name || used >= MAX_PROCESSES) return -1;
    size_t slot = 0;
    while (slot < MAX_PROCESSES && table[slot].state != PROCESS_UNUSED) ++slot;
    if (slot == MAX_PROCESSES) return -2;
    table[slot].pid = next_pid++;
    table[slot].state = PROCESS_READY;
    table[slot].entry = entry;
    size_t i = 0;
    for (; i + 1 < sizeof(table[slot].name) && name[i]; ++i) table[slot].name[i] = name[i];
    table[slot].name[i] = 0;
    ++used;
    return (int)table[slot].pid;
}

const process_t *process_get(size_t index) {
    return index < MAX_PROCESSES && table[index].state != PROCESS_UNUSED ? &table[index] : 0;
}

size_t process_count(void) { return used; }
