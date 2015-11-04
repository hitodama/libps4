#pragma once

#include "types/camera.h"

int sceCameraClose(int handle);
int sceCameraGetConfig(int handle, SceCameraConfig *config);
int sceCameraGetDeviceConfig(int handle, SceCameraConfig *config);
int sceCameraGetDeviceInfo(int handle, SceCameraDeviceInfo *info);
int sceCameraGetFrameData(int handle, SceCameraFrameData *frame);
int sceCameraIsAttached(int index);
int sceCameraOpen(int userid, int type, int index, void *);
int sceCameraSetConfig(int handle, SceCameraConfig *config);
int sceCameraStart(int handle, SceCameraStartParameter *param);
int sceCameraStop(int handle);

//sceCameraSetConfigInternal sceCameraProcConfigStop sceCameraDevStop sceCameraStopByHandle
