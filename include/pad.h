#pragma once

int scePadInit(void);
int scePadOpe(int userID, int, int, void *);
int scePadClose(int handle);
int scePadRead(int handle, void *data, int count);
int scePadReadState(int handle, void *data);
