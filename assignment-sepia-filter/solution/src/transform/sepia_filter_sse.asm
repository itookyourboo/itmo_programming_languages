global sepia_filter_sse

section .rodata

align 16
c1: dd 0.272, 0.349, 0.393, 0.272
align 16
c2: dd 0.543, 0.686, 0.769, 0.543
align 16
c3: dd 0.131, 0.168, 0.189, 0.131

align 16
filter: dd 255.0, 255.0, 255.0, 255.0

%macro calc_four_values 1
    movq xmm0, [rdi]
    movq xmm1, [rsi]
    movq xmm2, [rdx]
    shufps xmm0, xmm0, %1
    shufps xmm1, xmm1, %1
    shufps xmm2, xmm2, %1
    ; r *= c1
    mulps xmm0, xmm3
    ; g *= c2
    mulps xmm1, xmm4
    ; b *= c3
    mulps xmm2, xmm5
    ; r += g
    addps xmm0, xmm1
    ; r += b
    addps xmm0, xmm2

    pminsd xmm0, xmm6
    cvtps2dq xmm0, xmm0
    pextrb [rcx], xmm0, 0
    pextrb [rcx+1], xmm0, 4
    pextrb [rcx+2], xmm0, 8
    pextrb [rcx+3], xmm0, 12
%endmacro


%macro next 0
    add rdi, 4
    add rsi, 4
    add rdx, 4
    add rcx, 4
    shufps xmm3, xmm3, 0b_01_00_10_01
    shufps xmm4, xmm4, 0b_01_00_10_01
    shufps xmm5, xmm5, 0b_01_00_10_01
%endmacro


section .text
sepia_filter_sse:
    movdqa xmm6, [rel filter]

    movdqa xmm3, [rel c1]
    movdqa xmm4, [rel c2]
    movdqa xmm5, [rel c3]
    ; 0b_01_00_00_00:
    ; 0000x2x1 -> x1x1x1x2
    calc_four_values 0b_01_00_00_00
    ; 0b_01_01_00_00:
    ; 0000x3x2 -> x2x2x2x3
    next
    calc_four_values 0b_01_01_00_00
    ; 0b_01_01_01_00:
    ; 0000x4x3 -> x3x3x3x4
    next
    calc_four_values 0b_01_01_01_00

    ret
