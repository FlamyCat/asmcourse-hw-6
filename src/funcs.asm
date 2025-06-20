section .data
    four: dq 4.0

section .text
global f1, f2, f3

f1:
    enter 0, 0
    
    finit
    fld qword [four]
    fld qword [ebp+8]
    fld qword [ebp+8]
    fmul
    
    fld1
    fadd

    fdiv

    fld1
    faddp
    
    leave
    ret

f2:
    enter 0, 0

    finit
    TIMES 3 fld qword [ebp+8]
    TIMES 2 fmulp

    leave
    ret

f3:
    enter 0, 0

    finit
    fld qword [ebp+8] ; st0 = x
    fchs              ; st0 = -x
    fld st0           ; Дублируем -x
    
    frndint           ; Округляем копию -x до ближайшего целого (-n)
    fxch              ; Теперь на верхушке стека лежит -x, -n лежит в st(0)
    fsub st0, st1     ; f := (-x) - (-n) = -f

    f2xm1             ; На верхушке стека лежит 2^(-f) - 1

    fld1              ; Исправим на 2^(-f)
    faddp
    
    fscale            ; Судя по документации, эта инструкция положит на стек 2^(-f)*2^(-n), что нам и нужно
    
    fstp

    leave
    ret