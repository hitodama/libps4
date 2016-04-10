#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/mman.h>

#include <kernel.h>

#include <ps4/protectedmemory.h>
#include <ps4/inline.h>

typedef struct PS4ProtectedMemory
{
	void *writable;
	void *executable;
	size_t size;
}
PS4ProtectedMemory;

PS4ProtectedMemory *ps4ProtectedMemoryCreate(size_t size)
{
	int executableHandle, writableHandle;
	PS4ProtectedMemory *memory;
	long pageSize = sysconf(_SC_PAGESIZE);

	if(size == 0)
		return NULL;

	memory = (PS4ProtectedMemory *)malloc(sizeof(PS4ProtectedMemory));
	if(memory == NULL)
		return NULL;

	memory->size = (size / pageSize + 1) * pageSize; // align to pageSize

	sceKernelJitCreateSharedMemory(0, memory->size, PROT_READ | PROT_WRITE | PROT_EXEC, &executableHandle);
	if(executableHandle == 0)
		goto freeandreturn;
	sceKernelJitCreateAliasOfSharedMemory(executableHandle, PROT_READ | PROT_WRITE, &writableHandle);
	if(writableHandle == 0)
		goto closee;
	//sceKernelJitMapSharedMemory(memory->writableHandle, PROT_CPU_READ | PROT_CPU_WRITE, &writable);
	memory->executable = mmap(NULL, memory->size, PROT_READ | PROT_EXEC, MAP_SHARED, executableHandle, 0);
	if(memory->executable == MAP_FAILED)
		goto closew;
	memory->writable = mmap(NULL, memory->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_TYPE, writableHandle, 0);
	if(memory->writable == MAP_FAILED)
		goto munmape;

	close(executableHandle);
	close(writableHandle);

	return memory;

	munmape:
		munmap(memory->executable, memory->size);
	closew:
		close(writableHandle);
	closee:
		close(executableHandle);
	freeandreturn:
		free(memory);

	return NULL;
}

int ps4ProtectedMemoryDestroy(PS4ProtectedMemory *memory)
{
	int r = 0;
	if(memory == NULL)
		return -1;
	r |= munmap(memory->writable, memory->size);
	r |= munmap(memory->executable, memory->size);
	free(memory);
	return r;
}

void *ps4ProtectedMemoryWritableAddress(PS4ProtectedMemory *memory)
{
	if(memory == NULL)
		return NULL;
	return memory->writable;
}

void *ps4ProtectedMemoryExecutableAddress(PS4ProtectedMemory *memory)
{
	if(memory == NULL)
		return NULL;
	return memory->executable;
}

size_t ps4ProtectedMemorySize(PS4ProtectedMemory *memory)
{
	if(memory == NULL)
		return 0;
	return memory->size;
}
