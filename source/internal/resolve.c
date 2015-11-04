#include "internal/resolve.h"

unsigned long resolveSyscall(unsigned long n, ...);
SyscallNamed(resolveSyscall, 0)

void resolveModuleSymbol(int64_t *module, void **symbol, char *moduleName, char *symbolName)
{
	static int k = 0;
	static int (*lsm)(const char *name, size_t argc, const void *argv, unsigned int flags, int a, int b) = NULL;

	if(lsm)
		*module = lsm(moduleName, 0, NULL, 0, 0, 0);

	if(!*module)
	{
		if(!k)
			resolveSyscall(594, "libkernel.sprx", 0, &k, 0);
		if(k)
			resolveSyscall(591, k, "sceKernelLoadStartModule", (void **)&lsm);
		if(lsm)
			*module = lsm(moduleName, 0, NULL, 0, 0, 0);
	}

	if(*module && *module != (int64_t)-1)
		resolveSyscall(591, *module, symbolName, symbol);
}
