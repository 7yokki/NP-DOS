#include "services.h"
#include "io.h"
#include "vga.h"
#include "devices.h"
#include <stdint.h>

struct idt_gate {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

struct idtr { uint16_t limit; uint64_t base; } __attribute__((packed));

static struct idt_gate idt[256];
extern void isr_default(void);
extern void isr10(void);
extern void isr13(void);
extern void isr14(void);
extern void isr16(void);
extern void isr17(void);

static void set_gate(uint8_t vector, void (*handler)(void)) {
    uint64_t address = (uint64_t)(uintptr_t)handler;
    idt[vector].offset_low = (uint16_t)address;
    idt[vector].selector = 0x08;
    idt[vector].ist = 0;
    idt[vector].type_attr = 0x8E;
    idt[vector].offset_mid = (uint16_t)(address >> 16);
    idt[vector].offset_high = (uint32_t)(address >> 32);
    idt[vector].reserved = 0;
}

void idt_init(void) {
    for (uint16_t i = 0; i < 256; ++i) set_gate((uint8_t)i, isr_default);
    set_gate(0x10, isr10);
    set_gate(0x13, isr13);
    set_gate(0x14, isr14);
    set_gate(0x16, isr16);
    set_gate(0x17, isr17);
    struct idtr descriptor = { sizeof(idt) - 1, (uint64_t)(uintptr_t)idt };
    __asm__ volatile ("lidt %0" : : "m"(descriptor));
}

void service_int10(void) {
    // DOS-compatible software vector: the stage-1 ABI uses AL as the character.
    uint64_t rax;
    __asm__ volatile ("mov %%rax, %0" : "=r"(rax));
    vga_putc((char)(rax & 0xFF));
}

uint64_t service_int16(void) {
    char key = 0;
    return keyboard_poll(&key) ? (uint64_t)(uint8_t)key : 0;
}

uint64_t service_int13(void) {
    uint64_t rax, rdx, rdi;
    __asm__ volatile ("mov %%rax, %0; mov %%rdx, %1; mov %%rdi, %2" : "=r"(rax), "=r"(rdx), "=r"(rdi));
    uint8_t function = (uint8_t)(rax >> 8);
    uint8_t count = (uint8_t)rax;
    if (function == 0x02) return (uint64_t)ata_read_sectors((uint32_t)rdx, count, (void *)(uintptr_t)rdi);
    if (function == 0x03) return (uint64_t)ata_write_sectors((uint32_t)rdx, count, (const void *)(uintptr_t)rdi);
    return (uint64_t)-5;
}

uint64_t service_int14(void) {
    uint64_t rax;
    __asm__ volatile ("mov %%rax, %0" : "=r"(rax));
    if ((uint8_t)(rax >> 8) == 0x01) serial_putc((char)rax);
    return 0;
}

uint64_t service_int17(void) {
    uint64_t rax;
    __asm__ volatile ("mov %%rax, %0" : "=r"(rax));
    if ((uint8_t)(rax >> 8) == 0x00) parallel_putc((char)rax);
    return 0;
}
