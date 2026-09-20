; NP-DOS boot entry. GRUB Multiboot2 loads us in 32-bit protected mode.
; We install identity paging for the first 1 GiB and enter long mode ourselves.

BITS 32

SECTION .multiboot
ALIGN 8
multiboot_header:
    dd 0xE85250D6                 ; magic
    dd 0                          ; i386 protected-mode architecture
    dd multiboot_header_end - multiboot_header
    dd -(0xE85250D6 + 0 + (multiboot_header_end - multiboot_header))
    dw 0                          ; end tag
    dw 0
    dd 8
multiboot_header_end:

SECTION .text
extern kernel_main

global _start
_start:
    cli
    mov esp, stack_top
    mov ebp, 0

    ; GRUB passes the Multiboot2 information address in EBX.
    mov [multiboot_info_ptr], ebx

    ; Clear the page-table storage so no stale flags can be interpreted.
    mov edi, pml4_table
    mov ecx, (4096 * 3) / 4
    xor eax, eax
    rep stosd

    ; PML4[0] -> PDP table, PDP[0] -> page directory.
    mov eax, pdp_table
    or eax, 0x003
    mov [pml4_table], eax
    mov eax, page_directory
    or eax, 0x003
    mov [pdp_table], eax

    ; Identity-map 1 GiB with 2 MiB pages.
    mov edi, page_directory
    mov eax, 0x00000083           ; present | writable | PS (2 MiB)
    mov ecx, 512
.map_2m:
    mov [edi], eax
    add eax, 0x00200000
    add edi, 8
    loop .map_2m

    ; Enable PAE, long mode, paging.
    mov eax, cr4
    or eax, 1 << 5                ; CR4.PAE
    mov cr4, eax
    mov eax, pml4_table
    mov cr3, eax
    mov ecx, 0xC0000080           ; IA32_EFER
    rdmsr
    or eax, 1 << 8                ; EFER.LME
    wrmsr
    mov eax, cr0
    or eax, 1 << 31                ; CR0.PG
    mov cr0, eax

    lgdt [gdt64_descriptor]
    jmp 0x08:long_mode_entry

BITS 64
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    xor ax, ax
    mov fs, ax
    mov gs, ax

    mov rsp, stack_top
    and rsp, -16
    xor ebp, ebp
    mov edi, [multiboot_info_ptr]
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt

SECTION .rodata
ALIGN 16
gdt64:
    dq 0x0000000000000000       ; null
    dq 0x00AF9A000000FFFF       ; 64-bit code
    dq 0x00AF92000000FFFF       ; data

gdt64_descriptor:
    dw gdt64_descriptor - gdt64 - 1
    dq gdt64

SECTION .bss
ALIGNB 8
multiboot_info_ptr: resd 1
ALIGNB 4096
pml4_table: resq 512
pdp_table: resq 512
page_directory: resq 512
ALIGNB 16
stack_bottom: resb 16384
stack_top:

SECTION .note.GNU-stack noalloc noexec nowrite progbits
