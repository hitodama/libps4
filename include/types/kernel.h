#pragma once

#include <stdint.h>
#include <sys/time.h>

#ifndef MAP_TYPE
	#define MAP_TYPE 0x0f
#endif

typedef struct SceKernelModuleSegmentInfo
{
    void *address;
    uint32_t size;
    int32_t prot;
}
SceKernelModuleSegmentInfo;

typedef struct SceKernelModuleInfo
{
	size_t size;
	char name[256];
	SceKernelModuleSegmentInfo segmentInfo[4];
	uint32_t segmentCount;
	uint8_t fingerprint[20];
}
SceKernelModuleInfo;

typedef struct timeval SceKernelTimeval;
typedef unsigned int SceKernelUseconds;
typedef void *ScePthread;
typedef void *ScePthreadAttr;
typedef void *ScePthreadMutex;
typedef void *ScePthreadMutexattr;
typedef uint32_t SceKernelModule;

typedef int SceKernelEqueue; // why should this not be a 32 bit fd?!?
