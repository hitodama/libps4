#ifndef LibPS4InternalSyscallPayloadH
#define LibPS4InternalSyscallPayloadH

extern int ps4CallbackSyscallPayloadSize;
extern int ps4TruthFunctionPayloadSize;
extern int ps4TrampolinePayloadSize;

int ps4CallbackSyscallPayload(void *td, void *args);
int ps4TruthFunctionPayload();
int ps4TrampolinePayload();

#endif
