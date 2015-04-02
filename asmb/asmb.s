	.file	"asmb.c"
	.text
.globl asmb
	.type	asmb, @function
asmb:
.LFB0:
	.cfi_startproc
	mov $0, %r8
	clc
forloop:
	jrcxz endfor
	movq (%rdi,%r8,8), %rax
	movq (%rsi,%r8,8), %r9
	sbbq %r9, %rax
	movq %rax, (%rdx,%r8,8)
	inc %r8
	dec %rcx
	jmp forloop
endfor:
	setc %al
	movzx %al, %rsi
	movq $0, %rax
	sub %rsi, %rax
	movq %rax, (%rdx,%r8,8)
	ret
	.cfi_endproc
.LFE0:
	.size	asmb, .-asmb
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",@progbits
