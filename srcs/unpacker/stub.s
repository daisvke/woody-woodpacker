bits 64

section .text
global _start

_start:
	lea	r8, [rel _start]              				; Get _start address

	mov	r9, r8                     					; Copy that to r9
	sub	r9, [r8 + main_entry_offset_from_stub] 		; Compute main entry address into r9

	mov	r13, [r8 + text_length]						; Put text length in r13
	mov	r14, r8                     				; Copy _start address into r14
	sub	r14, [r8 + text_segment_offset_from_stub]	; Compute segment address into r14

	jmp	get_data

print_woody:
	mov		eax, 0x1    ; system call number for sys_write
	mov		edi, 0x1    ; file descriptor (stdout)
	pop		rsi			; pop the address of the string from the stack
	mov		edx, 0xe	; length of the string
	syscall         ; call the kernel

prepare_decrpytion:
	mov		r10, r13	; Assign data_length to r10
    mov		r12, r9		; Assign address of data to r12
	; Use mprotect to change flags of .text memory region
	; so that we can write the decrpyted data back into r9
	mov		eax, 0xa	; sys_mprotect
	mov		rdi, r14	; .text address
	mov		rsi, 0x1000	; .text segment size
	mov		edx, 0x7	; PROT_READ|PROT_WRITE|PROT_EXEC
	syscall

    lea     rbx, [rel key]	; Load the address of the key string into rbx
	mov		r11, rbx		; Copy that into r11

xor_loop:
	; This code segment is a loop that iterates over the bytes
	; of the data, performs an XOR operation between the corresponding
	; bytes of r9 and r11, and replaces the original byte with the XORed
	; value into r9.
	movzx	eax, byte [r11]
	xor		byte [r9], al
	
	inc		r11		; Go to next key char
	inc		r9		; Go to next data char
	dec		r10

	; Check if the end of the key is reached
	cmp		byte [r11], 0x0
	jne		continue_loop
	; Reset the key pointer to the beginning
	mov		r11, rbx

; The loop continues until all bytes of the data have been processed.
continue_loop:
	; Check if the end of the data is reached
	cmp		r10, 0x0
	; If not, continue loop
	jne		xor_loop
	; If reached, print the result
	call	print_data

; Reset registers
clean_return:
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

; Print the processed data
print_data:
    mov		rax, 1
    mov		rdi, 1
    mov		rsi, r12
	mov		rdx, r13
    syscall
	ret

get_data:
	call print_woody
	woody_msg	db "....WOODY....", 0xa 	 ; newline-terminated string
    key 		db "abcdefghijklmnopqr", 0x0 ; Assign address of key to rbx

	; Define the variables as placeholders
	; The values will be patched from the C file
	main_entry_offset_from_stub		dq 0x000000000000016d
	text_segment_offset_from_stub	dq 0x00000000000001ad
	text_length						dq 0x0000000000000161
