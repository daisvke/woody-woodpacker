[bits 64]
[SECTION .text]
global _start

_start:
    jmp short ender

starter:
    xor eax, eax    ; zero out eax
    xor edi, edi    ; zero out edi (we'll use edi as the file descriptor)
    xor rsi, rsi    ; zero out rsi (we'll use rsi as the pointer to the string)
    xor rdx, rdx    ; zero out rdx (we'll use rdx as the string length)

    mov eax, 1      ; system call number for sys_write
    mov edi, 1      ; file descriptor (stdout)
    pop rsi         ; pop the address of the string from the stack
    mov rdx, 14     ; length of the string
    syscall         ; call the kernel

    xor eax, eax    ; zero out eax
    jmp 0x0000000000801050
    mov al, 60      ; system call number for sys_exit
    xor edi, edi    ; exit code 0
    syscall         ; call the kernel

ender:
    call starter    ; put the address of the string on the stack
    db '....WOODY....', 10 ; newline-terminated string


    ; ; Exit
    ; mov		rax, 60         ; Exit syscall code
    ; mov		rbx, 0          ; Exit status code
    ; syscall

; String header that would be printed in stdout during execution
; of the output file.
;
; This header is an indication that the file has
; been encrypted by this program.
;
; Here we have an embed string in the .text (and not .data/.bss).
; It is stored in the same region of memory as the .text section.
; In this way we avoid having to use other sections of the original
; file (that would most likely be encrypted anyway).