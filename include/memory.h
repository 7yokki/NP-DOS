#ifndef NPDOS_MEMORY_H
#define NPDOS_MEMORY_H

#include <stdint.h>
#include <stddef.h>

void pmm_init(uint64_t multiboot_info);
void *pmm_alloc_page(void);
void *pmm_alloc_pages(size_t count);
uint64_t pmm_total_pages(void);
uint64_t pmm_free_pages(void);

#endif
