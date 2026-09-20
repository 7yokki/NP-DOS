#include "devices.h"
#include "io.h"

#define ATA_DATA 0x1F0
#define ATA_SECCOUNT 0x1F2
#define ATA_LBA0 0x1F3
#define ATA_LBA1 0x1F4
#define ATA_LBA2 0x1F5
#define ATA_DRIVE 0x1F6
#define ATA_STATUS 0x1F7
#define ATA_COMMAND 0x1F7
#define ATA_ALTSTATUS 0x3F6
#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30
#define ATA_CMD_IDENTIFY 0xEC

static bool present;

static int wait_not_busy(void) {
    for (uint32_t i = 0; i < 1000000; ++i) {
        uint8_t status = inb(ATA_STATUS);
        if (!(status & 0x80)) return (status & 0x01) ? -1 : 0;
    }
    return -2;
}

static int wait_drq(void) {
    for (uint32_t i = 0; i < 1000000; ++i) {
        uint8_t status = inb(ATA_STATUS);
        if (status & 0x08) return 0;
        if (status & 0x01) return -1;
    }
    return -2;
}

bool ata_init(void) {
    outb(ATA_DRIVE, 0xA0);
    io_wait();
    outb(ATA_COMMAND, ATA_CMD_IDENTIFY);
    if (!inb(ATA_STATUS)) { present = false; return false; }
    if (wait_not_busy() < 0 || wait_drq() < 0) { present = false; return false; }
    for (int i = 0; i < 256; ++i) (void)inw(ATA_DATA);
    present = true;
    return true;
}

int ata_read_sectors(uint32_t lba, uint8_t count, void *buffer) {
    if (!present || !count) return -1;
    uint16_t *out = (uint16_t *)buffer;
    for (uint8_t sector = 0; sector < count; ++sector) {
        if (wait_not_busy() < 0) return -2;
        outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
        outb(ATA_SECCOUNT, 1);
        outb(ATA_LBA0, (uint8_t)lba);
        outb(ATA_LBA1, (uint8_t)(lba >> 8));
        outb(ATA_LBA2, (uint8_t)(lba >> 16));
        outb(ATA_COMMAND, ATA_CMD_READ);
        if (wait_drq() < 0) return -3;
        for (int i = 0; i < 256; ++i) *out++ = inw(ATA_DATA);
        ++lba;
    }
    return 0;
}

int ata_write_sectors(uint32_t lba, uint8_t count, const void *buffer) {
    if (!present || !count) return -1;
    const uint16_t *in = (const uint16_t *)buffer;
    for (uint8_t sector = 0; sector < count; ++sector) {
        if (wait_not_busy() < 0) return -2;
        outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
        outb(ATA_SECCOUNT, 1);
        outb(ATA_LBA0, (uint8_t)lba);
        outb(ATA_LBA1, (uint8_t)(lba >> 8));
        outb(ATA_LBA2, (uint8_t)(lba >> 16));
        outb(ATA_COMMAND, ATA_CMD_WRITE);
        if (wait_drq() < 0) return -3;
        for (int i = 0; i < 256; ++i) outw(ATA_DATA, *in++);
        outb(ATA_COMMAND, 0xE7); // cache flush
        if (wait_not_busy() < 0) return -4;
        ++lba;
    }
    return 0;
}
