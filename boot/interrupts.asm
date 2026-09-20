BITS 64

extern service_int10
extern service_int13
extern service_int14
extern service_int16
extern service_int17

global isr_default, isr10, isr13, isr14, isr16, isr17

isr_default:
    iretq

; INT 10h is a character output service and preserves AX.
%macro service_stub 2
%1:
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    sub rsp, 8
    call %2
    add rsp, 8
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    iretq
%endmacro

; INT 13h, 14h, 16h, and 17h return a small status/character in RAX.
%macro service_stub_return_rax 2
%1:
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8
    call %2
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    iretq
%endmacro

service_stub isr10, service_int10
service_stub_return_rax isr13, service_int13
service_stub_return_rax isr14, service_int14
service_stub_return_rax isr16, service_int16
service_stub_return_rax isr17, service_int17

SECTION .note.GNU-stack noalloc noexec nowrite progbits
