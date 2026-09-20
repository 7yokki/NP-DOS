#include "memory.h"
#include "types.h"
#include "vga.h"
#include "services.h"

#define PAGE_SIZE 4096ULL
#define MAX_PHYSICAL_MEMORY (1024ULL * 1024ULL * 1024ULL)
#define MAX_PAGES (MAX_PHYSICAL_MEMORY / PAGE_SIZE)
#define BITMAP_WORDS (MAX_PAGES / 64)

static uint64_t page_bitmap[BITMAP_WORDS];
static uint64_t total_pages;
static uint64_t free_pages;
extern char __kernel_end;

static void mark_page(uint64_t page, bool used) {
    if (page >= MAX_PAGES) return;
    uint64_t mask = 1ULL << (page & 63);
    if (used) page_bitmap[page >> 6] |= mask;
    else page_bitmap[page >> 6] &= ~mask;
}

static bool page_used(uint64_t page) {
    return (page_bitmap[page >> 6] >> (page & 63)) & 1;
}

void pmm_init(uint64_t info_address) {
    for (uint64_t i = 0; i < BITMAP_WORDS; ++i) page_bitmap[i] = UINT64_MAX;
    total_pages = 0;
    free_pages = 0;

    multiboot_info_header_t *info = (multiboot_info_header_t *)(uintptr_t)info_address;
    bool found_map = false;
    if (info) {
        uint8_t *cursor = (uint8_t *)info + 8;
        uint8_t *end = (uint8_t *)info + info->total_size;
        while (cursor + 8 <= end) {
            multiboot_tag_t *tag = (multiboot_tag_t *)cursor;
            if (tag->type == 0) break;
            if (tag->type == 6) {
                found_map = true;
                uint32_t entry_size = *(uint32_t *)(cursor + 8);
                if (entry_size < sizeof(multiboot_mmap_entry_t)) break;
                for (uint8_t *entry_ptr = cursor + 16; entry_ptr + sizeof(multiboot_mmap_entry_t) <= cursor + tag->size;) {
                    multiboot_mmap_entry_t *entry = (multiboot_mmap_entry_t *)entry_ptr;
                    if (entry->type == 1) {
                        uint64_t begin = (entry->base + PAGE_SIZE - 1) / PAGE_SIZE;
                        uint64_t end_page = (entry->base + entry->length) / PAGE_SIZE;
                        if (end_page > MAX_PAGES) end_page = MAX_PAGES;
                        for (uint64_t p = begin; p < end_page; ++p) mark_page(p, false);
                    }
                    entry_ptr += entry_size;
                }
            }
            cursor += (tag->size + 7) & ~7U;
        }
    }

    if (!found_map) {
        for (uint64_t p = 0x100000 / PAGE_SIZE; p < (64ULL * 1024 * 1024) / PAGE_SIZE; ++p) mark_page(p, false);
    }

    // Keep low memory, the kernel image, and the allocator metadata reserved.
    for (uint64_t p = 0; p < 0x100000 / PAGE_SIZE; ++p) mark_page(p, true);
    uint64_t kernel_end_page = ((uint64_t)(uintptr_t)&__kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint64_t p = 0x100000 / PAGE_SIZE; p <= kernel_end_page; ++p) mark_page(p, true);
    for (uint64_t p = 0; p < MAX_PAGES; ++p) {
        if (!page_used(p)) { ++free_pages; if (p + 1 > total_pages) total_pages = p + 1; }
    }
    if (!total_pages) total_pages = 1024;
}

void *pmm_alloc_page(void) {
    for (uint64_t word = 0; word < BITMAP_WORDS; ++word) {
        uint64_t free_bits = ~page_bitmap[word];
        if (!free_bits) continue;
        uint64_t bit = (uint64_t)__builtin_ctzll(free_bits);
        uint64_t page = word * 64 + bit;
        mark_page(page, true);
        if (free_pages) --free_pages;
        return (void *)(uintptr_t)(page * PAGE_SIZE);
    }
    return NULL;
}

void *pmm_alloc_pages(size_t count) {
    if (!count) return NULL;
    for (uint64_t start = 0; start + count <= MAX_PAGES; ++start) {
        bool available = true;
        for (size_t i = 0; i < count; ++i) if (page_used(start + i)) { available = false; break; }
        if (!available) continue;
        for (size_t i = 0; i < count; ++i) mark_page(start + i, true);
        free_pages = free_pages >= count ? free_pages - count : 0;
        return (void *)(uintptr_t)(start * PAGE_SIZE);
    }
    return NULL;
}

uint64_t pmm_total_pages(void) { return total_pages; }
uint64_t pmm_free_pages(void) { return free_pages; }
