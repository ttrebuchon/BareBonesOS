%macro IRQ 2
	[GLOBAL irq%1]
	irq%1:
		CLI
		PUSH BYTE 0
		PUSH BYTE %2
		JMP irq_common_stub
%endmacro


IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47



[EXTERN irq_handler]

irq_common_stub:
	PUSHA
	
	MOV AX, DS
	PUSH EAX
	
	MOV AX, 0x10
	MOV DS, AX
	MOV ES, AX
	MOV FS, AX
	MOV GS, AX
	
	CALL irq_handler
	
	
	POP EBX
	MOV DS, BX
	MOV ES, BX
	MOV FS, BX
	MOV GS, BX
	
	POPA
	
	ADD ESP, 8
	STI
	IRET