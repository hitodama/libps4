#ifndef LibPS4InternalResolveH
#define LibPS4InternalResolveH

#include <ps4/resolve.h>

PS4ResolveStatus ps4ResolveModuleAndSymbolOrKernelSymbol(char *module, char *symbol, int *moduleId, void **userAddress, void **kernelAddress, void *function);

#endif /*LibPS4ResolveH*/
