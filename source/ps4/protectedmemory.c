#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/mman.h>

#include <kernel.h>

#include "ps4/protectedmemory.h"
#include "ps4/inline.h"

typedef struct PS4ProtectedMemory
{
	void *writable;
	void *executable;
	int writableHandle;
	int executableHandle;
	size_t size;
}
PS4ProtectedMemory;

enum{ PS4ProtectedMemoryPageSize = 16 * 1024 };

PS4ProtectedMemory *ps4ProtectedMemoryCreate(size_t size)
{
	PS4ProtectedMemory *memory;
	long pageSize = sysconf(_SC_PAGESIZE);

	if(pageSize < 0)
		pageSize = PS4ProtectedMemoryPageSize;

	memory = (PS4ProtectedMemory *)malloc(sizeof(PS4ProtectedMemory));

	memory->size = (size / pageSize + 1) * pageSize; // align to pageSize

	sceKernelJitCreateSharedMemory(0, memory->size, PROT_READ | PROT_WRITE | PROT_EXEC, &memory->executableHandle);
	if(memory->executableHandle == 0)
		goto freeandreturn;
	sceKernelJitCreateAliasOfSharedMemory(memory->executableHandle, PROT_READ | PROT_WRITE, &memory->writableHandle);
	if(memory->writableHandle == 0)
		goto closee;
	//sceKernelJitMapSharedMemory(memory->writableHandle, PROT_CPU_READ | PROT_CPU_WRITE, &writable);
	memory->executable = mmap(NULL, memory->size, PROT_READ | PROT_EXEC, MAP_SHARED, memory->executableHandle, 0);
	if(memory->executable == NULL)
		goto closew;
	memory->writable = mmap(NULL, memory->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_TYPE, memory->writableHandle, 0);
	if(memory->writable == NULL)
		goto munmape;

	return memory;

	munmape:
		munmap(memory->executable, memory->size);
	closew:
		close(memory->writableHandle);
	closee:
		close(memory->executableHandle);
	freeandreturn:
		free(memory);

	return NULL;
}

int ps4ProtectedMemoryDestroy(PS4ProtectedMemory *memory)
{
	int r = 0;

	r |= munmap(memory->writable, memory->size);
	r |= munmap(memory->executable, memory->size);
	if(close(memory->writableHandle) == EOF)
		r = -1;
	if(close(memory->executableHandle) == EOF)
		r = -1;

	return r;
}

void *ps4ProtectedMemoryWritable(PS4ProtectedMemory *memory)
{
	return memory->writable;
}

void *ps4ProtectedMemoryExecutable(PS4ProtectedMemory *memory)
{
	return memory->executable;
}

size_t ps4ProtectedMemorySize(PS4ProtectedMemory *memory)
{
	return memory->size;
}
