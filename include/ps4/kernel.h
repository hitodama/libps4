#ifndef LibPS4KernelH
#define LibPS4KernelH

// self-made kernel stuff goes here

#include <ps4/runnable.h>

int ps4KernelRunMain(PS4RunnableMain fn, int argc, char **argv);
//int ps4KernelRun(PS4RunnableSyscall fn, void *uap);
void *ps4KernelDlSym(char *name);
int ps4KernelIsInKernel();
int ps4KernelIsKernelAddress(void *addr);
void ps4KernelUARTEnable();
void ps4KernelGainRoot();
void ps4KernelUnjail();
void ps4KernelEscalatePrivileges();
void *ps4KernelMalloc(unsigned long size);
void ps4KernelFree(void *addr);
int ps4KernelHookSyscall(int number, int argc, void *to);
int ps4KernelHookFunction(void *from, void *to);
void ps4KernelPatchToTruthFunction(void *function);
void ps4KernelMemcpy(void *dest, void *src, unsigned long size);

#endif
