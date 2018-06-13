[global link_callback_handler]
[extern linker_callback]



link_callback_handler:
	; push ebp
	; mov eax, [esp + 4]
	; push eax
	; mov eax, [esp + 12]
	; push eax
	call linker_callback
	pop ebx
	pop ebx
	; pop ebp
	; pop ebx
	; pop ebx
	jmp eax