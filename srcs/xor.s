bits 64

;----------------------------------------------------------------
; void	xor_with_additive_cipher(
;	void *key, size_t key_length, void *data, size_t data_length)
;		<rdi>			<rsi>		<rdx>			<rcx>
;----------------------------------------------------------------
; In cryptography, the simple XOR cipher is a type of additive
; cipher, an encryption algorithm that operates, in binary,
; according to the principles:
;
;   0 1 1
;   0 1 0
;=> 0 0 1
;
; We added an additive cipher:
; - encyption:
; 	1. <byte> + <additive cipher value>
;	2. XOR <data-byte> <key-byte>
;
; - decryption:
;	1. XOR <data-byte> <key-byte>
; 	2. <byte> - <additive cipher value>

;---------------------------------- Macros
SYS_WRITE	equ 1
STDOUT		equ 1
;----------------------------------

section .text
    global	xor_with_additive_cipher

xor_with_additive_cipher:
	; Prologue: save caller's stack pointers
    push	rbp
    mov		rbp, rsp
	
    lea		rsi, [rdx]	; Assign address of data to rsi
    mov 	rbx, rdi	; Assign key to rbx

	; We need to keep the size of the data as we are using the original value
	; in print_data, and we are also decrementing it inside the loop to be
	; able to stop according to the data size
	; Doing cmp byte [rsi], 0 to check the end of the string instead of
	; using a counter would terminate the process earlier if a null character
	; is part of the data
	mov		r8, rcx	; Assign data_length to r10

xor_loop:
	; This code segment is a loop that iterates over the bytes
	; of the data, adds the value 42 to the current byte of the data
	; (this is to complexify the encryption algorithm, and the value will be
	; substracted during decryption),
	; then performs an XOR operation between the corresponding
	; bytes of rsi and rbx.
	mov		al, byte [rbx]	; Assign the current byte from the data to al
	mov		cl, 42			; Load the value 42 into the low 8 bits of rbx
	add		byte [rsi], cl	; Add 42 to the byte
	xor		byte [rsi], al	; XOR the data-byte with the key-byte
	
	inc		rbx		; Go to next key char
	inc		rsi		; Go to next data char
	dec		r8		; decrease the data_length counter

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
	; call	print_data
	; And return from the function after putting back the caller's
	; original values in the registers
	leave	; mov rsp, rbp ; pop rbp
	ret

; Print the processed data
print_data:
    mov		rax, SYS_WRITE
    mov		rdi, STDOUT
    mov		rsi, rdx
	mov		rdx, rcx
    syscall
	ret