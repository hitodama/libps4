#ifndef LibPS4RunnableH
#define LibPS4RunnableH

typedef int (*PS4RunnableMain)(int argc, char **argv);
typedef int (*PS4RunnableSyscall)(void *td, void *uap);
typedef void *(*PS4RunnableThread)(void *arg);
typedef void (*PS4Runnable)(void);

#endif
