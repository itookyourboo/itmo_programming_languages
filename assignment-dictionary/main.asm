%include "words.inc"
%include "lib.inc"

%define BUFF_SIZE 256
%define WORD_SIZE 8

extern find_word

global _start

section .data
buffer:
	times BUFF_SIZE db 0
no_key:
	db "Key not found", 0
buff_err:
	db "Buffer overflow", 0

section .text

_start:
	xor rax, rax
	mov rdi, buffer
	mov rsi, BUFF_SIZE
	call read_word
	cmp rax, 0
	jne .check_word
	mov rdi, buff_err
	call print_error
	call print_newline
	call exit
.check_word:
	mov rdi, rax
	mov rsi, first
	push rdx
	call find_word
	cmp rax, 0
	jne .get_word
	mov rdi, no_key
	call print_error
	call print_newline
	call exit
.get_word:
	pop rdx
	add rax, WORD_SIZE
	add rax, rdx
	add rax, 1
	mov rdi, rax
	call print_string
	call print_newline
	call exit
