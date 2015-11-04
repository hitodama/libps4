#pragma once

#include "stdint.h"
#include "sys/time.h"

typedef struct SceKernelModuleInfo {
	size_t size; // 0x0
	char name[32]; // 0x8
	char padding1[0xe0]; // 0x28
	void *codeBase; // 0x108
	unsigned int codeSize; // 0x110
	void *dataBase; // 0x118
	unsigned int dataSize; // 0x120
	char padding2[0x3c]; // 0x124
} SceKernelModuleInfo;

typedef struct timeval SceKernelTimeval;
typedef unsigned int SceKernelUseconds;
typedef void *ScePthread;
typedef void *ScePthreadAttr;
typedef void *ScePthreadMutex;
typedef void *ScePthreadMutexattr;
typedef uint32_t SceKernelModule;
