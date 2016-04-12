#include ps4/internal/callbacksyscallpayload.h

.pushsection .text

.global ps4CallbackSyscallPayload
.type ps4CallbackSyscallPayload, @function
ps4CallbackSyscallPayload:
	push %r15
	movq %rdi, %r15
	movq (%rsi), %r11
	testq %r11, %r11
	jz ps4CallbackSyscallPayloadOk
	movq 8(%rsi), %rdi
	movq 16(%rsi), %rsi
	#movq 8(%rsi), %rsi
	call *%r11
	movq %rax, 0x380(%r15)
	ps4CallbackSyscallPayloadOk:
		xorq %rax, %rax
	pop %r15
	ret
.Lps4CallbackSyscallPayloadEnd:
.size ps4CallbackSyscallPayload, .-ps4CallbackSyscallPayload

.global ps4TruthFunctionPayload
.type ps4TruthFunctionPayload, @function
ps4TruthFunctionPayload:
	mov $1, %rax
	ret
.Lps4TruthFunctionPayloadEnd:
.size ps4TruthFunctionPayload, .-ps4TruthFunctionPayload

.global ps4TrampolinePayload
.type ps4TrampolinePayload, @function
ps4TrampolinePayload:
	movabs $0, %rax
	jmp *%rax
.Lps4TrampolinePayloadEnd:
.size ps4TrampolinePayload, .-ps4TrampolinePayload

.popsection

.pushsection .rodata

.global ps4CallbackSyscallPayloadSize
.type ps4CallbackSyscallPayloadSize, @object
ps4CallbackSyscallPayloadSize:
.int .Lps4CallbackSyscallPayloadEnd - ps4CallbackSyscallPayload
.size ps4CallbackSyscallPayloadSize, .-ps4CallbackSyscallPayloadSize

.global ps4TruthFunctionPayloadSize
.type ps4TruthFunctionPayloadSize, @object
ps4TruthFunctionPayloadSize:
.int .Lps4TruthFunctionPayloadEnd - ps4TruthFunctionPayload
.size ps4TruthFunctionPayloadSize, .-ps4TruthFunctionPayloadSize

.global ps4TrampolinePayloadSize
.type ps4TrampolinePayloadSize, @object
ps4TrampolinePayloadSize:
.int .Lps4TrampolinePayloadEnd - ps4TrampolinePayload
.size ps4TrampolinePayloadSize, .-ps4TrampolinePayloadSize

.popsection
