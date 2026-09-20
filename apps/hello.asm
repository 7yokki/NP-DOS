BITS 64

SECTION .text
GLOBAL _start
_start:
    ; NP-DOS stage-0 ABI sample: INT 10h AH=0Eh, AL=character.
    ; The loader maps this ELF; ring-3 transfer is a later milestone.
    mov rsi, message
.next:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .next
.done:
    hlt

SECTION .rodata
message db 'Hello from NP-ELF ASM', 13, 10, 0
