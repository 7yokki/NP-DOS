#ifndef NPDOS_SERVICES_H
#define NPDOS_SERVICES_H

#include <stdint.h>

void idt_init(void);
void service_int10(void);
uint64_t service_int16(void);
uint64_t service_int13(void);
uint64_t service_int14(void);
uint64_t service_int17(void);

#endif
