#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <ps4/namedmemory.h>
#include <ps4/inline.h>

typedef struct PS4NamedMemory
{
	void *memory;
	size_t size;
	char *path;
}
PS4NamedMemory;

enum{ PS4NamedMemoryPageSize = 16 * 1024 };

//FIXME: Implement Refcount and/or seperate unlink/close ?
PS4NamedMemory *ps4NamedMemoryOpen(const char *path, size_t size)
{
	int handle;
	PS4NamedMemory *memory;
	struct stat st;
	size_t fileSize;
	long pageSize = sysconf(_SC_PAGESIZE);
	size_t l = strnlen(path, 255);

	if(pageSize < 0)
		pageSize = PS4NamedMemoryPageSize;

	memory = (PS4NamedMemory *)malloc(sizeof(PS4NamedMemory));
	if(memory == NULL)
		return NULL;

	memory->path = malloc((l + 1)* sizeof(char));
	if(memory->path == NULL)
		goto freeandreturn;

	strncpy(memory->path, path, l);
	memory->path[l] = '\0';

	handle = shm_open(path, O_CREAT | O_RDWR, 0755);

	if(handle < 0)
		goto freepath;

	fileSize = 0;
	if(fstat(handle, &st) == 0)
		fileSize = (size_t)st.st_size;

	if(size == 0 && fileSize > 0)
		memory->size = ((fileSize - 1) / pageSize + 1) * pageSize;
	else if(size == 0)
		memory->size = pageSize;
	else
		memory->size = ((size - 1) / pageSize + 1) * pageSize;

	if(memory->size > fileSize && ftruncate(handle, memory->size) < 0)
		goto shmopen;

	memory->memory = mmap(NULL, memory->size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, handle, 0);
	if(memory->memory == MAP_FAILED)
		goto shmopen;

	close(handle);

	return memory;

	shmopen:
		close(handle);
		shm_unlink(memory->path);
	freepath:
		free(memory->path);
	freeandreturn:
		free(memory);

	return NULL;
}

int ps4NamedMemoryClose(PS4NamedMemory *memory)
{
	int r = 0;
	if(memory == NULL)
		return -1;
	//r |= shm_unlink(memory->path);
	r |= munmap(memory->memory, memory->size);
	free(memory->path);
	free(memory);
	return r;
}

int ps4NamedMemoryUnlink(PS4NamedMemory *memory)
{
	int r = 0;
	if(memory == NULL)
		return -1;
	r |= shm_unlink(memory->path);
	r |= ps4NamedMemoryClose(memory);
	return r;
}

void *ps4NamedMemoryMemory(PS4NamedMemory *memory)
{
	if(memory == NULL)
		return NULL;
	return memory->memory;
}

size_t ps4NamedMemorySize(PS4NamedMemory *memory)
{
	if(memory == NULL)
		return 0;
	return memory->size;
}
