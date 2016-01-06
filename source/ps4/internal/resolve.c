#include <stddef.h>

#include "ps4/resolve.h"
#include "ps4/inline.h"
#include "ps4/internal/resolve.h"

long ps4ResolveSyscall(long n, ...);
PS4SyscallNamed(ps4ResolveSyscall, 0)

static volatile PS4ResolveHandler ps4ResolveErrorHandler;
static volatile PS4ResolveHandler ps4ResolvePreHandler;
static volatile PS4ResolveHandler ps4ResolvePostHandler;

/* inline */
PS4Inline PS4ResolveStatus ps4ResolveCallHandler(PS4ResolveHandler handler, char *moduleName, char *symbolName, int *module, void **symbol, PS4ResolveStatus state)
{
	if(handler != NULL)
		state = handler(moduleName, symbolName, module, symbol, state);
	return state;
}

PS4ResolveStatus ps4ResolveModuleAndSymbol(char *moduleName, char *symbolName, int *module, void **symbol)
{
	static int (*lsm)(const char *name, size_t argc, const void *argv, unsigned int flags, int a, int b) = NULL;
	int state;

	if((state = ps4ResolveCallHandler(ps4ResolvePreHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusInterceptContinue)) != PS4ResolveStatusInterceptContinue)
		return state;

	if(module == NULL || symbol == NULL || moduleName == NULL || symbolName == NULL)
		if((state = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusArgumentError)) != PS4ResolveStatusInterceptContinue)
			return state;

	if(*module <= 0)
	{
		if(!lsm)
		{
			int k = 0;
			ps4ResolveSyscall(594, "libkernel.sprx", 0, &k, 0);
			if(k <= 0)
				if((state = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusKernelLoadError)) != PS4ResolveStatusInterceptContinue)
					return state;
			if(ps4ResolveSyscall(591, k, "sceKernelLoadStartModule", (void **)&lsm) != 0)
				if((state = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusLSMResolveError)) != PS4ResolveStatusInterceptContinue)
					return state;
		}
		*module = lsm(moduleName, 0, NULL, 0, 0, 0);
		if(*module <= 0)
			if((state = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusModuleLoadError)) != PS4ResolveStatusInterceptContinue)
				return state;
	}

	if(ps4ResolveSyscall(591, *module, symbolName, symbol) != 0)
		if((state = ps4ResolveCallHandler(ps4ResolveErrorHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusFunctionResolveError)) != PS4ResolveStatusInterceptContinue)
			return state;

	if((state = ps4ResolveCallHandler(ps4ResolvePostHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusInterceptContinue)) != PS4ResolveStatusInterceptContinue)
		return state;

	return PS4ResolveStatusSuccess;
}

PS4ResolveStatus ps4ResolvePostIntercept(char *moduleName, char *symbolName, int *module, void **symbol, PS4ResolveStatus state)
{
	return PS4ResolveStatusInterceptFailure;
}

PS4ResolveStatus ps4Resolve(void *function) // FIXME: thread save
{
	PS4ResolveStatus s = PS4ResolveStatusFunctionResolveError;
	PS4ResolveHandler t = NULL;
	unsigned char *b = (unsigned char *)function;

	char *moduleName = *(char **)(b + 30); // alter if resolve changes
	char *symbolName = *(char **)(b + 40);
	int *module = (int *)(*(int **)(b + 50));
	void **symbol = (void **)(*(void ***)(b + 60));

	// check arbitrary bytes
	if(*(b + 13) != 0x49 || *(b + 14) != 0x93)
		return s;

	t = ps4ResolveSetPostHandler(ps4ResolvePostIntercept);
	s = ps4ResolveModuleAndSymbol(moduleName, symbolName, module, symbol);
	ps4ResolveSetPostHandler(t);

	if(t == NULL)
		return s;
	else if((s = ps4ResolveCallHandler(ps4ResolvePostHandler, moduleName, symbolName, module, symbol, PS4ResolveStatusInterceptContinue)) != PS4ResolveStatusInterceptContinue)
		return s;

	return PS4ResolveStatusSuccess;
}

PS4ResolveHandler ps4ResolveSetErrorHandler(PS4ResolveHandler handler)
{
	PS4ResolveHandler r = ps4ResolveErrorHandler;
	ps4ResolveErrorHandler = handler;
	return r;
}

PS4ResolveHandler ps4ResolveSetPreHandler(PS4ResolveHandler handler)
{
	PS4ResolveHandler r = ps4ResolvePreHandler;
	ps4ResolvePreHandler = handler;
	return r;
}

PS4ResolveHandler ps4ResolveSetPostHandler(PS4ResolveHandler handler)
{
	PS4ResolveHandler r = ps4ResolvePostHandler;
	ps4ResolvePostHandler = handler;
	return r;
}
