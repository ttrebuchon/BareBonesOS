
#ifdef TESTING
.GLOBAL read_eip
.global pop_eip

read_eip:
	//LDR x0, [sp, 4]
	//mov lr, x0
	mov x0, lr
	ret


pop_eip:
	mov lr, x0
	mov x0, 0
	br lr
	
#endif