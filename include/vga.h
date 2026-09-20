#ifndef NPDOS_VGA_H
#define NPDOS_VGA_H

#include <stdint.h>
#include <stdbool.h>

void vga_init(void);
void vga_clear(void);
void vga_putc(char c);
void vga_write(const char *text);
void vga_write_line(const char *text);
void vga_set_color(uint8_t color);
void vga_set_cursor(uint8_t row, uint8_t column);
uint16_t vga_get_cursor(void);
void vga_set_cursor_visible(bool visible);

#endif
