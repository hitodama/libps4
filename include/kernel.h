#pragma once

#include "types/kernel.h"
#include "sys/types.h"

int sceKernelAllocateDirectMemory(off_t searchStart, off_t searchEnd, size_t length, size_t alignment, int type, off_t *physicalAddressDestination);
int sceKernelGetCurrentCpu(void);
int sceKernelGettimeofday(SceKernelTimeval *tp);
int sceKernelLoadStartModule(const char *name, size_t argc, const void *argv, unsigned int flags, int, int);
int sceKernelMapDirectMemory(void **addr, size_t length, int protection, int flags, off_t start, size_t alignment);
int sceKernelUsleep(unsigned int microseconds);
int scePthreadCancel(ScePthread thread);
int scePthreadCreate(ScePthread *thread, const ScePthreadAttr *attr, void *(*entry)(void *), void *arg, const char *name);
int scePthreadDetach(ScePthread thread);
int scePthreadJoin(ScePthread thread, void **value_ptr);
int scePthreadMutexDestroy(ScePthreadMutex *mutex);
int scePthreadMutexInit(ScePthreadMutex *mutex, const ScePthreadMutexattr *attr, const char *name);
int scePthreadMutexLock(ScePthreadMutex *mutex);
int scePthreadMutexTimedlock(ScePthreadMutex *mutex, SceKernelUseconds usec);
int scePthreadMutexTrylock(ScePthreadMutex *mutex);
int scePthreadMutexUnlock(ScePthreadMutex *mutex);
ScePthread scePthreadSelf(void);
uint64_t sceKernelGetProcessTime(void);
unsigned int sceKernelSleep(unsigned int seconds);
void scePthreadExit(void *value);
void scePthreadYield(void);

int sceKernelDlsym(int handle, const char *symbol, void **addr);
int sceKernelGetModuleInfo(int handle, SceKernelModuleInfo *info);
