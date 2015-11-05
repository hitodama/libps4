#ifndef LibPS4ResolveH
#define LibPS4ResolveH

#include <stddef.h>
#include <stdint.h>
#include <sys/syscall.h>

#include <internal/pushpop.h>

int resolveModuleAndSymbol(int64_t *module, void **symbol, char *moduleName, char *symbolName);
uint64_t kerncall();

//#define Stringify(n) #n
//#define StringifyDeep(n) Stringify(n)
//#define Apply(n) n

#ifndef ModuleSymbol
	#define ModuleSymbol(name) name
#endif
#ifndef ModuleNameSymbol
	#define ModuleNameSymbol(name) name##Name
#endif

#ifndef FunctionSymbol
	#define FunctionSymbol(name) name
#endif
#ifndef FunctionNameSymbol
	#define FunctionNameSymbol(name) .L##name##Name
#endif
#ifndef FunctionAddressSymbol
	#define FunctionAddressSymbol(name) .L##name##Address
#endif

#ifndef SyscallSymbol
	#define SyscallSymbol(name) name
#endif
#ifndef SyscallNumber
	#define SyscallNumber(name) SYS_##name
#endif

#ifndef Data
	#define Data(section, scope, name, type, value) \
		__asm__(" \
			.pushsection "#section" \n \
			"#scope" "#name" \n \
			.type "#name", "#type" \n \
			"#name": \n \
			"#value" \n \
			.size "#name", .-"#name" \n \
			.popsection \n \
		");
#endif

#ifndef ModuleIDData
	#define ModuleIDData(name, size) \
		Data(.bss, .global, name, @object, .zero size)
#endif

#ifndef ModuleNameData
	#define ModuleNameData(name, value) \
		Data(.rodata, .global, name, @object, .asciz #value)
#endif

#ifndef FunctionAddressData
	#define FunctionAddressData(name, size) \
		Data(.bss, .local, name, @object, .zero size)
#endif

#ifndef FunctionNameData
	#define FunctionNameData(name, value) \
		Data(.rodata, .local, name, @object, .asciz #value)
#endif

#ifndef Module //static int name;
	#define Module(name) \
 		ModuleIDData(ModuleSymbol(name), 8) \
		ModuleNameData(ModuleNameSymbol(name), name.sprx)
#endif

#ifndef FunctionNamed
	#define FunctionNamed(lib, libName, fn, fnName, address) \
		FunctionNamed_(lib, libName, fn, fnName, address)
#endif

#ifndef FunctionNamed_
	#define FunctionNamed_(lib, libName, fn, fnName, address) \
		FunctionNameData(fnName, fn) \
		__asm__(" \
			.pushsection .text \n \
			.global "#fn" \n \
			.type "#fn", @function \n \
			"#fn": \n \
				mov %rax, %r11 \n \
				movabs "#address", %rax \n \
				xchg %rax, %r11 \n \
				test %r11, %r11 \n \
				je .L"#fn"Resolve \n \
				jmp *%r11 \n \
				.L"#fn"Resolve: \n \
					call pushall \n \
					movabs $"#lib", %rdi \n \
					movabs $"#address", %rsi \n \
					movabs $"#libName", %rdx \n \
					movabs $"#fnName", %rcx \n \
					xor %rax, %rax \n \
					call resolveModuleAndSymbol \n \
					mov %rax, %r11 \n \
					call popall \n \
					cmp $-1, %r11 \n \
					je .L"#fn"Error \n \
					jmp "#fn" \n \
				.L"#fn"Error: \n \
					mov $-1, %rax \n \
					ret \n \
			.size "#fn", .-"#fn" \n \
			.popsection \n \
		");
#endif

#ifndef Function
	#define Function(lib, name) \
		FunctionAddressData(FunctionAddressSymbol(name), 8) \
		FunctionNamed(ModuleSymbol(lib), ModuleNameSymbol(lib), FunctionSymbol(name), FunctionNameSymbol(name), FunctionAddressSymbol(name))
#endif

#ifndef FunctionAlias
	#define FunctionAlias(name, alias) \
		__asm__(" \
			.pushsection .text \n \
			.global "#name" \n \
			.type "#name", @function \n \
			"#name": \n \
				jmp "#alias" \n \
			.size "#name", .-"#name" \n \
			.popsection \n \
		");
#endif

#ifndef SyscallNamed
	#define SyscallNamed(name, number) SyscallNamed_(name, number)
#endif

#ifndef SyscallNamed_
	#define SyscallNamed_(name, number) \
		__asm__("\
			.pushsection .text \n \
			.global "#name"\n \
			.type "#name", @function \n \
		 	"#name":\n \
				movq $"#number", %rax \n \
				jmp kerncall\n \
			.size "#name", .-"#name" \n \
			.popsection \n \
		");
#endif

#ifndef Syscall
	#define Syscall(name) SyscallNamed(SyscallSymbol(name), SyscallNumber(name))
#endif

#ifdef LibPS4SyscallDirect
	#define FunctionOrSyscall(lib, name) Syscall(name)
#else //LibPS4SyscallWrapped
	#define FunctionOrSyscall(lib, name) Function(lib, name)
#endif

#endif /*LibPS4ResolveH*/
