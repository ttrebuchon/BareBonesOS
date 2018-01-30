[global idt_flush]

idt_flush:
    MOV eax, [esp+4]
    LIDT [eax]
    RET

%macro ISR_NOERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        CLI
        PUSH byte 0
        PUSH byte %1
        JMP isr_common_stub
%endmacro


%macro ISR_ERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        CLI
        PUSH byte %1
        JMP isr_common_stub
%endmacro


ISR_NOERRCODE   0
ISR_NOERRCODE   1
ISR_NOERRCODE   2
ISR_NOERRCODE   3
ISR_NOERRCODE   4
ISR_NOERRCODE   5
ISR_NOERRCODE   6
ISR_NOERRCODE   7
ISR_ERRCODE     8
ISR_NOERRCODE   9
ISR_ERRCODE     10
ISR_ERRCODE     11
ISR_ERRCODE     12
ISR_ERRCODE     13
ISR_ERRCODE     14
ISR_NOERRCODE   15
ISR_NOERRCODE   16
ISR_NOERRCODE   17
ISR_NOERRCODE   18
ISR_NOERRCODE   19
ISR_NOERRCODE   20
ISR_NOERRCODE   21
ISR_NOERRCODE   22
ISR_NOERRCODE   23
ISR_NOERRCODE   24
ISR_NOERRCODE   25
ISR_NOERRCODE   26
ISR_NOERRCODE   27
ISR_NOERRCODE   28
ISR_NOERRCODE   29
ISR_NOERRCODE   30
ISR_NOERRCODE   31


[EXTERN isr_handler]

isr_common_stub:
    PUSHA

    MOV ax, ds
    PUSH eax

    MOV ax, 0x10
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    CALL isr_handler
    

    POP ebx
    MOV ds, bx
    MOV es, bx
    MOV fs, bx
    MOV gs, bx


    POPA
    ADD esp, 8
    STI
    IRET
    