#pragma once
//#include <Windows.h>
#ifdef _MSC_VER 
#define MYTHAPI _stdcall
#ifdef __cplusplus
#define HBAPI extern "C" __declspec (dllimport)
#else
#define HBAPI __declspec (dllexport)
#endif
#else
#define HBAPI
#define  MYTHAPI
#endif

HBAPI void* MYTHAPI RTMPInit(char* rtmplink);
HBAPI int MYTHAPI RTMPStart(void* ptr);
HBAPI int MYTHAPI RTMPPutH264Data(void* ptr, char* data, int len);
HBAPI int MYTHAPI RTMPClose(void* ptr);