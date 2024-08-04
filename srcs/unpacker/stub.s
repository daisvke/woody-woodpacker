bits 64

section .text
global _start

_start:
	lea		r8, [rel _start]              				; Get _start address
	mov		r9, r8                     					; Copy that to r9
	sub		r9, [r8 + main_entry_offset_from_stub] 		; Compute main entry address into r9

modify_data_flags:
	; Use mprotect to change flags of .text memory region
	; so that we can write the decrpyted data back into r9
	mov		eax, 0xa									; sys_mprotect
	mov		rdi, r8
	sub		rdi, [r8 + text_segment_offset_from_stub]	; Compute segment address
	mov		rsi, [r8 + text_segment_offset_from_stub]	; segment size
	mov		edx, 0x7									; PROT_READ|PROT_WRITE|PROT_EXEC
	syscall

ft_strlen:
	lea		rdi, [rel woody_msg]; Pop the address of the string from the stack
	mov		rsi, rdi			; Save the string (used by print_woody)
	xor		rax, rax			; Init rax to 0 by XORing bits
	jmp		count_loop			; Jump to loop subroutine

count_loop:
	cmp		byte [rdi + rax], 0xa	; Compare the current string position to 0xa (new line)
	jz		print_woody				; If = 0xa, end of string => jump to return
	inc		rax						; Increment rax by one
	jmp		count_loop				; Continue loop

print_woody:
	mov		edx, eax	; Put the result from ft_strlen in edx (used by sys_write)
	add		rdx, 0x1	; Add 1 to the length for the new line
	mov		eax, 0x1    ; System call number for sys_write
	mov		edi, 0x1    ; File descriptor (stdout)
	syscall         	; Call the kernel

prepare_decrpytion:
	mov		r14, r8                     				; Copy _start address into r14
	sub		r14, [r8 + text_section_offset_from_stub]	; Compute section address into r14

	mov		r10, [r8 + text_length]	; Assign .text section length to r10
    mov		r12, r9					; Keep address of data in r12 for jumping to it while exiting
	lea     rbx, [rel key]			; Assign address of the key string to rbx
	mov		r11, rbx				; Copy that into r11

xor_loop:
	; This code segment is a loop that iterates over the bytes
	; of the data, performs an XOR operation between the corresponding
	; bytes of r9 and r11, and replaces the original byte with the XORed
	; value back into r9 (the original data location).
	mov		al, byte [r11]	; Copy the current byte into al
	mov		cl, 0x2a		; Load the value 42 into the lower 8 bits of rcx
	xor		byte [r14], al	; xor the current byte from the data with the current key byte
	sub		byte [r14], cl	; substract 42 from it, as we had added 42 during encryption
	inc		r11				; Go to next key char
	inc		r14				; Go to next data char
	dec		r10				; Decrement decrypting data size

	; Check if the end of the key is reached
	cmp		byte [r11], 0x0
	jne		continue_loop
	; Reset the key pointer to the beginning
	mov		r11, rbx

; The loop continues until all bytes of the data have been processed.
continue_loop:
	cmp		r10, 0x0		; Check if the end of the data is reached
	jne		xor_loop		; If not, continue loop
	; If reached, print the result (only for testing)
	; call	_print_data

; Reset all used registers, just in case
clean_return:
	xor		rax, rax
	xor		rcx, rcx
	xor		rdi, rdi
	xor		rsi, rsi
	xor		rbx, rbx
	xor		rdx, rdx    ; Segfaults without this
	xor		r8,  r8
	xor		r9,  r9
	xor		r10, r10
	xor		r11, r11
	xor		r13, r13
	xor		r14, r14
	push	r12			; Push the main entry address to the stack
	ret         	    ; On return, we will jump to the pushed address

; Print the processed data (only for testing)
print_data:
    mov		rax, 0x1
    mov		rdi, 0x1
    mov		rsi, r9
	mov		rdx, r13
    syscall
	ret

; New-line-terminated string to print
woody_msg						db "....WOODY....", 0xa	
; Define the variables as placeholders
; The values will be patched from stub_injection.c
main_entry_offset_from_stub		dq 0x0000000000000000
text_segment_offset_from_stub	dq 0x0000000000000000
text_section_offset_from_stub	dq 0x0000000000000000
text_length						dq 0x0000000000000000
; Here we put a random string that will be replaced with the actual key.
; This is to reserve the space on the stub file for the key to come.
key 							db "01234567891011121314151617181920", 0x0
