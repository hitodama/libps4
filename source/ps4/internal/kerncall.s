#include ps4/internal/resolve.h

.pushsection .text
.global ps4Kerncall
.type ps4Kerncall, @function
ps4Kerncall:
	movq $0x9263FFFF8, %r11
	movq (%r11), %r11
	movq %rcx, %r10
	jmp *%r11
.size ps4Kerncall, .-ps4Kerncall
.popsection
