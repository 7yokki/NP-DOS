/* Freestanding C ELF sample for NP_ELF_LOADER. */
static void putc(char c) {
    __asm__ volatile("movb $0x0E, %%ah; int $0x10" : : "a"(c) : "cc");
}

void _start(void) {
    const char *message = "Hello from NP-ELF C\r\n";
    for (const char *p = message; *p; ++p) putc(*p);
    for (;;) __asm__ volatile("hlt");
}
