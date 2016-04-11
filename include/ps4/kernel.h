#ifndef LibPS4KernelH
#define LibPS4KernelH

// self-made kernel stuff goes here

#include <ps4/runnable.h>

int ps4KernelRun(PS4RunnableMain fn, int argc, char **argv);
void *ps4KernelDlSym(char *name);
int ps4KernelIsInKernel();
void ps4KernelUARTEnable();
void ps4KernelGainRoot();
void ps4KernelUnjail();
void ps4KernelEscalatePrivileges();
void *ps4KernelMalloc(unsigned long size);
void ps4KernelFree(void *addr);
void ps4KernelHookSyscall(int number, int argc, void *to);
void ps4KernelHookFunction(void *from, void *to);
void ps4KernelPatchToTruthFunction(void *function);
/*void ps4KernelPeek(void *kern, void *user, size_t size);
void ps4KernelPoke(void *kern, void *user, size_t size);*/
void ps4KernelMemcpy(void *dest, void *src, unsigned long size);

#endif
