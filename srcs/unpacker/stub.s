bits 64

section .text
global _start

_start:
	lea	r8, [rel _start]              				; Get _start address

	mov	r9, r8                     					; Copy that to r9
	sub	r9, [r8 + _main_entry_offset_from_stub] 	; Compute main entry address into r9

	mov	r13, [r8 + _text_length]					; Put .text section length in r13
	mov	r14, r8                     				; Copy _start address into r14
	sub	r14, [r8 + _text_segment_offset_from_stub]	; Compute segment address into r14

	jmp	_get_data
_print_woody:
	mov		eax, 0x1    ; system call number for sys_write
	mov		edi, 0x1    ; file descriptor (stdout)
	pop		rsi			; pop the address of the string from the stack
	mov		edx, 0xe	; length of the string
	syscall         	; call the kernel

_modify_data_flags:
	; Use mprotect to change flags of .text memory region
	; so that we can write the decrpyted data back into r9
	mov		eax, 0xa	; sys_mprotect
	mov		rdi, r14	; .text segment address
	mov		rsi, 0x1000	; .text segment size
	mov		edx, 0x7	; PROT_READ|PROT_WRITE|PROT_EXEC
	syscall

_prepare_decrpytion:
	mov		r10, r13		; Assign data_length to r10
    mov		r12, r9			; Assign address of data to r12
	lea     rbx, [rel _key]	; Assign address of the key string to rbx
	mov		r11, rbx		; Copy that into r11

_xor_loop:
	; This code segment is a loop that iterates over the bytes
	; of the data, performs an XOR operation between the corresponding
	; bytes of r9 and r11, and replaces the original byte with the XORed
	; value back into r9 (the original data location).
	mov		al, byte [r11]	; Copy the current byte into al
	xor		byte [r9], al	; xor that with the current key byte
	
	inc		r11		; Go to next key char
	inc		r9		; Go to next data char
	dec		r10		; Decrement decrypting data size

	; Check if the end of the key is reached
	cmp		byte [r11], 0x0
	jne		_continue_loop
	; Reset the key pointer to the beginning
	mov		r11, rbx

; The loop continues until all bytes of the data have been processed.
_continue_loop:
	cmp		r10, 0x0	; Check if the end of the data is reached
	jne		_xor_loop	; If not, continue loop
	; If reached, print the result (only for testing)
	; call	_print_data

; Reset all used registers, just in case
_clean_return:
	xor		rax, rax
	xor		rdi, rdi
	xor		rsi, rsi
	xor		rbx, rbx
	xor		rdx, rdx    ; Segfaults without this
	xor		r8, r8
	xor		r10, r10
	xor		r11, r11
	xor		r13, r13
	xor		r14, r14
	push	r12			; Push the main entry address to the stack
	xor		r12, r12
	ret         	    ; On return, we will jump to the pushed address

; Print the processed data (only for testing)
_print_data:
    mov		rax, 1
    mov		rdi, 1
    mov		rsi, r12
	mov		rdx, r13
    syscall
	ret

_get_data:
	call _print_woody

	db "....WOODY....", 0xa	; newline-terminated string
	; Define the variables as placeholders
	; The values will be patched from stub_injection.c
	_main_entry_offset_from_stub		dq 0x0000000000000000
	_text_segment_offset_from_stub		dq 0x0000000000000000
	_text_length						dq 0x0000000000000000
	; Here we put a random string that will be replaced with the actual key.
	; This is to reserve the space on the stub file for the key to come.
    _key 								db "01234567891011121314151617181920", 0x0