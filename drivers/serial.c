#include "devices.h"
#include "io.h"

#define COM1 0x3F8

void serial_init(void) {
    outb(COM1 + 1, 0x00); // disable interrupts
    outb(COM1 + 3, 0x80); // divisor latch
    outb(COM1 + 0, 0x03); // 38400 baud divisor
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03); // 8 data, no parity, one stop
    outb(COM1 + 2, 0xC7); // FIFO enable, clear, 14-byte threshold
    outb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR
}

static int transmit_empty(void) { return inb(COM1 + 5) & 0x20; }

void serial_putc(char c) {
    while (!transmit_empty()) { __asm__ volatile ("pause"); }
    outb(COM1, (uint8_t)c);
}

void serial_write(const char *text) {
    while (*text) serial_putc(*text++);
}

void serial_write_line(const char *text) {
    serial_write(text);
    serial_putc('\r');
    serial_putc('\n');
}
