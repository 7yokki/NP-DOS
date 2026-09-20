#include "devices.h"
#include "ext4.h"
#include "string.h"

static ext4_superblock_t superblock;
static bool mounted;

int ext4_mount(void) {
    unsigned char sector_data[1024];
    memset(sector_data, 0, sizeof(sector_data));
    if (ata_read_sectors(2, 2, sector_data) != 0) {
        mounted = false;
        return -1;
    }
    memcpy(&superblock, sector_data, sizeof(superblock));
    if (superblock.magic != EXT4_SUPER_MAGIC) {
        mounted = false;
        return -2;
    }
    mounted = true;
    return 0;
}

bool ext4_is_mounted(void) { return mounted; }
uint32_t ext4_block_size(void) { return 1024U << superblock.log_block_size; }
uint64_t ext4_block_count(void) { return superblock.blocks_count_lo | ((uint64_t)superblock.blocks_count_hi << 32); }
