#ifndef LibCutter_
#define LibCutter_

#include "include/serial_port.hpp"
#include "include/device_c.hpp"
#include "include/pub/keys.h"

#ifdef WIN32
#ifdef LIBCUTTERPLUGIN_EXPORTS
#define LIBCUTTERPLUGIN_API __declspec(dllexport)
#else
#define LIBCUTTERPLUGIN_API __declspec(dllimport)
#endif
#else

#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/serial/ioss.h>

#define LIBCUTTERPLUGIN_API

/* The classes below are exported */
#pragma GCC visibility push(default)

#endif

class LibCutter
{
public:
	LibCutter(void);
	~LibCutter(void);

	bool LoadKeys(void);

	long GetCapabilities(void);
	bool FindDevice(void);
	bool OpenDevice(void);
	bool CloseDevice(void);

	bool Cutting_Begin(void);
	bool Cutting_MoveTo(double xInch, double yInch);
	bool Cutting_LineTo(double xInch, double yInch);
	bool Cutting_CurveTo(double cx1Inch, double cy1Inch, double cx2Inch, double cy2Inch, double xInch, double yInch);
	bool Cutting_End(void);



	std::string sDllFolderPath;

	std::string sDevPath;
	Device::C *pLibCutterDevice;

	double lastX, lastY;

	unsigned long moveKey0, moveKey1, moveKey2, moveKey3;
	unsigned long lineKey0, lineKey1, lineKey2, lineKey3;
	unsigned long curveKey0, curveKey1, curveKey2, curveKey3;
};

extern "C" {
	LIBCUTTERPLUGIN_API void *SCP_AllocatePlugin(char *dllFolderPath);
	LIBCUTTERPLUGIN_API void SCP_DeallocatePlugin(void *pluginPtr);
	LIBCUTTERPLUGIN_API long SCP_GetCapabilities(void *pluginPtr);
	LIBCUTTERPLUGIN_API bool SCP_FindDevice(void *pluginPtr);
	LIBCUTTERPLUGIN_API bool SCP_OpenDevice(void *pluginPtr);
	LIBCUTTERPLUGIN_API bool SCP_CloseDevice(void *pluginPtr);

	LIBCUTTERPLUGIN_API bool SCP_CuttingSetSpeed(void *pluginPtr, int speedUp, int speedDown);
	LIBCUTTERPLUGIN_API bool SCP_CuttingSetPressure(void *pluginPtr, int pressure);
	LIBCUTTERPLUGIN_API bool SCP_CuttingMatSize(void *pluginPtr, double widthInch, double heightInch);

	LIBCUTTERPLUGIN_API bool SCP_Cutting_Begin(void *pluginPtr);
	LIBCUTTERPLUGIN_API bool SCP_Cutting_MoveTo(void *pluginPtr, double xInch, double yInch);
	LIBCUTTERPLUGIN_API bool SCP_Cutting_LineTo(void *pluginPtr, double xInch, double yInch);
	LIBCUTTERPLUGIN_API bool SCP_Cutting_CurveTo(void *pluginPtr, double cx1Inch, double cy1Inch, double cx2Inch, double cy2Inch, double xInch, double yInch);
	LIBCUTTERPLUGIN_API bool SCP_Cutting_End(void *pluginPtr);
}


#pragma GCC visibility pop
#endif
