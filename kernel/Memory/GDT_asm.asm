[global flush_gdt]

flush_gdt:
    MOV EAX, [ESP+4]
    LGDT [EAX]

    MOV AX, 0x10
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX
    MOV SS, AX

    JMP 0x08:.flush

.flush:
    ret