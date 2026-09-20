#include "elf_loader.h"
#include "memory.h"
#include "string.h"

static bool range_ok(uint64_t offset, uint64_t length, size_t total) {
    return offset <= total && length <= (uint64_t)total - offset;
}

bool elf64_validate(const void *image, size_t image_size) {
    if (!image || image_size < sizeof(struct elf64_header)) return false;
    const struct elf64_header *header = image;
    if (header->ident[0] != 0x7F || header->ident[1] != 'E' || header->ident[2] != 'L' || header->ident[3] != 'F') return false;
    if (header->ident[4] != 2 || header->ident[5] != 1 || header->ident[6] != 1) return false;
    if (header->machine != ELF_EM_X86_64) return false;
    if (header->type != ELF_ET_EXEC && header->type != ELF_ET_DYN) return false;
    if (header->phentsize != sizeof(struct elf64_phdr) || header->phnum > 16) return false;
    return range_ok(header->phoff, (uint64_t)header->phnum * header->phentsize, image_size);
}

int elf64_load(const void *image, size_t image_size, elf_page_allocator_t allocator, elf_image_t *result) {
    if (!elf64_validate(image, image_size)) return -1;
    const struct elf64_header *header = image;
    const struct elf64_phdr *programs = (const struct elf64_phdr *)((const uint8_t *)image + header->phoff);
    if (!allocator || !result) return -2;
    memset(result, 0, sizeof(*result));
    result->entry = header->entry;
    for (uint16_t i = 0; i < header->phnum; ++i) {
        const struct elf64_phdr *ph = &programs[i];
        if (ph->type != ELF_PT_LOAD) continue;
        if (ph->memsz < ph->filesz || !range_ok(ph->offset, ph->filesz, image_size)) return -3;
        size_t pages = (size_t)((ph->memsz + 4095) / 4096);
        if (!pages || result->segment_count >= 16) return -4;
        void *destination = allocator(pages);
        if (!destination) return -5;
        memset(destination, 0, pages * 4096);
        memcpy(destination, (const uint8_t *)image + ph->offset, (size_t)ph->filesz);
        elf_loaded_segment_t *segment = &result->segments[result->segment_count++];
        segment->virtual_address = ph->vaddr;
        segment->loaded_address = (uint64_t)(uintptr_t)destination;
        segment->file_size = ph->filesz;
        segment->memory_size = ph->memsz;
        segment->flags = ph->flags;
    }
    return result->segment_count ? 0 : -6;
}

const char *elf64_error(int code) {
    switch (code) {
        case 0: return "ok";
        case -1: return "invalid ELF64 image";
        case -2: return "loader argument missing";
        case -3: return "segment outside image";
        case -4: return "too many or empty load segments";
        case -5: return "out of physical memory";
        case -6: return "no PT_LOAD segment";
        default: return "unknown loader error";
    }
}
