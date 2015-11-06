libps4-boilerplate
=====

> Imported by libps4-generator to generate libps4

##Description

The libps4-boilerplate exposes C macros which generate code and data. libps4-generator generates small C stubs which consist of calls to these macros with generated arguments. This allows the generator to be simple and less error prone, as it does not actually need to write breakable C code, but instead to use a narrowly defined meta-language of macros.

##Internals

The libps4-boilerplate can be used to generate various different global and local, read-only and uninitialized entries in the data segments of an elf file by using macros. They can also also generate function entries in the text (code) segment.

These base macros are then further connected to expose concrete libps4 macros, which are intended to be used by libps4-generator to create valid C code files.

The macros are exposed in `resolve.h`. Any macro not listed here is considered private and improper as part of the meta-language.

###Macros
```c
Module(name)
```
Declares a module globally. Along with a data entry for the module name, an internal 8 byte entry for the modules address is declared using am internal naming convention. During the resolution process, the name will be used to find and load the module, and store its address in the 8 byte data. The name and address symbols are declared globally as they needs to be shared across translation units (files) with the Function() declarations of the same module.

```c
Function(module, name)
```
Declares a function symbol globally. Along with a data entry for the function name, an internal 8 byte entry for the function address is declared using am internal naming convention. During the resolution process, the name will be used to find and load the function, and store its address in the 8 byte data. The name and address are local as they do not need to be shared with other macros in different translation units.

```c
Syscall(name)
```
Declares a syscall symbol globally. Internally gets the approptiate syscall number from `sys/syscall.h`. Produces a simple piece of code that simply moves the number to rax and performs the `syscall` instruction.

```c
FunctionOrSyscall(module, name)
```
`Syscall()` exposes a symbol which directly calls the syscall. This is typically not done. Syscalls are instead wrapped by a user-space function which in turn is exposed. For example `libkernel.sprx` exposes `sceKernelDlsym` which is also a syscall (see include/sys/syscall.h) namely SYS_sceKernelDlsym with the number 591.

`FunctionOrSyscall()` is an extension which allows libps4 to be generated regardless of the preference to either perform a `direct syscall` or `wrapped` call instead. In wrapped mode, the module function of a syscall takes precedence over the direct call.

`FunctionOrSyscall()` is only generated for syscalls that have a corresponding ps4 exposed symbol. Any other syscalls found in headers and include/sys/syscall.h are generated to be called direct (and a `Syscall()` is generated instead).

To build a direct syscall libps4 simply provide `LibPS4SyscallDirect` to make. Alternatively provide `LibPS4SyscallWrapped` to build the lib in wrapped mode. The absence of either flag will result in an currently undefined but valid choice (we need to investigate the better choice).

###Resolution process and Overhead

When libps4 is build, a small resolver stub function is generated for each function. When a call to a libps4 function takes place, the call is intercepted. The context of the call (register arguments) are saved onto the stack, a resolution is performed, the context is restored, and the call to the resolved actual function is performed.

The following steps take place as part of the resolution:

1. If the call is the first call ever made to the library
	- Load (determine the id of) libkernel.sprx, resolve the symbol sceKernelLoadStartModule to load and potentially start any other needed modules in the future
	- Save the address of lsm in static (global) variables (bss)
2. If the call is the first call to a module
	- sceKernelLoadStartModule the module by name
	- Save the address in the modules static global variable (bss)
3. If the call is the first call to a function
	- sceKernelDlsym the function by name
	- Save the address into a (hidden) local variable (bss)

Afterwards the call is performed against the hidden function address, regardless of which of these conditions was true. So most function calls in a module will only need to resolve their own symbol, and that only once. Afterwards, they are redirected without further performance impacts (aside from the negligible redirection (jmp) itself. In return, functions don't need to be pointers, and std headers can easily be imported without a translation.

On error, of either of the obove steps of resolution. The call performs no operation and returns -1. This is at best avoided (since -1 may be a desired valid return value (e.g. strcmp), however as the generator is able to check all functions for they ability to be resolved - and error should not occur on a `check` generated libps4 build. The prevention of runtime complications (such as out of memory during resolution) will further be a mart of the libraries hardening process.

##Files
```
Makefile                        /* Makefile to build libps4 */
crt0.s                          /* Can be linked in binary builds */

/* include|source/internal */
pushpops.s|.h                   /* Save SystemV registers (function arguments) before resolve */
kerncall.s                      /* Symbol which performs an asm syscall instruction */
resolve.c                       /* Resolves module and symbol name dynamically */
resolve.h                       /* Multiple macros to generate stubs (magic happens here) */

/* include/sys */
syscall.h                       /* List of all libps4 supported syscalls */

```

##TODO
- Extend syscall.h
- Cleanup, Minor changes
- Harden resolve (sceKernelLoadStartModule can exhaust internal sce constraints and fail)
