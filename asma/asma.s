	.file	"asma.c"
	.text
.globl asma
	.type	asma, @function
asma:
.LFB0:
	.cfi_startproc
	movq	(%rdi), %rax
	movq	8(%rdi), %rcx
	subq	(%rsi), %rax
	sbbq	8(%rsi), %rcx
	movq	%rax, (%rdx)
	movq	%rcx, 8(%rdx)
	ret
	.cfi_endproc
.LFE0:
	.size	asma, .-asma
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",@progbits
