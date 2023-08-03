bits 64
; We will use the 64-bit registers as 32-bit instructions
; have the same size.
global _start

_start:
    ; Print using write syscall
    mov		rax, 1          ; Write syscall code
    mov		rdi, 1          ; stdout code
    lea     rsi, [rel w]    ; load relative address of 'w' into rsi
	mov		rdx, 13         ; String size of 'w'
    syscall                 ; Run write(rdi, rsi, rdx)

    mov     r10, 0x0000000008001051
    jmp     r10

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
w db "....WOODY....", 10