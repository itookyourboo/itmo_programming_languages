section .text
 
 
; Принимает код возврата и завершает текущий процесс
exit: 
	mov rax, 60
	syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
	; очищаем регистр rax
	xor rax, rax
	.next:
		; rdi - указатель на строку
		; rax - индекс текущего символа
		; сравниваем символ с нулем
		cmp byte[rdi+rax], 0
		; если равен 0 (Jump if Equals)
		; завершаем процесс
		je .return
		; добавляем единицу к rax (переходим к след. символу)
		inc rax
		jmp .next
	.return:
		; в rax будет лежать кол-во символов
		ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
	xor rax, rax
	; переносим указатель на строку в rsi [указатель при выводе]
	mov rsi, rdi
	call string_length ; получаем кол-во символов в rax
	mov rdx, rax ; переносим их в rdx [кол-во символов при выводе]
	mov rdi, 1; [это просто код возврата]
	mov rax, 1; [команда write]
	syscall
	ret

; Переводит строку (выводит символ с кодом 0xA)
; Тут финт ушами - мы кладем символ, а дальше он пойдет в print_char
print_newline:
	mov rdi, 0xA

; Принимает код символа и выводит его в stdout
; Тут все аналогично с print_string
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

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
	mov rax, rdi
	;в rsp лежит указатель на число
	mov r9, rsp
	dec rsp
	mov byte [rsp], 0
	.loop:
		xor rdx, rdx	;rdx = 0
		mov rdi, 10
		div rdi		;rax /= 10
		mov rdi, rdx	
		add rdi, '0'	;добавляем код символа '0' к rdi ('0' + 2 = '2')
		dec rsp		;rsp--;
		mov byte [rsp], dil	;заполняем rsp символами (dil - это младший байт rdi)
		test rax, rax		;аналогично cmp rax, 0
		jne .loop		;Jump if Not Equals (повторяем, пока не дойдем до нуль-терминатора)
	mov rdi, rsp			;переносим rsp в rdi, чтобы напечатать
	call print_string
	mov rsp, r9
	ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
	test rdi, rdi
	jns .uint	;Jump if Not Sign (неотрицательное)
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
		je .return
		inc rcx
		jmp .loop
	.not_equals:
		mov rax, 0
	.return:
		ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
	xor rax, rax
	mov rdi, 0
	lea rsi, [rsp-1]	;загрузка адреса [rsp - 1] в rsi (куда читаем)
	mov rdx, 1		;режим чтения
	syscall
	test rax, rax
	jz .return		; Jump if Zero (если ноль, выходим)
	mov al, [rsp-1]		; al - младший полубайт rax (кладем туда символ)
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
			jng .loop
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
	jne parse_uint
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

