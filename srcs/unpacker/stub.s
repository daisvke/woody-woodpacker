bits 64

section .text
global _start

_start:
	lea	r8, [rel _start]              			; Get _start address
	mov	r9, r8                     				; Copy that to r9
	sub	r9, [r8 + main_entry_offset_from_stub]  ; Put offset of main entry into r9
	jmp ender

print_woody:
	mov eax, 1      ; system call number for sys_write
	mov edi, 1      ; file descriptor (stdout)
	pop rsi         ; pop the address of the string from the stack
	mov rdx, 14     ; length of the string
	syscall         ; call the kernel

	mov		r10, text_length	; Assign data_length to r10
    mov		rsi, r9	            ; Assign address of data to rsi

    ; Load the address of the key string into rbx
    lea     rbx, [rel key]
	mov r11, rbx

xor_loop:
	; This code segment is a loop that iterates over the bytes
	; of the data, performs an XOR operation between the corresponding
	; bytes of rsi and rbx, and replaces the original byte with the XORed
	; value into rsi.
	movzx	eax, byte [r11] 
	xor		al, byte [rsi]
	mov		byte [rsi], al
	
	inc		r11		; Go to next key char
	inc		rsi		; Go to next data char
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

clean_return:
	xor eax, eax    ; Reset registers
	xor rdi, rdi
	xor rsi, rsi
	xor rbx, rbx
	xor rdx, rdx    ; Segfaults without this
	xor r8, r8
	xor r11, r11
	xor r12, r12
	xor r13, r13
	push r9         ; Push the main entry address to the stack
	ret             ; On return, we will jump to the pushed address

; Print the processed data
print_data:
    mov		rax, 1
    mov		rdi, 1
    mov		rsi, r9
	mov		rdx, text_length
    syscall
	ret
	

ender:
	call print_woody
	woody_msg db "....WOODY....", 0xa ; newline-terminated string
    key db "abcdefghijklmnopqr", 0x0	; Assign address of key to rbx

	; Define the variables as placeholders
	; The values will be patched from the C file
	main_entry_offset_from_stub dq 0x000000000000016d
	key_length dq 0x0000000000000012
	text_length dq 0x0000000000000161
	; text_segment_offset dq 0x000000000000016d
