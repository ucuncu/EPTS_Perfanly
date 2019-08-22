/*****************************************************************************************

 * @file	flags.h
 * @target LPC1768
 * @date	09.06.2018
 * @swrevision v0.0
 * @brief	includes flags

******************************************************************************************/
#ifndef __FLAGS_H__
#define __FLAGS_H__

#include "lpc_types.h"
#include "hlp_type.h"
#include "type.h"
#include "fat32.h"

extern BOOL fGPSdataUpdate;
extern BOOL fPowerDown;
extern BOOL fEnableDebugLog;
extern BOOL fEnableGPSDataLog;
extern BOOL fUart3to0Bride;
extern BOOL fPowerDown;
extern BOOL f10ms;
extern BOOL f100ms;
extern BOOL f500ms;
extern BOOL f1s;
extern BOOL f2s;
extern BOOL f3s;
extern BOOL f10s;
extern BOOL f1m;
extern BOOL fReadAcc;
extern BOOL fReadGyro;
extern BOOL fReadMagneto;
extern BOOL frecordJSONtoSD;
extern BOOL fshowSDCardStatus;
extern BOOL fSDCardStatus;
extern BOOL fErrorToggleStatus;
extern BOOL fSDbufferFull;
extern BOOL fResetLongPressed;
extern BOOL fInitGPS;

extern fileConfig_st *filePtr; 

#endif  /* __FLAGS_H__ */
