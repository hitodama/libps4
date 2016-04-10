#include ps4/internal/callbacksyscallpayload.h

.pushsection .text
.global ps4CallbackSyscallPayload
.global ps4CallbackSyscallPayloadEnd
.type ps4CallbackSyscallPayload, @function
ps4CallbackSyscallPayload:
	push %r15
	movq %rdi, %r15
	movq (%rsi), %r11
	testq %r11, %r11
	jz ps4CallbackSyscallPayloadOk
	movq 8(%rsi), %rdi
	movq 16(%rsi), %rsi
	call *%r11
	movq %rax, 0x380(%r15)
	ps4CallbackSyscallPayloadOk:
		xorq %rax, %rax
	pop %r15
	ret
ps4CallbackSyscallPayloadEnd:
.size ps4CallbackSyscallPayload, .-ps4CallbackSyscallPayload
.popsection
