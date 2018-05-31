[EXTERN __save_x86_registers]
[GLOBAL save_x86_registers]

save_x86_registers:
    push ebp
    ; Offset = 4
    mov ebp, esp
    mov eax, [ebp + 8]
    push eax
    ; Offset = 8
    pushad
    ; Offset = 8 + ?
    call __save_x86_registers
    popad
    ; Offset = 8
    pop eax
    ; Offset = 4
    pop ebp
    ; Offset = 0
    ret


[GLOBAL save_x86_context]
[EXTERN __save_x86_context]

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



[GLOBAL restore_x86_context]
restore_x86_context:
    pop eax             ; Remove the return address from the stack
    popad               ; Restore the registers
    pop esp
    jmp edx


; [EXTERN __save_context_x86_forward]
; [GLOBAL save_context]
; save_context:
;     push ebp
;     ; Offset = 4
;     mov ebp, esp

;     mov eax, [ebp + 8]
;     mov edx, eax
;     add edx, 20
;     push edx
;     ; Offset = 8
;     push eax
;     ; Offset = 12
;     call save_x86_context
;     pop edx
;     ; Offset = 8
;     pop edx
;     ; Offset = 4

;     cmp eax, 0
;     jne save_context_restored_dwid

;     pop ebp
;     ; Offset = 0
;     ret



; save_context_restored_dwid:
;     hlt























[EXTERN __save_context_x86_forward]
[GLOBAL save_context]
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
