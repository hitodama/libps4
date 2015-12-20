#include ps4/internal/pushpop.h

#FIXME: for completeness, take care of st0 ... one day ;)

.pushsection .text
.global ps4Pushall
.type ps4Pushall, @function
ps4Pushall:
	sub $240, %rsp

	movq %rax, 8(%rsp)
	movq 240(%rsp), %rax

	movdqu %xmm7, 232(%rsp)
	movdqu %xmm6, 216(%rsp)
	movdqu %xmm5, 200(%rsp)
	movdqu %xmm4, 184(%rsp)
	movdqu %xmm3, 168(%rsp)
	movdqu %xmm2, 152(%rsp)
	movdqu %xmm1, 136(%rsp)
	movdqu %xmm0, 120(%rsp)

	movq %r15, 104(%rsp)
	movq %r14, 96(%rsp)
	movq %r13, 88(%rsp)
	movq %r12, 80(%rsp)
	movq %r10, 72(%rsp)

	movq %r9, 64(%rsp)
	movq %r8, 56(%rsp)
	movq %rdi, 48(%rsp)
	movq %rsi, 40(%rsp)
	movq %rdx, 32(%rsp)
	movq %rcx, 24(%rsp)
	movq %rbx, 16(%rsp)

	movq %rax, (%rsp)

	ret
.size ps4Pushall, .-ps4Pushall

.global ps4Popall
.type ps4Popall, @function
ps4Popall:
	movq (%rsp), %rax
	movq %rax, 240(%rsp)

	movq 8(%rsp), %rax
	movq 16(%rsp), %rbx
	movq 24(%rsp), %rcx
	movq 32(%rsp), %rdx
	movq 40(%rsp), %rsi
	movq 48(%rsp), %rdi
	movq 56(%rsp), %r8
	movq 64(%rsp), %r9

	mov 72(%rsp), %r10
	mov 80(%rsp), %r12
	mov 88(%rsp), %r13
	mov 96(%rsp), %r14
	mov 104(%rsp), %r15

	movdqu 120(%rsp), %xmm0
	movdqu 136(%rsp), %xmm1
	movdqu 152(%rsp), %xmm2
	movdqu 168(%rsp), %xmm3
	movdqu 184(%rsp), %xmm4
	movdqu 200(%rsp), %xmm5
	movdqu 216(%rsp), %xmm6
	movdqu 232(%rsp), %xmm7

	add $240, %rsp

	ret
.size ps4Popall, .-ps4Popall
.popsection
