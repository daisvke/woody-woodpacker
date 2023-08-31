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

    mov		rsi, r9	; Assign address of data to rsi
    lea 	rbx, [rel key]	; Assign address of key to rbx

xor_loop:
	; This code segment is a loop that iterates over the bytes
	; of the data, performs an XOR operation between the corresponding
	; bytes of rsi and rbx, and replaces the original byte with the XORed
	; value into rsi.
	movzx	rax, byte [rbx]
	xor		al, byte [rsi]
	mov		byte [rsi], al
	
	inc		rbx		; Go to next key char
	inc		rsi		; Go to next data char
	dec		r8

	; Check if the end of the key is reached
	cmp		byte [rbx], 0
	jne		continue_loop
	; Reset the key pointer to the beginning
	mov		rbx, rdi

; The loop continues until all bytes of the data have been processed.
continue_loop:
	; Check if the end of the data is reached
	cmp		r8, 0
	; If not, continue loop
	jne		xor_loop
	; If reached, print the result
	call	print_data
	; And return from the function after putting back the caller's
	; original values in the registers
	leave	; mov rsp, rbp ; pop rbp
	ret

clean_return:
	xor eax, eax    ; Reset registers
	xor edi, edi
	xor rsi, rsi
	xor rdx, rdx    ; Segfaults without this
	push r9         ; Push the main entry address to the stack
	ret             ; On return, we will jump to the pushed address

; Print the processed data
print_data:
    mov		rax, 1
    mov		rdi, 1
    mov		rsi, rdx
	mov		rdx, rcx
    syscall
	ret

ender:
	call print_woody
	db '....WOODY....', 10 ; newline-terminated string
	key db "abcdefghijklmnopqr"
	main_entry_offset_from_stub dq 0x000000000000016d
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
