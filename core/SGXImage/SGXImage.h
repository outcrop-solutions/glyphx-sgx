// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SGXIMAGE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SGXIMAGE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include "SGxConfig.h"

#ifdef WIN32

#ifdef SGXImage_EXPORTS
#define SGXIMAGE_API __declspec(dllexport)
#else
#define SGXIMAGE_API __declspec(dllimport)
#endif

#else
#define SGXIMAGE_API
#endif