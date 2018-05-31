[EXTERN __save_x86_registers]
[EXTERN __save_x86_context]
[GLOBAL save_x86_context]
[GLOBAL restore_x86_context]
[GLOBAL save_context]

save_x86_context:
    push ebp
    ; Offset = 4
    mov ebp, esp

    mov eax, [ebp + 12]     ; Move the registers_t* into EAX
    push eax                ; Push it as a parameter
    ; Offset = 8
    mov eax, 1              ; Store a 1 in EAX register
    pushad                  ; Push all registers
    ; Offset = 8 + ?
    call __save_x86_registers ; Save all the registers
    popad                   ; Pop all the registers
    ; Offset = 8
    pop eax                 ; Move the context_t* off the stack
    ; Offset = 4


    mov eax, [ebp + 4]      ; Push caller EIP
    push eax
    ; Offset = 8

    mov eax, [ebp]          ; Push caller EBP
    ; mov edx, [ebp]
    ; mov eax, [edx]
    push eax
    ; Offset = 12

    mov eax, ebp
    add eax, 8             ; Calculate the old ESP (Before it hit 'call')
    push eax                ; Push the old ESP
    ; Offset = 16

    mov eax, [ebp + 8]      ; Push the context_t pointer
    push eax
    ; Offset = 20

    call __save_x86_context ; __save_x86_context(context_t*, ESP, EBP, EIP)

    pop eax                 ; Pop context_t
    ; Offset = 16
    pop eax                 ; Pop ESP
    ; Offset = 12
    pop eax                 ; Pop EBP
    ; Offset = 8
    pop eax                 ; Pop EIP
    ; Offset = 4

    mov eax, 0              ; Put a 0 into eax as the return value

    pop ebp
    ; Offset = 0
    ret




restore_x86_context:
    pop eax             ; Remove the return address from the stack
    popad               ; Restore the registers
    pop esp
    jmp edx






save_context:
    push ebp
    ; Offset = 4
    mov ebp, esp

    mov edx, [ebp + 8]      ; EDX = context_t*
    mov eax, edx            ; EAX = EDX
    add eax, 20             ; EAX = registers_t*
    push eax                ; Push it as a parameter
    ; Offset = 8
    mov eax, 1              ; Store a 1 in EAX register
    pushad                  ; Push all registers
    ; Offset = 8 + ?
    call __save_x86_registers ; Save all the registers
    popad                   ; Pop all the registers
    ; Offset = 8
    pop eax                 ; Move the context_t* off the stack
    ; Offset = 4

    mov eax, [ebp + 4]      ; Push caller EIP
    push eax
    ; Offset = 8

    mov eax, [ebp]          ; Push caller EBP
    push eax
    ; Offset = 12



    mov eax, ebp
    add eax, 8              ; Calculate the old ESP (Before it hit 'call')
    push eax                ; Push the old ESP
    ; Offset = 16

    mov eax, [ebp + 8]      ; Push the context_t pointer
    push eax
    ; Offset = 20

    
    call __save_x86_context ; __save_x86_context(context_t*, ESP, EBP, EIP)

    pop eax                 ; Pop context_t
    ; Offset = 16
    pop eax                 ; Pop ESP
    ; Offset = 12
    pop eax                 ; Pop EBP
    ; Offset = 8
    pop eax                 ; Pop EIP
    ; Offset = 4

    mov eax, 0              ; Put a 0 into eax as the return value

    pop ebp
    ; Offset = 0
    ret
