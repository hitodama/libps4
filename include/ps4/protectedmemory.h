#ifndef LibPS4ProtectedMemoryH
#define LibPS4ProtectedMemoryH

#include <stddef.h>

typedef struct PS4ProtectedMemory PS4ProtectedMemory;

PS4ProtectedMemory *ps4ProtectedMemoryCreate(size_t size);
int ps4ProtectedMemoryDestroy(PS4ProtectedMemory *memory);

void *ps4ProtectedMemoryWritableAddress(PS4ProtectedMemory *memory);
void *ps4ProtectedMemoryExecutableAddress(PS4ProtectedMemory *memory);
size_t ps4ProtectedMemorySize(PS4ProtectedMemory *memory);

#endif
