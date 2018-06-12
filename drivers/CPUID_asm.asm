[global cpuid_supported]

cpuid_supported:
	push ebp
	mov ebp, esp
	pushfd
	pushfd
	xor [esp], 0x00200000
	popfd
	pushfd
	pop eax
	xor [esp], eax
	popfd
	and eax, 0x00200000
	pop ebp
	ret