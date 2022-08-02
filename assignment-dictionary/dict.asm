%define WORD_SIZE 8

global find_word
extern string_equals

section .text
find_word:
	xor rax, rax
	mov r8, rdi
	mov r9, rsi
	.next:
		add r9, WORD_SIZE
		mov rsi, r9
		mov rdi, r8
		push r8
		push r9
		call string_equals
		pop r9
		pop r8
		cmp rax, 1
		je .found
		mov r9, [r9 - WORD_SIZE]
		cmp r9, 0
		je .return
		jmp .next
	.found:
		sub r9, WORD_SIZE
		mov rax, r9
		ret
	.return:
		xor rax, rax
		ret

