#include internal/resolve.h

.pushsection .text
.global kerncall
.type kerncall, @function
kerncall:
	movq $0x9263FFFF8, %r11
	movq (%r11), %r11
	movq %rcx, %r10
	jmp *%r11
.size kerncall, .-kerncall
.popsection
