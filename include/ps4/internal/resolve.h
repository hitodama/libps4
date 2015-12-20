#ifndef LibPS4ResolveH
#define LibPS4ResolveH

#include <stddef.h>
#include <stdint.h>
#include <sys/syscall.h>

#include <ps4/internal/pushpop.h>

#ifndef PS4Inline
	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199409L
		#define PS4Inline inline
	#else
		#ifdef __GNUC__
			#define PS4Inline __inline__
		#else
			#define PS4Inline
		#endif
	#endif
#endif

typedef enum
{
	PS4ResolveStatusSuccess = 0,
	PS4ResolveStatusInterceptContinue = 1,
	PS4ResolveStatusInterceptFailure = -6,
	PS4ResolveStatusArgumentError = -1,
	PS4ResolveStatusKernelLoadError = -2,
	PS4ResolveStatusLSMResolveError = -3,
	PS4ResolveStatusModuleLoadError = -4,
	PS4ResolveStatusFunctionResolveError = -5
}
PS4ResolveStatus;

typedef PS4ResolveStatus (*PS4ResolveHandler)(char *moduleName, char *symbolName, int64_t *module, void **symbol, PS4ResolveStatus state);

PS4ResolveStatus ps4ResolveModuleAndSymbol(char *moduleName, char *symbolName, int64_t *module, void **symbol);
PS4ResolveHandler ps4ResolveSetErrorHandler(PS4ResolveHandler errorHandler);
PS4ResolveHandler ps4ResolveSetPreHandler(PS4ResolveHandler preHandler);
PS4ResolveHandler ps4ResolveSetPostHandler(PS4ResolveHandler postHandler);

uint64_t ps4Kerncall();

#ifndef PS4ModuleSymbol
	#define PS4ModuleSymbol(name) \
		name
#endif
#ifndef PS4ModuleNameSymbol
	#define PS4ModuleNameSymbol(name) \
		name##Name
#endif

#ifndef PS4FunctionSymbol
	#define PS4FunctionSymbol(name) \
		name
#endif
#ifndef PS4FunctionNameSymbol
	#define PS4FunctionNameSymbol(name) \
		.L##name##Name
#endif
#ifndef PS4FunctionAddressSymbol
	#define PS4FunctionAddressSymbol(name) \
		.L##name##Address
#endif

#ifndef PS4SyscallSymbol
	#define PS4SyscallSymbol(name) name
#endif
#ifndef PS4SyscallNumberSymbol
	#define PS4SyscallNumberSymbol(name) SYS_##name
#endif

#ifndef PS4Data
	#define PS4Data(section, scope, name, type, value) \
		PS4Data_(section, scope, name, type, value)

	#define PS4Data_(section, scope, name, type, value) \
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

#ifndef PS4ModuleData
	#define PS4ModuleHandleData(name) \
		PS4ModuleHandleData_(name)
	#define PS4ModuleHandleData_(name) \
		PS4Data(.bss, .global, name, @object, .zero 8)
#endif

#ifndef ModuleNameData
	#define PS4ModuleNameData(name, value) \
		PS4ModuleNameData_(name, value)
	#define PS4ModuleNameData_(name, value) \
		PS4Data(.rodata, .global, name, @object, .asciz #value)
#endif

#ifndef FunctionAddressData
	#define PS4FunctionAddressData(name) \
		PS4FunctionAddressData_(name)
	#define PS4FunctionAddressData_(name) \
		PS4Data(.bss, .local, name, @object, .zero 8)
#endif

#ifndef FunctionNameData
	#define PS4FunctionNameData(name, value) \
		PS4FunctionNameData_(name, value)
	#define PS4FunctionNameData_(name, value) \
		PS4Data(.rodata, .local, name, @object, .asciz #value)
#endif

#ifndef PS4Module
	#define PS4Module(name) \
		PS4Module_(name)
	#define PS4Module_(name) \
 		PS4ModuleHandleData(PS4ModuleSymbol(name)) \
		PS4ModuleNameData(PS4ModuleNameSymbol(name), name.sprx)
#endif

#ifndef PS4FunctionStub
	#define PS4FunctionStub(function, moduleName, functionName, moduleHandle, functionAddress) \
		PS4FunctionStub_(function, moduleName, functionName, moduleHandle, functionAddress)

	#define PS4FunctionStub_(function, moduleName, functionName, moduleHandle, functionAddress) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				mov %rax, %r11 \n \
				movabs "#functionAddress", %rax \n \
				xchg %rax, %r11 \n \
				test %r11, %r11 \n \
				jz .L"#function"R \n \
				jmp *%r11 \n \
				.L"#function"R: \n \
					call ps4Pushall \n \
					movabs $"#moduleName", %rdi \n \
					movabs $"#functionName", %rsi \n \
					movabs $"#moduleHandle", %rdx \n \
					movabs $"#functionAddress", %rcx \n \
					xor %rax, %rax \n \
					call ps4ResolveModuleAndSymbol \n \
					mov %rax, %r11 \n \
					call ps4Popall \n \
					test %r11, %r11 \n \
					js .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

#ifndef PS4FunctionNamed
	#define PS4FunctionNamed(module, symbol, name) \
		PS4FunctionNamed_(module, symbol, name)

	#define PS4FunctionNamed_(module, symbol, name) \
		PS4FunctionNameData(PS4FunctionNameSymbol(symbol), PS4FunctionSymbol(name)) \
		PS4FunctionAddressData(PS4FunctionAddressSymbol(symbol)) \
		PS4FunctionStub(PS4FunctionSymbol(symbol), PS4ModuleNameSymbol(module), PS4FunctionNameSymbol(symbol),  PS4ModuleSymbol(module), PS4FunctionAddressSymbol(symbol))
#endif

#ifndef PS4Function
	#define PS4Function(module, name) \
		PS4Function_(module, name)

	#define PS4Function_(module, name) \
		PS4FunctionNamed(module, name, name)
#endif

#ifndef PS4SyscallNamed
	#define PS4SyscallNamed(name, number) \
		PS4SyscallNamed_(name, number)

	#define PS4SyscallNamed_(name, number) \
		__asm__("\
			.pushsection .text \n \
			.global "#name"\n \
			.type "#name", @function \n \
		 	"#name":\n \
				movq $"#number", %rax \n \
				jmp ps4Kerncall\n \
			.size "#name", .-"#name" \n \
			.popsection \n \
		");
#endif

#ifndef Syscall
	#define PS4Syscall(name) \
		PS4Syscall_(name)
	#define PS4Syscall_(name) \
		PS4SyscallNamed(PS4SyscallSymbol(name), PS4SyscallNumberSymbol(name))
#endif

#ifdef LibPS4SyscallDirect
	#define PS4FunctionOrSyscall(module, name) \
		PS4Syscall(name)
#else //LibPS4SyscallWrapped
	#define PS4FunctionOrSyscall(module, name) \
		PS4Function(module, name)
#endif

#endif /*LibPS4ResolveH*/
