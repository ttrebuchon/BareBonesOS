[EXTERN __save_x86_registers]
[GLOBAL save_x86_registers]

save_x86_registers:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    push eax
    pushad
    call __save_x86_registers
    popad
    pop eax
    pop ebp
    ret