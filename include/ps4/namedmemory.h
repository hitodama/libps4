#ifndef LibPS4NamedMemoryH
#define LibPS4NamedMemoryH

#include <stddef.h>

typedef struct PS4NamedMemory PS4NamedMemory;

PS4NamedMemory *ps4NamedMemoryOpen(const char *path, size_t size);
int ps4NamedMemoryClose(PS4NamedMemory *memory);
int ps4NamedMemoryUnlink(PS4NamedMemory *memory);

void *ps4NamedMemoryMemory(PS4NamedMemory *memory);
size_t ps4NamedMemorySize(PS4NamedMemory *memory);

#endif
