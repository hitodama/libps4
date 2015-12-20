#include "ps4/internal/resolve.h"

int64_t ps4ResolveSyscall(int64_t n, ...);
PS4SyscallNamed(ps4ResolveSyscall, 0)

static PS4ResolveHandler ps4ResolveErrorHandler;
static PS4ResolveHandler ps4ResolvePreHandler;
static PS4ResolveHandler ps4ResolvePostHandler;

/* inline */
PS4Inline PS4ResolveStatus ps4ResolveCallHandler(PS4ResolveHandler h, char *moduleName, char *symbolName, int64_t *module, void **symbol, PS4ResolveStatus stat)
{
	if(h != NULL)
		stat = h(moduleName, symbolName, module, symbol, stat);
	return stat;
}

PS4ResolveStatus ps4ResolveModuleAndSymbol(char *moduleName, char *symbolName, int64_t *module, void **symbol)
{
	static int (*lsm)(const char *name, size_t argc, const void *argv, unsigned int flags, int a, int b) = NULL;
	int stat;

	if((stat = ps4ResolveCallHandler(ps4ResolvePreHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusInterceptContinue)) != PS4ResolveStatusInterceptContinue)
		return stat;

	if(module == NULL || symbol == NULL || moduleName == NULL || symbolName == NULL)
		if((stat = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusArgumentError)) != PS4ResolveStatusInterceptContinue)
			return stat;

	if(*module <= 0)
	{
		if(!lsm)
		{
			int64_t k = 0;
			ps4ResolveSyscall(594, "libkernel.sprx", 0, &k, 0);
			if(k <= 0)
				if((stat = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusKernelLoadError)) != PS4ResolveStatusInterceptContinue)
					return stat;
			if(ps4ResolveSyscall(591, k, "sceKernelLoadStartModule", (void **)&lsm) != 0)
				if((stat = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusLSMResolveError)) != PS4ResolveStatusInterceptContinue)
					return stat;
		}
		*module = lsm(moduleName, 0, NULL, 0, 0, 0);
		if(*module <= 0)
			if((stat = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusModuleLoadError)) != PS4ResolveStatusInterceptContinue)
				return stat;
	}

	if(ps4ResolveSyscall(591, *module, symbolName, symbol) != 0)
		if((stat = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusFunctionResolveError)) != PS4ResolveStatusInterceptContinue)
			return stat;

	if((stat = ps4ResolveCallHandler(ps4ResolvePostHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusInterceptContinue)) != PS4ResolveStatusInterceptContinue)
		return stat;

	return PS4ResolveStatusSuccess;
}

PS4ResolveHandler ps4ResolveSetErrorHandler(PS4ResolveHandler errorHandler)
{
	PS4ResolveHandler r = ps4ResolveErrorHandler;
	ps4ResolveErrorHandler = errorHandler;
	return r;
}
