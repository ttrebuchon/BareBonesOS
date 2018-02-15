[GLOBAL copy_page_physical]
copy_page_physical:
    PUSH EBX

    PUSHF

    CLI

    MOV EBX, [ESP+12]
    MOV ECX, [ESP+16]

    MOV EDX, CR0
    AND EDX, 0x7FFFFFFF
    MOV CR0, EDX

    MOV EDX, 1024

    .loop:
        MOV EAX, [EBX]
        MOV [ECX], EAX
        ADD EBX, 4
        ADD ECX, 4
        DEC EDX
        
        JNZ .loop

    MOV EDX, CR0
    OR EDX, 0x80000000
    MOV CR0, EDX

    POPF
    POP EBX

    RET
