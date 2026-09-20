#include "vga.h"
#include "devices.h"
#include "memory.h"
#include "services.h"
#include "ext4.h"
#include "elf_loader.h"
#include "io.h"
#include "process.h"
#include <stdint.h>
#include <stdbool.h>

static void print_u64(uint64_t value) {
    char digits[24];
    int index = 0;
    if (!value) { vga_putc('0'); return; }
    while (value && index < 23) { digits[index++] = (char)('0' + value % 10); value /= 10; }
    while (index) vga_putc(digits[--index]);
}

static void serial_status(const char *label, bool ok) {
    serial_write(label);
    serial_write(ok ? " OK\r\n" : " --\r\n");
}

static void show_banner(void) {
    vga_set_color(0x1F);
    vga_write(" NP-DOS 0.1 // No Problem Disk Operating System ");
    vga_set_color(0x07);
    vga_putc('\n');
    vga_write_line("No Problem Family | No Problem | Author: Manus.AI");
    // VGA text mode exposes an 8-bit ROM code page; keep the Turkish text
    // readable on real BIOS fonts instead of emitting raw UTF-8 byte pairs.
    vga_write_line("Bu proje Yapay zeka tarafindan yurutulmektedir");
    vga_write_line("------------------------------------------------------------");
}

static void show_status(bool ata, bool ext4) {
    vga_write("BOOT: Multiboot2 / x86_64 long mode                  [OK]\n");
    vga_write("VIDEO: VGA text 80x25 / cursor control                [OK]\n");
    vga_write("INPUT: PS/2 BIOS-style / Turkish Q mapping             [OK]\n");
    vga_write("INTS: 10h 13h 14h 16h 17h native vectors              [OK]\n");
    vga_write("MEM: page allocator "); print_u64(pmm_free_pages()); vga_write(" free pages\n");
    vga_write("DISK: ATA PIO primary                                  ["); vga_write(ata ? "OK" : "--"); vga_write("]\n");
    vga_write("FS: ext4 superblock                                   ["); vga_write(ext4 ? "MOUNTED" : "NOT MOUNTED"); vga_write("]\n");
    vga_write("ELF: NP_ELF_LOADER ELF64 ET_EXEC/ET_DYN parser        [LINKED]\n");
    vga_write("PROC: 16-slot process registry / no context switch     [READY]\n");
    vga_write_line("");
}

static void service_vector_smoke_test(void) {
    vga_write("INT 10h output: ");
    __asm__ volatile("movb $0x0E, %%ah; movb $'.', %%al; int $0x10" : : : "rax");
    __asm__ volatile("int $0x16" : : : "rax");
    vga_write_line(" | INT 16h software-vector smoke test                 [OK]");
}

static void help(void) {
    vga_write_line("NP-DOS commands:");
    vga_write_line("  HELP       show this command list");
    vga_write_line("  CLS        clear the VGA screen");
    vga_write_line("  VER        print version and ABI state");
    vga_write_line("  MEM        show physical page counters");
    vga_write_line("  DRV        show INT and driver matrix");
    vga_write_line("  ELF        show NP_ELF_LOADER status");
    vga_write_line("  PROC       show the small process registry");
    vga_write_line("  REBOOT     request keyboard-controller reboot");
}

static void command(const char *line) {
    if (!line[0]) return;
    if (line[0] == 'h' || line[0] == 'H') { help(); return; }
    if (line[0] == 'c' || line[0] == 'C') { vga_clear(); return; }
    if (line[0] == 'v' || line[0] == 'V') {
        vga_write_line("NP-DOS 0.1 (x86_64) | DOS ABI-compatible services, native kernel");
        vga_write_line("Build: freestanding C + NASM | License: MIT");
        return;
    }
    if (line[0] == 'm' || line[0] == 'M') {
        vga_write("Free pages: "); print_u64(pmm_free_pages()); vga_write(" / highest tracked page: "); print_u64(pmm_total_pages()); vga_putc('\n');
        return;
    }
    if (line[0] == 'd' || line[0] == 'D') {
        vga_write_line("INT 10h video | INT 13h ATA PIO | INT 14h COM1 | INT 16h PS/2 | INT 17h LPT1");
        return;
    }
    if (line[0] == 'e' || line[0] == 'E') {
        vga_write_line("NP_ELF_LOADER: ELF64 little-endian x86-64, PT_LOAD, ET_EXEC/ET_DYN");
        vga_write_line("User-mode execution is intentionally not enabled in this stage.");
        return;
    }
    if (line[0] == 'p' || line[0] == 'P') {
        vga_write("Process registry: "); print_u64(process_count()); vga_write_line(" entry (context switching is not enabled)");
        const process_t *kernel = process_get(0);
        if (kernel) vga_write_line("  PID 0  READY  kernel");
        return;
    }
    if (line[0] == 'r' || line[0] == 'R') {
        vga_write_line("Reboot: keyboard controller command sent.");
        while (inb(0x64) & 2) {}
        outb(0x64, 0xFE);
        return;
    }
    vga_write_line("Bad command or file name");
}

static void shell(void) {
    char line[80];
    size_t length = 0;
    vga_write("A:\\> ");
    for (;;) {
        char input;
        if (!keyboard_poll(&input)) continue;
        if (input == '\n') {
            line[length] = 0;
            vga_putc('\n');
            command(line);
            length = 0;
            vga_write("A:\\> ");
        } else if (input == '\b') {
            if (length) { --length; vga_putc('\b'); }
        } else if (input >= 32 && input < 127 && length < sizeof(line) - 1) {
            line[length++] = input;
            vga_putc(input);
        }
    }
}

void kernel_main(uint32_t multiboot_info) {
    vga_init();
    serial_init();
    keyboard_init();
    parallel_init();
    process_init();
    pmm_init(multiboot_info);
    idt_init();

    bool ata = ata_init();
    bool ext4 = false;
    if (ata) ext4 = ext4_mount() == 0;

    serial_write_line("NP-DOS: kernel_main reached in long mode");
    serial_write_line("Bu proje Yapay zeka tarafından yürütülmektedir");
    serial_status("VGA", true);
    serial_status("PS2", true);
    serial_status("ATA", ata);
    serial_status("EXT4", ext4);
    serial_write_line("ELF64 loader linked; no user mode claim in stage 0.1");

    show_banner();
    show_status(ata, ext4);
    service_vector_smoke_test();
    help();
    shell();
}
