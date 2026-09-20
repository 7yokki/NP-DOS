#ifndef NPDOS_DEVICES_H
#define NPDOS_DEVICES_H

#include <stdint.h>
#include <stdbool.h>

void serial_init(void);
void serial_putc(char c);
void serial_write(const char *text);
void serial_write_line(const char *text);

void keyboard_init(void);
bool keyboard_poll(char *out);

bool ata_init(void);
int ata_read_sectors(uint32_t lba, uint8_t count, void *buffer);
int ata_write_sectors(uint32_t lba, uint8_t count, const void *buffer);
void parallel_init(void);
void parallel_putc(char c);

#endif
