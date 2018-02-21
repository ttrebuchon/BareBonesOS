;%pragma O 0
[EXTERN sleep_for]

[GLOBAL __sleep]
__sleep:
    PUSH EAX
__sleep_loop:
    STI
    MOV EAX, sleep_for
    OR EAX, EAX
    JZ __sleep_done
    
    NOP
    NOP
    NOP
    NOP    
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP    
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP    
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP    
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    JMP __sleep_loop
__sleep_done:
    STI
    POP EAX
    RET