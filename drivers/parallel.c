#include "devices.h"
#include "io.h"

#define LPT1 0x378

void parallel_init(void) {
    outb(LPT1 + 2, 0x0C); // select, initialize printer
}

void parallel_putc(char c) {
    for (uint32_t i = 0; i < 100000; ++i) {
        if (inb(LPT1 + 1) & 0x80) break;
    }
    outb(LPT1, (uint8_t)c);
    outb(LPT1 + 2, 0x0D); // strobe high
    io_wait();
    outb(LPT1 + 2, 0x0C); // strobe low
}
