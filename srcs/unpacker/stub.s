bits 64

section .text
global _start

_start:
	lea	r8, [rel _start]              			; Get _start address
	mov	r9, r8                     				; Copy that to r9
	sub	r9, [r8 + main_entry_offset_from_stub]  ; Put offset of main entry into r9
	jmp	short ender

print_woody:
	mov eax, 1      ; system call number for sys_write
	mov edi, 1      ; file descriptor (stdout)
	pop rsi         ; pop the address of the string from the stack
	mov rdx, 14     ; length of the string
	syscall         ; call the kernel
	xor rsi, rsi	; key index
	xor rcx, rcx	; .text index

; uncrpyt_loop:
; 	cmp 
; uncrypt:

clean_return:
	xor eax, eax    ; Reset registers
	xor edi, edi
	xor rsi, rsi
	xor rdx, rdx    ; Segfaults without this
	push r9         ; Push the main entry address to the stack
	ret             ; On return, we will jump to the pushed address

ender:
	call print_woody
	db '....WOODY....', 10 ; newline-terminated string
	main_entry_offset_from_stub dq 0x000000000000016d
	key db "abcdefghijklmnopqr"
	key_length dq 0x0000000000000012
	text_length dq 0x0000000000000161
	; text_segment_offset dq 0x000000000000016d

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