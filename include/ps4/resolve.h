#ifndef LibPS4ResolveH
#define LibPS4ResolveH

typedef enum
{
	PS4ResolveStatusSuccess = 0,
	PS4ResolveStatusInterceptContinue = 1,
	PS4ResolveStatusInterceptFailure = -1,
	PS4ResolveStatusArgumentError = -10,
	PS4ResolveStatusKernelLoadError = -11,
	PS4ResolveStatusLSMResolveError = -12,
	PS4ResolveStatusModuleLoadError = -13,
	PS4ResolveStatusFunctionResolveError = -14
}
PS4ResolveStatus;

typedef PS4ResolveStatus (*PS4ResolveHandler)(char *moduleName, char *symbolName, int *module, void **symbol, PS4ResolveStatus state);

PS4ResolveHandler ps4ResolveSetErrorHandler(PS4ResolveHandler handler);
PS4ResolveHandler ps4ResolveSetPreHandler(PS4ResolveHandler handler);
PS4ResolveHandler ps4ResolveSetPostHandler(PS4ResolveHandler handler);

PS4ResolveStatus ps4Resolve(void *function);

#endif
