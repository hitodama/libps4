#ifndef LibPS4KernelH
#define LibPS4KernelH

// self-made kernel stuff goes here

#include <ps4/runnable.h>

int ps4KernelIsInKernel();
int ps4KernelEscalatePrivileges();
void *ps4KernelDlSym(char *name);
int ps4KernelRun(PS4RunnableMain fn, int argc, char **argv);

#endif
