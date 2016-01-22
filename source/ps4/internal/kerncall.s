#include ps4/internal/resolve.h

.pushsection .text
.global ps4Kerncall
.type ps4Kerncall, @function
ps4Kerncall:
	movq $0x9263FFFF8, %r11
	movq (%r11), %r11
	movq %rcx, %r10
	call *%r11
	jnc ps4KerncallR
	# Be aware that direct mode does not set errno
	# "neg %rax" would work too - but break more
	movq $-1, %rax
ps4KerncallR:
	ret
.size ps4Kerncall, .-ps4Kerncall
.popsection
