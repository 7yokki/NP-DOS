#ifndef NPDOS_EXT4_H
#define NPDOS_EXT4_H

#include <stdint.h>
#include <stdbool.h>

#define EXT4_SUPER_MAGIC 0xEF53

typedef struct {
    uint32_t inodes_count;
    uint32_t blocks_count_lo;
    uint32_t reserved_blocks_count_lo;
    uint32_t free_blocks_count_lo;
    uint32_t free_inodes_count;
    uint32_t first_data_block;
    uint32_t log_block_size;
    uint32_t log_cluster_size;
    uint32_t blocks_per_group;
    uint32_t clusters_per_group;
    uint32_t inodes_per_group;
    uint32_t mtime;
    uint32_t wtime;
    uint16_t mount_count;
    uint16_t max_mount_count;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;
    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;
    uint16_t def_resuid;
    uint16_t def_resgid;
    uint32_t first_ino;
    uint16_t inode_size;
    uint16_t block_group_nr;
    uint32_t feature_compat;
    uint32_t feature_incompat;
    uint32_t feature_ro_compat;
    uint8_t uuid[16];
    char volume_name[16];
    char last_mounted[64];
    uint32_t algorithm_usage_bitmap;
    uint8_t padding[0x150 - 0xCC];
    uint32_t blocks_count_hi;
} __attribute__((packed)) ext4_superblock_t;

int ext4_mount(void);
bool ext4_is_mounted(void);
uint32_t ext4_block_size(void);
uint64_t ext4_block_count(void);

#endif
