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

#define PS4KernelElfSeekAddress (char *)0xffffffff80000000
enum{ PS4KernelElfSeekSize = 0x02000000 };
#define PS4KernelElfSeekStaticAddress (char *)0xffffffff80700000
enum{ PS4KernelElfSize = 0x00EAC180 };
enum{ PS4KernelElfPageSize = 16 * 1024 };

static int ps4KernelInternalInitialised;

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

void ps4KernelInternalInitialise()
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

	ps4KernelInternalInitialised = 1;
}

void *ps4KernelExecutableMemoryMalloc(size_t size)
{
	char *a;
	//FIXME: Find neat exec malloc ...
	if(!ps4KernelInternalInitialised)
		ps4KernelInternalInitialise();
	a = ps4KernelExecutableAddress_;
	ps4KernelExecutableAddress_ += size;
	return a;
}

void ps4KernelExecutableMemoryFree(void *addr)
{
	// once we got something better
}

int ps4KernelEscalatePrivileges()
{
	return ps4KernelRun(ps4KernelExploitPayloadEscalatePrivileges, 0, NULL);
}

int ps4KernelDlSymWrapper(int argc, char **argv)
{
	argv[0] = ps4KernelDlSym(argv[1]);
	return 0;
}

void *ps4KernelDlSym(char *name)
{
	ElfSymbol *symbol;

	if(!ps4KernelIsInKernel())
	{
		char *argv[3] = { NULL, name, NULL};
		ps4KernelRun(ps4KernelDlSymWrapper, 2, argv);
		return argv[0];
	}

	if(!ps4KernelInternalInitialised)
		ps4KernelInternalInitialise();

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

	if(syscall(SYS_ps4_kernel_execute, NULL) == -1)
	{
		ps4KernelExploitRun(ps4KernelExploitPayloadInstallKernelRunSyscall, 0, NULL, NULL);
		if(syscall(SYS_ps4_kernel_execute, NULL) == -1)
		{
			//*errno = EAGAIN;
			return -1;
		}
	}

	return syscall(SYS_ps4_kernel_execute, fn, argc, argv);
}
