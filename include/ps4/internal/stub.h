#ifndef LibPS4InternalStubH
#define LibPS4InternalStubH

#include <stddef.h>
#include <stdint.h>
#include <sys/syscall.h>

#include <ps4/internal/pushpop.h>
#include <ps4/internal/resolve.h>

#include <ps4/resolve.h>

typedef enum
{
	PS4StubTypeFunction = 0,
	PS4StubTypeKernelFunction = 1,
	PS4StubTypeFunctionAndKernelFunction = 2,
	PS4StubTypeSyscallAndKernelFunction = 3,
	PS4StubTypeSyscall = 4
}
PS4StubType; // sync with stubs first mnemonic and resolve.c

#define ps4StubType(fn) ((int)((char *)fn)[2])

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
#ifndef PS4KernelFunctionAddressSymbol
	#define PS4KernelFunctionAddressSymbol(name) \
		.L##name##KernelAddress
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

#ifndef PS4ModuleNameData
	#define PS4ModuleNameData(name, value) \
		PS4ModuleNameData_(name, value)
	#define PS4ModuleNameData_(name, value) \
		PS4Data(.rodata, .global, name, @object, .asciz #value)
#endif

#ifndef PS4FunctionAddressData
	#define PS4FunctionAddressData(name) \
		PS4FunctionAddressData_(name)
	#define PS4FunctionAddressData_(name) \
		PS4Data(.bss, .local, name, @object, .zero 8)
#endif

#ifndef PS4KernelFunctionAddressData
	#define PS4KernelFunctionAddressData(name) \
		PS4KernelFunctionAddressData_(name)
	#define PS4KernelFunctionAddressData_(name) \
		PS4Data(.bss, .local, name, @object, .zero 8)
#endif

#ifndef PS4FunctionNameData
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

// if you call these in kernel mode you shoot yourself (sometimes) (could check)
#ifndef PS4FunctionStub
	#define PS4FunctionStub(function, moduleName, functionName, moduleHandle, functionAddress) \
		PS4FunctionStub_(function, moduleName, functionName, moduleHandle, functionAddress)

	#define PS4FunctionStub_(function, moduleName, functionName, moduleHandle, functionAddress) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				test $0, %al #this is a two byte type marker, see resolve.c \n \
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
					movabs $"#function", %r9 \n \
					xor %rax, %rax \n \
					call ps4ResolveModuleAndSymbolOrKernelSymbol \n \
					mov %rax, %r11 \n \
					call ps4Popall \n \
					test %r11, %r11 \n \
					jnz .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

/*
				test %r11, %r11 \n \
				jz .L"#function"R \n \
					movq %rsp, %r11 \n \
					shrq $48, %r11 \n \
					test %r11, %r11 \n \
					jz .L"#function"U \n \
					jmp *%r11 \n \
				.L"#function"U: \n \
					kerncall
				.L"#function"R:
*/

// if you call these in user mode you shoot yourself (could check)
#ifndef PS4KernelFunctionStub
	#define PS4KernelFunctionStub(function, functionName, functionAddress) \
		PS4KernelFunctionStub_(function, functionName, functionAddress)

	#define PS4KernelFunctionStub_(function, functionName, functionAddress) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				test $1, %al \n \
				mov %rax, %r11 \n \
				movabs "#functionAddress", %rax \n \
				xchg %rax, %r11 \n \
				test %r11, %r11 \n \
				jz .L"#function"R \n \
					jmp *%r11 \n \
				.L"#function"R: \n \
					call ps4Pushall \n \
					movabs $"#functionName", %rsi \n \
					movabs $"#functionAddress", %r8 \n \
					movabs $"#function", %r9 \n \
					xor %rax, %rax \n \
					call ps4ResolveModuleAndSymbolOrKernelSymbol \n \
					mov %rax, %r11 \n \
					call ps4Popall \n \
					test %r11, %r11 \n \
					jnz .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

// use rsp to check ring 0 ...
// if so, resolve and user kernel fn
// otherwise use useland stuff
#ifndef PS4FunctionAndKernelFunctionStub
	#define PS4FunctionAndKernelFunctionStub(function, moduleName, functionName, moduleHandle, userFunctionAddress, kernelFunctionAddress) \
		PS4FunctionAndKernelFunctionStub_(function, moduleName, functionName, moduleHandle, userFunctionAddress, kernelFunctionAddress)

	#define PS4FunctionAndKernelFunctionStub_(function, moduleName, functionName, moduleHandle, userFunctionAddress, kernelFunctionAddress) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				test $2, %al \n \
				mov %rax, %r11 \n \
				movq %rsp, %rax \n \
				shrq $48, %rax \n \
				test %rax, %rax \n \
				jnz .L"#function"K \n \
					movabs "#userFunctionAddress", %rax \n \
					xchg %rax, %r11 \n \
					test %r11, %r11 \n \
					jz .L"#function"R \n \
						jmp *%r11 \n \
				.L"#function"K: \n \
					movabs "#kernelFunctionAddress", %rax \n \
					xchg %rax, %r11 \n \
					test %r11, %r11 \n \
					jz .L"#function"R \n \
						jmp *%r11 \n \
				.L"#function"R: \n \
					call ps4Pushall \n \
					movabs $"#moduleName", %rdi \n \
					movabs $"#functionName", %rsi \n \
					movabs $"#moduleHandle", %rdx \n \
					movabs $"#userFunctionAddress", %rcx \n \
					movabs $"#kernelFunctionAddress", %r8 \n \
					movabs $"#function", %r9 \n \
					xor %rax, %rax \n \
					call ps4ResolveModuleAndSymbolOrKernelSymbol \n \
					mov %rax, %r11 \n \
					call ps4Popall \n \
					test %r11, %r11 \n \
					jnz .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

#ifndef PS4SyscallAndKernelFunctionStub
	#define PS4SyscallAndKernelFunctionStub(function, functionName, functionAddress, number) \
		PS4SyscallAndKernelFunctionStub_(function, functionName, functionAddress, number)

	#define PS4SyscallAndKernelFunctionStub_(function, functionName, functionAddress, number) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				test $3, %al \n \
				mov %rax, %r11 \n \
				movq %rsp, %rax \n \
				shrq $48, %rax \n \
				test %rax, %rax \n \
				jnz .L"#function"K \n \
					movq $"#number", %rax \n \
					jmp ps4Kerncall\n \
				.L"#function"K: \n \
					movabs "#functionAddress", %rax \n \
					xchg %rax, %r11 \n \
					test %r11, %r11 \n \
					jz .L"#function"R \n \
						jmp *%r11 \n \
				.L"#function"R: \n \
					call ps4Pushall \n \
					movabs $"#functionName", %rdi \n \
					movabs $"#functionAddress", %r8 \n \
					movabs $"#function", %r9 \n \
					xor %rax, %rax \n \
					call ps4ResolveModuleAndSymbolOrKernelSymbol \n \
					mov %rax, %r11 \n \
					call ps4Popall \n \
					test %r11, %r11 \n \
					jnz .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

#ifndef PS4SyscallStub
	#define PS4SyscallStub(function, number) \
		PS4SyscallStub_(function, number)

	#define PS4SyscallStub_(function, number) \
		__asm__("\
			.pushsection .text \n \
			.global "#function"\n \
			.type "#function", @function \n \
		 	"#function":\n \
				test $4, %al \n \
				movq $"#number", %rax \n \
				jmp ps4Kerncall\n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

#ifndef PS4SyscallNamed
	#define PS4SyscallNamed(name, number) \
		PS4SyscallNamed_(name, number)

	#define PS4SyscallNamed_(name, number) \
		PS4SyscallStub(PS4SyscallSymbol(name), number)
#endif

#ifndef Syscall
	#define PS4Syscall(name) \
		PS4Syscall_(name)

	#define PS4Syscall_(name) \
		PS4SyscallStub(PS4SyscallSymbol(name), PS4SyscallNumberSymbol(name))
#endif

#ifndef PS4Function
	#define PS4Function(module, name) \
		PS4Function_(module, name)

	#define PS4Function_(module, name) \
		PS4FunctionNameData(PS4FunctionNameSymbol(name), PS4FunctionSymbol(name)) \
		PS4FunctionAddressData(PS4FunctionAddressSymbol(name)) \
		PS4FunctionStub(PS4FunctionSymbol(name), PS4ModuleNameSymbol(module), PS4FunctionNameSymbol(name),  PS4ModuleSymbol(module), PS4FunctionAddressSymbol(name))
#endif

#ifndef PS4KernelFunction
	#define PS4KernelFunction(name) \
		PS4KernelFunction_(name)

	#define PS4KernelFunction_(name) \
		PS4FunctionNameData(PS4FunctionNameSymbol(name), PS4FunctionSymbol(name)) \
		PS4KernelFunctionAddressData(PS4KernelFunctionAddressSymbol(name)) \
		PS4KernelFunctionStub(PS4FunctionSymbol(name), PS4FunctionNameSymbol(name), PS4KernelFunctionAddressSymbol(name))
#endif

#ifndef PS4FunctionAndKernelFunction
	#define PS4FunctionAndKernelFunction(module, name) \
		PS4FunctionAndKernelFunction_(module, name)

	#define PS4FunctionAndKernelFunction_(module, name) \
		PS4FunctionNameData(PS4FunctionNameSymbol(name), PS4FunctionSymbol(name)) \
		PS4FunctionAddressData(PS4FunctionAddressSymbol(name)) \
		PS4KernelFunctionAddressData(PS4KernelFunctionAddressSymbol(name)) \
		PS4FunctionAndKernelFunctionStub(PS4FunctionSymbol(name), PS4ModuleNameSymbol(module), PS4FunctionNameSymbol(name),  PS4ModuleSymbol(module), PS4FunctionAddressSymbol(name), PS4KernelFunctionAddressSymbol(name))
#endif

#ifndef PS4SyscallAndKernelFunction
	#define PS4SyscallAndKernelFunction(name) \
		PS4SyscallAndKernelFunction_(name)

	#define PS4SyscallAndKernelFunction_(name) \
		PS4FunctionNameData(PS4FunctionNameSymbol(name), PS4FunctionSymbol(name)) \
		PS4KernelFunctionAddressData(PS4KernelFunctionAddressSymbol(name)) \
		PS4SyscallAndKernelFunctionStub(PS4FunctionSymbol(name), PS4FunctionNameSymbol(name), PS4KernelFunctionAddressSymbol(name), PS4SyscallNumberSymbol(name))
#endif

#ifdef LibPS4KernelOnly

	#undef PS4Module
	#define PS4Module(module)

	#undef PS4Syscall
	#define PS4Syscall(module)

	#undef PS4Function
	#define PS4Function(module, name)

	#undef PS4FunctionAndKernelFunction
	#define PS4FunctionAndKernelFunction(module, name) \
		PS4KernelFunction(name)

	#undef PS4SyscallAndKernelFunction
	#define PS4SyscallAndKernelFunction(module, name) \
		PS4KernelFunction(name)

	#undef PS4FunctionOrSyscall
	#define PS4FunctionOrSyscall(module, name)

	#define PS4FunctionOrSyscallAndKernelFunction(module, name) \
		PS4KernelFunction(name)

#elif defined LibPS4KernelAndUser

	#ifdef LibPS4SyscallDirect
		#define PS4FunctionOrSyscall(module, name) \
			PS4Syscall(name)
		#define PS4FunctionOrSyscallAndKernelFunction(module, name) \
			PS4SyscallAndKernelFunction(name)
	#else //LibPS4SyscallWrapped
		#define PS4FunctionOrSyscall(module, name) \
			PS4Function(module, name)
		#define PS4FunctionOrSyscallAndKernelFunction(module, name) \
			PS4FunctionAndKernelFunction(module, name)
	#endif

#else //LibPS4KernelNone

	#undef PS4FunctionAndKernelFunction
	#define PS4FunctionAndKernelFunction(module, name) \
		PS4Function(module, name)

	#undef PS4SyscallAndKernelFunction
	#define PS4SyscallAndKernelFunction(module, name) \
		PS4Syscall(name)

	#undef PS4KernelFunction
	#define PS4KernelFunction(name)

	#undef PS4FunctionAndKernelFunction
	#define PS4FunctionAndKernelFunction(module, name) \
		PS4Function(module, name)

	#ifdef LibPS4SyscallDirect
		#define PS4FunctionOrSyscall(module, name) \
			PS4Syscall(name)
		#define PS4FunctionOrSyscallAndKernelFunction(module, name) \
			PS4Syscall(name)
	#else //LibPS4SyscallWrapped
		#define PS4FunctionOrSyscall(module, name) \
			PS4Function(module, name)
		#define PS4FunctionOrSyscallAndKernelFunction(module, name) \
			PS4Function(module, name)
	#endif

#endif

#endif /*LibPS4StubH*/
