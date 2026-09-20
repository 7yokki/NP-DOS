#ifndef NPDOS_ELF_LOADER_H
#define NPDOS_ELF_LOADER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ELF_PT_LOAD 1
#define ELF_ET_EXEC 2
#define ELF_ET_DYN 3
#define ELF_EM_X86_64 62

struct elf64_header {
    unsigned char ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __attribute__((packed));

struct elf64_phdr {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
} __attribute__((packed));

typedef void *(*elf_page_allocator_t)(size_t pages);

typedef struct {
    uint64_t virtual_address;
    uint64_t loaded_address;
    uint64_t file_size;
    uint64_t memory_size;
    uint32_t flags;
} elf_loaded_segment_t;

typedef struct {
    uint64_t entry;
    uint64_t load_bias;
    uint16_t segment_count;
    elf_loaded_segment_t segments[16];
} elf_image_t;

bool elf64_validate(const void *image, size_t image_size);
int elf64_load(const void *image, size_t image_size, elf_page_allocator_t allocator, elf_image_t *result);
const char *elf64_error(int code);

#endif
