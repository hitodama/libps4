#ifndef LibPS4InternalKernelH
#define LibPS4InternalKernelH

// self-made private kernel stuff goes here

#include <ps4/runnable.h>

void *ps4KernelExecutableMemoryMalloc(size_t size);
void ps4KernelExecutableMemoryFree(void *addr);

#endif
