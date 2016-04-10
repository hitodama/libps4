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
	PS4ResolveStatusFunctionResolveError = -14,
	PS4ResolveStatusKernelFunctionResolveError = -15
}
PS4ResolveStatus;

typedef struct
{
	char *module;
	char *symbol;
	int *moduleId;
	void **address;
	void **kernelAddress;
	int isKernel;
	PS4ResolveStatus status;
}
PS4ResolveState;

typedef PS4ResolveStatus (*PS4ResolveHandler)(PS4ResolveState *state);

PS4ResolveHandler ps4ResolveSetErrorHandler(PS4ResolveHandler handler);
PS4ResolveHandler ps4ResolveSetPreHandler(PS4ResolveHandler handler);
PS4ResolveHandler ps4ResolveSetPostHandler(PS4ResolveHandler handler);

PS4ResolveStatus ps4Resolve(void *function);

#endif
