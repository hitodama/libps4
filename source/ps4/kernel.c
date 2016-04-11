#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <ps4/elfloader.h>
#include <ps4/kernel.h>
#include <ps4/internal/kernel.h>
#include <ps4/internal/kernelexploit.h>
#include <ps4/internal/kernelexploitpayload.h>
#include <ps4/internal/asmpayload.h>

#define PS4KernelElfSeekAddress (char *)0xffffffff80000000
enum{ PS4KernelElfSeekSize = 0x02000000 };
#define PS4KernelElfSeekStaticAddress (char *)0xffffffff80700000
enum{ PS4KernelElfSize = 0x00EAC180 };
enum{ PS4KernelElfPageSize = 16 * 1024 };

static int ps4KernelInternalInitialized;

static ElfSymbol *ps4KernelElfSymbols;
static char *ps4KernelElfStrings;
static char *ps4KernelExecutableAddress_;

void *ps4KernelElfSeek()
{
	char *m;
	int i;
	unsigned char elfMagic[] = {0x7f, 'E', 'L', 'F', 0x02, 0x01, 0x01, 0x09, 0x00};
	const size_t magicSize = sizeof(elfMagic) / sizeof(*elfMagic);

	m = PS4KernelElfSeekStaticAddress;

	for(i = 0; i < magicSize && m[i] == elfMagic[i]; ++i);
	if(i == magicSize)
		return m;

	for(m = PS4KernelElfSeekAddress; m < PS4KernelElfSeekAddress + PS4KernelElfSeekSize; m += PS4KernelElfPageSize)
	{
		for(i = 0; i < magicSize && m[i] == elfMagic[i]; ++i);
		if(i == magicSize)
			return m;
	}

	return NULL;
}

void ps4KernelInternalInitialize()
{
	char buf[32];
	void *kernelAddress;

	Elf *kernel;
	ElfDynamic *dyn;

	kernelAddress = ps4KernelElfSeek();
	if(kernelAddress == NULL)
		return;

	kernel = elfCreateLocalUnchecked((void *)buf, kernelAddress, PS4KernelElfSize);

	dyn = elfLoadedDynamic(kernel, NULL, ElfDynamicAttributeTag, DT_SYMTAB);
	ps4KernelElfSymbols = (ElfSymbol *)elfDynamicAttribute(dyn, ElfDynamicAttributePointer);

	dyn = elfLoadedDynamic(kernel, NULL, ElfDynamicAttributeTag, DT_STRTAB);
	ps4KernelElfStrings = (char *)elfDynamicAttribute(dyn, ElfDynamicAttributePointer);

	ps4KernelExecutableAddress_ = (char *)elfSegment(kernel, NULL, ElfSegmentAttributeType, PT_LOAD)->p_vaddr;

	ps4KernelInternalInitialized = 1;
}

void *ps4KernelExecutableMemoryMalloc(size_t size)
{
	char *a;
	//FIXME: Find neat exec malloc ...
	if(!ps4KernelInternalInitialized)
		ps4KernelInternalInitialize();
	a = ps4KernelExecutableAddress_;
	ps4KernelExecutableAddress_ += size;
	return a;
}

void ps4KernelExecutableMemoryFree(void *addr)
{
	// once we got something better
}

int ps4KernelIsInKernel()
{
	uint64_t isKernel;
	__asm__ volatile("movq %%rsp, %0" : "=r"(isKernel));
	return ((isKernel >> 48) > 0);
}

int ps4KernelRun(PS4RunnableMain fn, int argc, char **argv)
{
	if(ps4KernelIsInKernel())
		return fn(argc, argv);

	if(syscall(SYS_ps4_kernel_run, NULL) == -1)
	{
		ps4KernelExploitRun(ps4KernelExploitPayloadInstallRunSyscall, 0, NULL, NULL);
		if(syscall(SYS_ps4_kernel_run, NULL) == -1)
		{
			//*errno = EAGAIN;
			return -1;
		}
	}

	return syscall(SYS_ps4_kernel_run, fn, argc, argv);
}

int ps4KernelDlSymWrapper(int argc, char **argv)
{
	argv[0] = ps4KernelDlSym((char *)argv[1]);
	return 0;
}

void *ps4KernelDlSym(char *name)
{
	ElfSymbol *symbol;

	if(!ps4KernelIsInKernel())
	{
		char *argv[] = { NULL, name, NULL};
		ps4KernelRun(ps4KernelDlSymWrapper, 2, argv);
		return (void *)argv[0];
	}

	if(!ps4KernelInternalInitialized)
		ps4KernelInternalInitialize();

	for(symbol = ps4KernelElfSymbols; symbol + 1 < (ElfSymbol *)ps4KernelElfStrings; ++symbol)
	{
		int j;
		char *n = (char *)&ps4KernelElfStrings[symbol->st_name];
		for(j = 0; n[j] == name[j] && n[j] != 0; ++j);
		if(j > 0 && n[j] == '\0' && name[j] == '\0')
			return (void *)symbol->st_value;
	}

	return NULL;
}

void ps4KernelUARTEnable()
{
	ps4KernelRun(ps4KernelExploitPayloadUARTEnable, 0, NULL);
}

void ps4KernelGainRoot()
{
	ps4KernelRun(ps4KernelExploitPayloadGainRoot, 0, NULL);
}

void ps4KernelUnjail()
{
	ps4KernelRun(ps4KernelExploitPayloadUnjail, 0, NULL);
}

void ps4KernelEscalatePrivileges()
{
	ps4KernelRun(ps4KernelExploitPayloadEscalatePrivileges, 0, NULL);
}

void *ps4KernelMalloc(size_t size)
{
	char *argv[] = {NULL, (char *)(uintptr_t)size, NULL};
	ps4KernelRun(ps4KernelExploitPayloadMalloc, 2, argv);
	return argv[0];
}

void ps4KernelFree(void *addr)
{
	char *argv[] = {(char *)addr, NULL};
	ps4KernelRun(ps4KernelExploitPayloadFree, 1, argv);
}

void ps4KernelHookSyscall(int number, int argc, void *to)
{
	char *argv[] = {(char *)(uintptr_t)number, (char *)(uintptr_t)argc, (char *)to, NULL};
	ps4KernelRun(ps4KernelExploitPayloadHookSyscall, 2, argv);
}

void ps4KernelHookFunction(void *from, void *to)
{
	char *argv[] = {(char *)from, (char *)to, NULL};
	ps4KernelRun(ps4KernelExploitPayloadHookFunction, 2, argv);
}

void ps4KernelPatchToTruthFunction(void *function)
{
	char *argv[] = {(char *)function, NULL};
	ps4KernelRun(ps4KernelExploitPayloadPatchToTruthFunction, 1, argv);
}

/*
void ps4KernelPeek(void *kern, void *user, size_t size)
{
	char *argv[] = {(char *)kern, (char *)user, (char *)(uintptr_t)size, NULL};
	ps4KernelRun(ps4KernelExploitPayloadPeek, 3, argv);
}

void ps4KernelPoke(void *kern, void *user, size_t size)
{
	char *argv[] = {(char *)kern, (char *)user, (char *)(uintptr_t)size, NULL};
	ps4KernelRun(ps4KernelExploitPayloadPoke, 3, argv);
}
*/

void ps4KernelMemcpy(void *dest, void *src, size_t size)
{
	char *argv[] = {(char *)dest, (char *)src, (char *)(uintptr_t)size, NULL};
	ps4KernelRun(ps4KernelExploitPayloadMemcpy, 3, argv);
}
