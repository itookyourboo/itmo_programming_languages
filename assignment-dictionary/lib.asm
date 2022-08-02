section .text

global string_length
global print_string
global print_error
global print_char
global print_newline
global print_uint
global print_int
global string_equals
global read_char
global read_word
global parse_uint
global parse_int
global string_copy
global exit


; Принимает код возврата и завершает текущий процесс
exit: 
	mov rax, 60
	syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
	xor rax, rax
	.next:
		cmp byte[rdi+rax], 0
		je .return
		inc rax
		jmp .next
	.return:
		ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
	xor rax, rax
	mov rsi, rdi
	call string_length
	mov rdx, rax
	mov rdi, 1
	mov rax, 1
	syscall
	ret

print_error:
	xor rax, rax
	mov rsi, rdi
	call string_length
	mov rdx, rax
	mov rdi, 2
	mov rax, 1
	syscall
	ret

; Принимает код символа и выводит его в stdout
print_char:
	xor rax, rax
	push rdi
	mov rsi, rsp
	pop rdi
	mov rax, 1
	mov rdi, 1
	mov rdx, 1
	syscall
	ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
	mov rdi, 0xA
	jmp print_char

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
	mov rax, rdi
	mov r9, rsp
	dec rsp
	mov byte [rsp], 0
	.loop:
		xor rdx, rdx
		mov rdi, 10
		div rdi
		mov rdi, rdx
		add rdi, '0'
		dec rsp
		mov byte [rsp], dil
		test rax, rax
		jne .loop
	mov rdi, rsp
	call print_string
	mov rsp, r9
	ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
	test rdi, rdi
	jns .uint
	push rdi
	mov rdi, '-'
	call print_char
	pop rdi
	neg rdi
	.uint:
		jmp print_uint

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
	mov rax, 1
	mov r8, rdi
	mov r9, rsi
	xor rcx, rcx
	xor rdi, rdi
	xor rsi, rsi
	.loop:
		mov dil, [r9+rcx]
		mov sil, [r8+rcx]
		cmp sil, dil
		jne .not_equals
		cmp byte dil, 0
		je .equals
		inc rcx
		jmp .loop
	.not_equals:
		mov rax, 0
		ret
	.equals:
		ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
	xor rax, rax
	mov rdi, 0
	lea rsi, [rsp-1]
	mov rdx, 1
	syscall
	test rax, rax
	jz .return
	mov al, [rsp-1]
	ret
	.return:
		xor rax, rax
		ret

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

is_not_digit:
	push rdi
	push rsi
	call read_char
	pop rsi
	pop rdi
	cmp al, 0x20
	je .not_digit
	cmp al, 0x9
	je .not_digit
	cmp al, 0xA
	je .not_digit
	xor rdx, rdx
	ret
	.not_digit:
		mov rax, 0
		mov rdx, 1
		ret

read_word:
	xor rax, rax
	xor r9, r9
	.nd_loop:
		call is_not_digit
		cmp rdx, 1
		je .nd_loop
		jmp .write
	.loop:
		call is_not_digit
		.write:
			mov byte [rdi+r9], al
			cmp rax, 0
			je .return
			inc r9
			cmp r9, rsi
			jg .ret_greater
			jmp .loop
	.ret_greater:
		xor rax, rax
		ret
	.return:
		mov rax, rdi
		mov rdx, r9
		ret


is_digit:
	xor rax, rax
	sub dil, 0x30
	cmp dil, 0
	jl .not_digit
	cmp dil, 9
	jg .not_digit
	mov al, dil
	ret
	.not_digit:
		mov rax, 10
		ret
 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
	xor rax, rax
	xor rcx, rcx
	mov r8, 10
	mov r10, rdi
	mov rdi, 0
	.loop:
		mov byte dil, [r10+rcx]
		push rax
		call is_digit
		cmp rax, 10
		je .return
		mov rdi, rax
		pop rax
		mul r8
		add rax, rdi
		inc rcx
		jmp .loop
	.return:
		pop rax
		mov rdx, rcx
		ret


; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
	xor rax, rax
	cmp byte [rdi], '-'
	je .neg
	jmp parse_uint
	.neg:
		inc rdi
		call parse_uint
		neg rax
		inc rdx
		ret

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
	call string_length
	cmp rax, rdx
	jg .if_greater
	mov rcx, 0
	.loop:
		mov r8b, [rdi+rcx]
		mov [rsi+rcx], r8b
		cmp rcx, rax
		je .return
		inc rcx
		jmp .loop
	.if_greater:
		xor rax, rax
	.return:
		ret

