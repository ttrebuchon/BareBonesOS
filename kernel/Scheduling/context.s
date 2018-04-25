	.arch armv8-a
	.file	"context.cpp"
	.text
	.align	2
	.global	load_context
	.type	load_context, %function
load_context:
.LFB428:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	add	x29, sp, 0
	.cfi_def_cfa_register 29
	str	x0, [x29, 24]
	mov	x1, 0
	ldr	x0, [x29, 24]
	bl	load_destroy_context
	.cfi_endproc
.LFE428:
	.size	load_context, .-load_context
	.section	.rodata
	.align	3
.LC0:
	.string	"false"
	.align	3
.LC1:
	.string	"../kernel/Scheduling/context.cpp"
	.text
	.align	2
	.global	load_destroy_context
	.type	load_destroy_context, %function
load_destroy_context:
.LFB429:
	.cfi_startproc
	stp	x29, x30, [sp, -64]!
	.cfi_def_cfa_offset 64
	.cfi_offset 29, -64
	.cfi_offset 30, -56
	add	x29, sp, 0
	.cfi_def_cfa_register 29
	str	x0, [x29, 24]
	str	x1, [x29, 16]
	ldr	x0, [x29, 24]
	ldr	x0, [x0, 16]
	str	x0, [x29, 56]
	ldr	x0, [x29, 24]
	ldr	x0, [x0, 8]
	str	x0, [x29, 48]
	ldr	x0, [x29, 24]
	ldr	x0, [x0, 24]
	str	x0, [x29, 40]
	ldr	x0, [x29, 16]
	cmp	x0, 0
	beq	.L3
	ldr	x1, [x29, 16]
	ldr	x0, [x29, 24]
	blr	x1
.L3:
	ldr	x0, [x29, 48]
	ldr	x1, [x29, 56]
	ldr	x2, [x29, 40]
#APP
// 46 "../kernel/Scheduling/context.cpp" 1
			mov SP, x0; 		mov FP, x1; 		mov x1, x2; 		mov w0, #1; br x1; 		NOP
// 0 "" 2
#NO_APP
	adrp	x0, .LC0
	add	x2, x0, :lo12:.LC0
	adrp	x0, _ZZ20load_destroy_contextE19__PRETTY_FUNCTION__
	add	x1, x0, :lo12:_ZZ20load_destroy_contextE19__PRETTY_FUNCTION__
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	mov	x3, x2
	mov	x2, x1
	mov	w1, 56
	bl	__assert2
	.cfi_endproc
.LFE429:
	.size	load_destroy_context, .-load_destroy_context
	.align	2
	.type	save_context_dummy, %function
save_context_dummy:
.LFB430:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	mov	w0, 4
	str	w0, [sp, 12]
	nop
	add	sp, sp, 16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE430:
	.size	save_context_dummy, .-save_context_dummy
	.section	.rodata
	.align	3
.LC2:
	.string	"c->ip == c->stack.sp - sizeof(addr_t)*6"
	.align	3
.LC3:
	.string	"g[0] == c->stack.fp"
	.text
	.align	2
	.global	some_func
	.type	some_func, %function
some_func:
.LFB431:
	.cfi_startproc
	stp	x29, x30, [sp, -48]!
	.cfi_def_cfa_offset 48
	.cfi_offset 29, -48
	.cfi_offset 30, -40
	add	x29, sp, 0
	.cfi_def_cfa_register 29
	str	x0, [x29, 24]
	str	x1, [x29, 16]
#APP
// 81 "../kernel/Scheduling/context.cpp" 1
	mov x1, SP
// 0 "" 2
#NO_APP
	ldr	x0, [x29, 24]
	str	x1, [x0, 24]
	ldr	x0, [x29, 24]
	ldr	x1, [x0, 24]
	ldr	x0, [x29, 24]
	ldr	x0, [x0, 8]
	sub	x0, x0, #48
	cmp	x1, x0
	beq	.L6
	adrp	x0, .LC2
	add	x2, x0, :lo12:.LC2
	adrp	x0, _ZZ9some_funcE19__PRETTY_FUNCTION__
	add	x1, x0, :lo12:_ZZ9some_funcE19__PRETTY_FUNCTION__
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	mov	x3, x2
	mov	x2, x1
	mov	w1, 82
	bl	__assert2
.L6:
#APP
// 85 "../kernel/Scheduling/context.cpp" 1
	mov x0, FP
// 0 "" 2
#NO_APP
	str	x0, [x29, 40]
	ldr	x0, [x29, 40]
	ldr	x1, [x0]
	ldr	x0, [x29, 24]
	ldr	x0, [x0, 16]
	cmp	x1, x0
	beq	.L8
	adrp	x0, .LC3
	add	x2, x0, :lo12:.LC3
	adrp	x0, _ZZ9some_funcE19__PRETTY_FUNCTION__
	add	x1, x0, :lo12:_ZZ9some_funcE19__PRETTY_FUNCTION__
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	mov	x3, x2
	mov	x2, x1
	mov	w1, 86
	bl	__assert2
.L8:
	nop
	ldp	x29, x30, [sp], 48
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa 31, 0
	ret
	.cfi_endproc
.LFE431:
	.size	some_func, .-some_func
	.align	2
	.global	save_context
	.type	save_context, %function
save_context:
.LFB432:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	add	x29, sp, 0
	.cfi_def_cfa_register 29
	str	x0, [x29, 24]
#APP
// 95 "../kernel/Scheduling/context.cpp" 1
	mov x1, SP
// 0 "" 2
#NO_APP
	ldr	x0, [x29, 24]
	str	x1, [x0, 8]
	ldr	x0, [x29, 24]
	ldr	x0, [x0, 8]
	add	x0, x0, 32
	mov	x1, x0
	ldr	x0, [x29, 24]
	str	x1, [x0, 8]
#APP
// 97 "../kernel/Scheduling/context.cpp" 1
	NOP
// 0 "" 2
// 98 "../kernel/Scheduling/context.cpp" 1
	mov x1, FP
// 0 "" 2
#NO_APP
	ldr	x0, [x29, 24]
	str	x1, [x0, 24]
	ldr	x0, [x29, 24]
	ldr	x0, [x0, 24]
	ldr	x1, [x0]
	ldr	x0, [x29, 24]
	str	x1, [x0, 16]
	ldr	x0, [x29, 24]
	ldr	x0, [x0, 24]
	ldr	x1, [x0, 8]
	ldr	x0, [x29, 24]
	str	x1, [x0, 24]
	bl	save_context_dummy
	mov	w0, 0
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa 31, 0
	ret
	.cfi_endproc
.LFE432:
	.size	save_context, .-save_context
	.align	2
	.global	__save_context_store
	.type	__save_context_store, %function
__save_context_store:
.LFB433:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	str	x0, [sp, 24]
	str	x1, [sp, 16]
	str	x2, [sp, 8]
	str	x3, [sp]
	ldr	x0, [sp, 24]
	ldr	x1, [sp, 16]
	str	x1, [x0, 24]
	nop
	add	sp, sp, 32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE433:
	.size	__save_context_store, .-__save_context_store
	.section	.rodata
	.align	3
	.type	_ZZ20load_destroy_contextE19__PRETTY_FUNCTION__, %object
	.size	_ZZ20load_destroy_contextE19__PRETTY_FUNCTION__, 84
_ZZ20load_destroy_contextE19__PRETTY_FUNCTION__:
	.string	"void Kernel::load_destroy_context(Kernel::context_t*, void (*)(Kernel::context_t*))"
	.align	3
	.type	_ZZ9some_funcE19__PRETTY_FUNCTION__, %object
	.size	_ZZ9some_funcE19__PRETTY_FUNCTION__, 50
_ZZ9some_funcE19__PRETTY_FUNCTION__:
	.string	"void Kernel::some_func(Kernel::context_t*, void*)"
	.ident	"GCC: (GNU) 7.2.0"
	.section	.note.GNU-stack,"",@progbits
