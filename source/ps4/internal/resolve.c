#include <stddef.h>
#include <stdarg.h>

#include <ps4/internal/stub.h>
#include <ps4/internal/kerncall.h>

#include <ps4/inline.h>
#include <ps4/kernel.h>

#include <ps4/internal/resolve.h>
#include <ps4/resolve.h>

long ps4ResolveSyscall(long n, ...);
PS4SyscallNamed(ps4ResolveSyscall, 0)

static volatile PS4ResolveHandler ps4ResolveErrorHandler;
static volatile PS4ResolveHandler ps4ResolvePreHandler;
static volatile PS4ResolveHandler ps4ResolvePostHandler;

/* inline */
PS4Inline PS4ResolveStatus ps4ResolveCallHandler(PS4ResolveHandler handler, PS4ResolveState *state, PS4ResolveStatus status)
{
	state->status = status;
	if(handler != NULL)
		status = handler(state);
	return status;
}

#ifdef DEBUG
int ps4ResolveEarlyPrintf(const char *format, ...)
{
	static int (*vpf)(const char *format, ...);
	int r;
	va_list args;

	va_start(args, format);

	if(vpf == NULL)
	{
		int c = 0;
		ps4ResolveSyscall(594, "libSceLibcInternal.sprx", 0, &c, 0);
		ps4ResolveSyscall(591, c, "vprintf", &vpf);
	}

	r = vpf(format, args);
	va_end(args);
	return r;
}
#endif

// this is a dirty monolith to reduce stub sizes
PS4ResolveStatus ps4ResolveModuleAndSymbolOrKernelSymbol(char *module, char *symbol, int *moduleId, void **userAddress, void **kernelAddress, void *function)
{
	static int (*lsm)(const char *name, size_t argc, const void *argv, unsigned int flags, int a, int b) = NULL;

	PS4ResolveStatus status;
	PS4ResolveState state;

	state.module = module;
	state.symbol = symbol;
	state.moduleId = moduleId;
	state.userAddress = userAddress;
	state.kernelAddress = kernelAddress;
	state.function = function;
	state.isKernel = ps4KernelIsInKernel();
	state.status = PS4ResolveStatusSuccess;

	if((status = ps4ResolveCallHandler(ps4ResolvePreHandler, &state, PS4ResolveStatusInterceptContinue)) != PS4ResolveStatusInterceptContinue)
		return status;

	if(state.isKernel || ps4StubType(function) == PS4StubTypeKernelFunction)
	{
		if(ps4StubType(function) == PS4StubTypeFunction)
			if((status = ps4ResolveCallHandler(ps4ResolveErrorHandler, &state, PS4ResolveStatusArgumentError)) != PS4ResolveStatusInterceptContinue)
				return status;

		if(state.symbol == NULL || state.kernelAddress == NULL)
			if((status = ps4ResolveCallHandler(ps4ResolveErrorHandler, &state, PS4ResolveStatusArgumentError)) != PS4ResolveStatusInterceptContinue)
				return status;

		*state.kernelAddress = ps4KernelDlSym(state.symbol);
		if(*state.kernelAddress == NULL)
			if((status = ps4ResolveCallHandler(ps4ResolveErrorHandler, &state, PS4ResolveStatusKernelFunctionResolveError)) != PS4ResolveStatusInterceptContinue)
				return status;

		if((status = ps4ResolveCallHandler(ps4ResolvePostHandler, &state, PS4ResolveStatusInterceptContinue)) != PS4ResolveStatusInterceptContinue)
			return status;

		return PS4ResolveStatusSuccess;
	}

	if(state.moduleId == NULL || state.userAddress == NULL || state.module == NULL || state.symbol == NULL)
		if((status = ps4ResolveCallHandler(ps4ResolveErrorHandler, &state, PS4ResolveStatusArgumentError)) != PS4ResolveStatusInterceptContinue)
			return status;

	if(*state.moduleId <= 0)
	{
		if(!lsm)
		{
			int k = 0;
			ps4ResolveSyscall(594, "libkernel.sprx", 0, &k, 0);
			if(k <= 0)
				if((status = ps4ResolveCallHandler(ps4ResolveErrorHandler, &state, PS4ResolveStatusKernelLoadError)) != PS4ResolveStatusInterceptContinue)
					return status;
			if(ps4ResolveSyscall(591, k, "sceKernelLoadStartModule", (void **)&lsm) != 0)
				if((status = ps4ResolveCallHandler(ps4ResolveErrorHandler, &state, PS4ResolveStatusLSMResolveError)) != PS4ResolveStatusInterceptContinue)
					return status;
		}
		*state.moduleId = lsm(state.module, 0, NULL, 0, 0, 0);
		if(*state.moduleId <= 0)
			if((status = ps4ResolveCallHandler(ps4ResolveErrorHandler, &state, PS4ResolveStatusModuleLoadError)) != PS4ResolveStatusInterceptContinue)
				return status;
	}

	if(ps4ResolveSyscall(591, *state.moduleId, state.symbol, state.userAddress) != 0)
		if((status = ps4ResolveCallHandler(ps4ResolveErrorHandler, &state, PS4ResolveStatusFunctionResolveError)) != PS4ResolveStatusInterceptContinue)
			return status;

	if((status = ps4ResolveCallHandler(ps4ResolvePostHandler, &state, PS4ResolveStatusInterceptContinue)) != PS4ResolveStatusInterceptContinue)
		return status;

	return PS4ResolveStatusSuccess;
}

// keep in sync with stubs layout
PS4ResolveStatus ps4Resolve(void *function)
{
	char *module = NULL;
	char *symbol = NULL;
	int *moduleId = NULL;
	void **userAddress = NULL;
	void **kernelAddress = NULL;
	void *fn = NULL;

	unsigned char *f = (unsigned char *)function;

	if(f[0] != 0xa8) // unlikely to be first
		return PS4ResolveStatusFunctionResolveError;

	switch(f[1])
	{
		case 0x00: // PS4FunctionStub
			module = *(char **)(f + 32);
			symbol = *(char **)(f + 42);
			moduleId = *(int **)(f + 52);
			userAddress = *(void ***)(f + 62);
			fn = *(void **)(f + 72);
			break;
		case 0x01: // PS4KernelFunctionStub
			symbol = *(char **)(f + 32);
			kernelAddress = *(void ***)(f + 42);
			fn = *(void **)(f + 52);
			break;
		case 0x02: // PS4FunctionAndKernelFunctionStub
			module = *(char **)(f + 64);
			symbol = *(char **)(f + 74);
			moduleId = *(int **)(f + 84);
			userAddress = *(void ***)(f + 94);
			kernelAddress = *(void ***)(f + 104);
			fn = *(void **)(f + 114);
			break;
		case 0x03: // PS4SyscallAndKernelFunctionStub
			symbol = *(char **)(f + 56);
			kernelAddress = *(void ***)(f + 66);
			fn = *(void **)(f + 76);
			break;
		case 0x04:  // PS4SyscallStub
			return PS4ResolveStatusSuccess;
		default:
			return PS4ResolveStatusFunctionResolveError;
	}

	if(fn != function)
		return PS4ResolveStatusFunctionResolveError;

	return ps4ResolveModuleAndSymbolOrKernelSymbol(module, symbol, moduleId, userAddress, kernelAddress, function);
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
