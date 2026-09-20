#include "vga.h"
#include "io.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile uint16_t *)0xB8000)

static uint8_t row;
static uint8_t column;
static uint8_t color;

static void move_cursor(void) {
    uint16_t position = (uint16_t)row * VGA_WIDTH + column;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)(position >> 8));
}

void vga_set_color(uint8_t new_color) { color = new_color; }

void vga_init(void) {
    row = 0;
    column = 0;
    color = 0x07;
    vga_clear();
    vga_set_cursor_visible(true);
}

void vga_clear(void) {
    for (uint16_t y = 0; y < VGA_HEIGHT; ++y) {
        for (uint16_t x = 0; x < VGA_WIDTH; ++x) {
            VGA_MEMORY[y * VGA_WIDTH + x] = ((uint16_t)color << 8) | ' ';
        }
    }
    row = 0;
    column = 0;
    move_cursor();
}

static void scroll(void) {
    if (row < VGA_HEIGHT) return;
    for (uint16_t y = 1; y < VGA_HEIGHT; ++y) {
        for (uint16_t x = 0; x < VGA_WIDTH; ++x) {
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }
    for (uint16_t x = 0; x < VGA_WIDTH; ++x) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ((uint16_t)color << 8) | ' ';
    }
    row = VGA_HEIGHT - 1;
}

void vga_putc(char c) {
    if (c == '\n') {
        column = 0;
        ++row;
        scroll();
        move_cursor();
        return;
    }
    if (c == '\r') {
        column = 0;
        move_cursor();
        return;
    }
    if (c == '\b') {
        if (column > 0) {
            --column;
            VGA_MEMORY[row * VGA_WIDTH + column] = ((uint16_t)color << 8) | ' ';
        }
        move_cursor();
        return;
    }
    if (column >= VGA_WIDTH) {
        column = 0;
        ++row;
        scroll();
    }
    VGA_MEMORY[row * VGA_WIDTH + column] = ((uint16_t)color << 8) | (uint8_t)c;
    ++column;
    if (column == VGA_WIDTH) {
        column = 0;
        ++row;
        scroll();
    }
    move_cursor();
}

void vga_write(const char *text) {
    while (*text) vga_putc(*text++);
}

void vga_write_line(const char *text) {
    vga_write(text);
    vga_putc('\n');
}

void vga_set_cursor(uint8_t new_row, uint8_t new_column) {
    row = new_row < VGA_HEIGHT ? new_row : VGA_HEIGHT - 1;
    column = new_column < VGA_WIDTH ? new_column : VGA_WIDTH - 1;
    move_cursor();
}

uint16_t vga_get_cursor(void) { return ((uint16_t)row << 8) | column; }

void vga_set_cursor_visible(bool visible) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, visible ? 0x0E : 0x20);
}
