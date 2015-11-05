#include "internal/resolve.h"

unsigned long resolveSyscall(unsigned long n, ...);
SyscallNamed(resolveSyscall, 0)

int resolveModuleAndSymbol(int64_t *module, void **symbol, char *moduleName, char *symbolName)
{
	static int (*lsm)(const char *name, size_t argc, const void *argv, unsigned int flags, int a, int b) = NULL;

	if(module == NULL || symbol == NULL || moduleName == NULL || symbolName == NULL)
		return -1;

	if(*module <= 0)
	{
		if(!lsm)
		{
			int64_t k = 0;
			resolveSyscall(594, "libkernel.sprx", 0, &k, 0);
			if(k <= 0)
				return -1;
			if(resolveSyscall(591, k, "sceKernelLoadStartModule", (void **)&lsm) != 0)
 				return -1;
		}
		*module = lsm(moduleName, 0, NULL, 0, 0, 0);
		if(*module <= 0)
			return -1;
	}

	if(resolveSyscall(591, *module, symbolName, symbol) != 0)
		return -1;

	return 0;
}
