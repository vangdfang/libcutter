

#ifdef WIN32
#include "stdafx.h"
#include <setupapi.h>
#include <Cfgmgr32.h>
#include <tchar.h>
#else
#include <dlfcn.h>
#include <CoreFoundation/CoreFoundation.h>
#endif
#include <iostream>
#include "LibCutterPlugin.h"


LIBCUTTERPLUGIN_API void *SCP_AllocatePlugin(char *dllFolderPath)
{
	LibCutter *ptr = new LibCutter;

	ptr->sDllFolderPath = dllFolderPath;
	return ptr;
}

LIBCUTTERPLUGIN_API void SCP_DeallocatePlugin(void *pluginPtr)
{
	if (pluginPtr) {
		delete (LibCutter*)pluginPtr;
	}

}

LIBCUTTERPLUGIN_API long SCP_GetCapabilities(void *pluginPtr)
{
	return ((LibCutter*)pluginPtr)->GetCapabilities();
}

LIBCUTTERPLUGIN_API bool SCP_FindDevice(void *pluginPtr)
{
	return ((LibCutter*)pluginPtr)->FindDevice();
}

LIBCUTTERPLUGIN_API bool SCP_OpenDevice(void *pluginPtr)
{
	return ((LibCutter*)pluginPtr)->OpenDevice();
}

LIBCUTTERPLUGIN_API bool SCP_CloseDevice(void *pluginPtr)
{
	return ((LibCutter*)pluginPtr)->CloseDevice();
}

LIBCUTTERPLUGIN_API bool SCP_Cutting_SetSpeed(void *pluginPtr, int speedUp, int speedDown)
{
	return true;
}

LIBCUTTERPLUGIN_API bool SCP_Cutting_SetPressure(void *pluginPtr, int pressure)
{
	return true;
}


LIBCUTTERPLUGIN_API bool SCP_Cutting_SetMatSize(void *pluginPtr, double widthInch, double heightInch)
{
	return true;
}


LIBCUTTERPLUGIN_API bool SCP_Cutting_Begin(void *pluginPtr)
{
	return ((LibCutter*)pluginPtr)->Cutting_Begin();
}

LIBCUTTERPLUGIN_API bool SCP_Cutting_MoveTo(void *pluginPtr, double xInch, double yInch)
{
	return ((LibCutter*)pluginPtr)->Cutting_MoveTo(xInch, yInch);
}

LIBCUTTERPLUGIN_API bool SCP_Cutting_LineTo(void *pluginPtr, double xInch, double yInch)
{
	return ((LibCutter*)pluginPtr)->Cutting_LineTo(xInch, yInch);
}

LIBCUTTERPLUGIN_API bool SCP_Cutting_CurveTo(void *pluginPtr, double cx1Inch, double cy1Inch, double cx2Inch, double cy2Inch, double xInch, double yInch)
{
	return ((LibCutter*)pluginPtr)->Cutting_CurveTo(cx1Inch, cy1Inch, cx2Inch, cy2Inch, xInch, yInch);
}

LIBCUTTERPLUGIN_API bool SCP_Cutting_End(void *pluginPtr)
{
	return ((LibCutter*)pluginPtr)->Cutting_End();
}


LibCutter::LibCutter(void) :
	pLibCutterDevice(NULL)
{

}

LibCutter::~LibCutter(void)
{
	if (pLibCutterDevice) {
		delete pLibCutterDevice;
	}
}

int GetBase(char *str)
{
	if (strstr(str, "0x")) {
		return 16;
	}
	return 10;
}

bool LibCutter::LoadKeys(void)
{
	long key_index = 0;
	std::string key_path = sDllFolderPath;
#ifdef WIN32
	key_path += "\\keys.h";
#else
	key_path += "/keys.h";
#endif
	FILE *fp = fopen(key_path.c_str(), "r");
	if (fp) {
		char str[256];
		while (fgets(str, 256, fp)) {
			char *pch = strtok(str, "\t ");
			int key_type = 0;
			while (pch) {
				if (key_type > 0) {
					switch (key_type) {
					case 1:
						moveKey0 = strtol(pch, NULL, GetBase(pch));
						break;
					case 2:
						moveKey1 = strtol(pch, NULL, GetBase(pch));
						break;
					case 3:
						moveKey2 = strtol(pch, NULL, GetBase(pch));
						break;
					case 4:
						moveKey3 = strtol(pch, NULL, GetBase(pch));
						break;
					case 5:
						lineKey0 = strtol(pch, NULL, GetBase(pch));
						break;
					case 6:
						lineKey1 = strtol(pch, NULL, GetBase(pch));
						break;
					case 7:
						lineKey2 = strtol(pch, NULL, GetBase(pch));
						break;
					case 8:
						lineKey3 = strtol(pch, NULL, GetBase(pch));
						break;
					case 9:
						curveKey0 = strtol(pch, NULL, GetBase(pch));
						break;
					case 10:
						curveKey1 = strtol(pch, NULL, GetBase(pch));
						break;
					case 11:
						curveKey2 = strtol(pch, NULL, GetBase(pch));
						break;
					case 12:
						curveKey3 = strtol(pch, NULL, GetBase(pch));
						break;
					case 13:
						if (GetBase(pch) == 16) {
							switch (key_index) {
							case 0:
								curveKey0 = strtol(pch, NULL, 16); break;
							case 1:
								curveKey1 = strtol(pch, NULL, 16); break;
							case 2:
								curveKey2 = strtol(pch, NULL, 16); break;
							case 3:
								curveKey3 = strtol(pch, NULL, 16); break;
							}
							key_index++;
						}
						break;
					case 14:
						if (GetBase(pch) == 16) {
							switch (key_index) {
							case 0:
								moveKey0 = strtol(pch, NULL, 16); break;
							case 1:
								moveKey1 = strtol(pch, NULL, 16); break;
							case 2:
								moveKey2 = strtol(pch, NULL, 16); break;
							case 3:
								moveKey3 = strtol(pch, NULL, 16); break;
							}
							key_index++;
						}
						break;
					case 15:
						if (GetBase(pch) == 16) {
							switch (key_index) {
							case 0:
								lineKey0 = strtol(pch, NULL, 16); break;
							case 1:
								lineKey1 = strtol(pch, NULL, 16); break;
							case 2:
								lineKey2 = strtol(pch, NULL, 16); break;
							case 3:
								lineKey3 = strtol(pch, NULL, 16); break;
							}
							key_index++;
						}
						break;
					}
				}
				else {
					if (0 == strcmp(pch, "MOVE_KEY_0")) {
						key_type = 1;
					}
					else if (0 == strcmp(pch, "MOVE_KEY_1")) {
						key_type = 2;
					}
					else if (0 == strcmp(pch, "MOVE_KEY_2")) {
						key_type = 3;
					}
					else if (0 == strcmp(pch, "MOVE_KEY_3")) {
						key_type = 4;
					}
					else if (0 == strcmp(pch, "LINE_KEY_0")) {
						key_type = 5;
					}
					else if (0 == strcmp(pch, "LINE_KEY_1")) {
						key_type = 6;
					}
					else if (0 == strcmp(pch, "LINE_KEY_2")) {
						key_type = 7;
					}
					else if (0 == strcmp(pch, "LINE_KEY_3")) {
						key_type = 8;
					}
					else if (0 == strcmp(pch, "CURVE_KEY_0")) {
						key_type = 9;
					}
					else if (0 == strcmp(pch, "CURVE_KEY_1")) {
						key_type = 10;
					}
					else if (0 == strcmp(pch, "CURVE_KEY_2")) {
						key_type = 11;
					}
					else if (0 == strcmp(pch, "CURVE_KEY_3")) {
						key_type = 12;
					}
					//
					else if (0 == strcmp(pch, "KEY1")) { // curve
						key_type = 13;
						key_index = 0;
					}
					else if (0 == strcmp(pch, "KEY2")) { // move
						key_type = 14;
						key_index = 0;
					}
					else if (0 == strcmp(pch, "KEY3")) { // line
						key_type = 15;
						key_index = 0;
					}

				}

				pch = strtok(NULL, "\t ");
			}
		}

		fclose(fp);
		return true;
	}
	return false;
}


long LibCutter::GetCapabilities(void)
{
	return 0;
}

#ifdef WIN32

BOOL IsNumeric(LPCSTR pszString, BOOL bIgnoreColon)
{
	long len = strlen(pszString);
	if (len == 0) {
		return FALSE;
	}

	BOOL b_numeric = TRUE;
	for (long i = 0; i<len && b_numeric; i++) {
		b_numeric = (isdigit((int)(pszString[i])) != 0);
		if (bIgnoreColon && (pszString[i] == ':')) {
			b_numeric = TRUE;
		}
	}
	return b_numeric;
}

BOOL IsNumeric(LPCWSTR pszString, BOOL bIgnoreColon)
{
	long len = wcslen(pszString);
	if (len == 0) {
		return FALSE;
	}

	BOOL b_numeric = TRUE;
	for (long i = 0; i<len && b_numeric; i++) {
		b_numeric = (iswdigit((int)(pszString[i])) != 0);
		if (bIgnoreColon && (pszString[i] == L':')) {
			b_numeric = TRUE;
		}
	}
	return b_numeric;
}
#endif

bool LibCutter::FindDevice(void)
{
	sDevPath.clear();
	long vid = 0x0403;
	long pid = 0x6001;
#ifdef WIN32
	DWORD dw_guids = 0;
	::SetupDiClassGuidsFromName(_T("Ports"), NULL, 0, &dw_guids);
	if (dw_guids == 0)
    {
		return false;
	}

	GUID *p_guids = new GUID[dw_guids];
	if (!::SetupDiClassGuidsFromName(_T("Ports"), p_guids, dw_guids, &dw_guids))
    {
		delete[] p_guids;
		return false;
	}

	HDEVINFO h_dev_info_set = SetupDiGetClassDevs(p_guids, NULL, NULL, DIGCF_PRESENT);
	if (h_dev_info_set == INVALID_HANDLE_VALUE)
    {
		delete[] p_guids;
		return false;
	}


	TCHAR vid_pid_str[64];
	_sntprintf(vid_pid_str, 64, _T("vid_%04x&pid_%04x"), vid, pid);
	TCHAR vid_pid_str2[64];
	_sntprintf(vid_pid_str2, 64, _T("vid_%04x+pid_%04x"), vid, pid);



	long com_port_number = 0;
	BOOL b_more_items = TRUE;
	long index = 0;
	SP_DEVINFO_DATA dev_info;
	while (b_more_items)
    {
		dev_info.cbSize = sizeof(SP_DEVINFO_DATA);
		b_more_items = SetupDiEnumDeviceInfo(h_dev_info_set, index, &dev_info);
		if (b_more_items)
        {
			BOOL b_added = FALSE;
			HKEY h_device_key = SetupDiOpenDevRegKey(h_dev_info_set, &dev_info, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
			if (h_device_key)
            {
				TCHAR sz_port_name[256];
				sz_port_name[0] = 0;
				DWORD dw_size = sizeof(sz_port_name);
				DWORD dw_type = 0;
				if ((RegQueryValueEx(h_device_key, _T("PortName"), NULL, &dw_type, (LPBYTE)(sz_port_name), &dw_size) == ERROR_SUCCESS) && (dw_type == REG_SZ))
                {
					long len = _tcslen(sz_port_name);
					if (len > 3) {
						if ((_tcsnicmp(sz_port_name, _T("COM"), 3) == 0) && IsNumeric(&(sz_port_name[3]), FALSE))
                        {
							long n_port = _ttoi(&(sz_port_name[3]));
							com_port_number = n_port;
							b_added = TRUE;
						}
					}
				}
				RegCloseKey(h_device_key);
			}

			if (b_added) {
				DWORD required_sz = 0;
				required_sz = 512;
				TCHAR dev_instance_str[512];
				dev_instance_str[0] = 0;
				if (SetupDiGetDeviceInstanceId(h_dev_info_set, &dev_info, dev_instance_str, required_sz, NULL))
                {
					long l = _tcslen(dev_instance_str);
					TCHAR *lowercase_str = new TCHAR[l + 1];
					TCHAR *p_dst = lowercase_str;
					TCHAR *p_src = dev_instance_str;
					while (l-- > 0)
                    {
						TCHAR c = *p_src++;
						if (_istascii(c) && _istupper(c))
                        {
							*p_dst = _tolower(c);
						}
						else {
							*p_dst = c;
						}
						++p_dst;

					}
					*p_dst = 0;


					bool b_found = (_tcsstr(lowercase_str, vid_pid_str)) != NULL;
					if (!b_found)
                    {
						b_found = (_tcsstr(lowercase_str, vid_pid_str2)) != NULL;
					}
					delete[] lowercase_str;

					if (b_found)
                    {
						char com_str[32];
						if (com_port_number > 9) {
							_sntprintf(com_str, 32, _T("\\\\.\\COM%d"), com_port_number);
						}
						else {
							_sntprintf(com_str, 32, _T("COM%d"), com_port_number);
						}

						sDevPath = com_str;

						SetupDiDestroyDeviceInfoList(h_dev_info_set);
						return true;
					}

				}
			}
		}
		++index;
	}

	SetupDiDestroyDeviceInfoList(h_dev_info_set);
	return false;
#else

	io_iterator_t serial_port_iterator;


	char bsd_path[512];


	kern_return_t kern_result;

	CFMutableDictionaryRef classes_to_match = IOServiceMatching(kIOSerialBSDServiceValue);
	kern_result = IOServiceGetMatchingServices(kIOMasterPortDefault, classes_to_match, &serial_port_iterator);
	if (KERN_SUCCESS != kern_result)
    {
		return false;
	}


	io_object_t service;
	long product_id = 0;
	long vendor_id = 0;

	while ((service = IOIteratorNext(serial_port_iterator)))
    {
		bsd_path[0] = 0;
		CFTypeRef bsd_path_as_cfstring = IORegistryEntryCreateCFProperty(service, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
		if (bsd_path_as_cfstring) {

			CFStringGetCString((CFStringRef)bsd_path_as_cfstring, bsd_path, 512, kCFStringEncodingUTF8);

			CFRelease(bsd_path_as_cfstring);

			io_registry_entry_t parent;
			kern_result = IORegistryEntryGetParentEntry(service, kIOServicePlane, &parent);
			CFTypeRef vendor_id_as_cfnumber = NULL;
			CFTypeRef product_id_as_cfnumber = NULL;
			while (kern_result == KERN_SUCCESS && !vendor_id_as_cfnumber && !product_id_as_cfnumber) {


				vendor_id_as_cfnumber = IORegistryEntrySearchCFProperty(parent, kIOServicePlane, CFSTR(kUSBVendorID), kCFAllocatorDefault, 0);
				product_id_as_cfnumber = IORegistryEntrySearchCFProperty(parent, kIOServicePlane, CFSTR(kUSBProductID), kCFAllocatorDefault, 0);

				io_registry_entry_t old_parent = parent;
				kern_result = IORegistryEntryGetParentEntry(parent, kIOServicePlane, &parent);
				IOObjectRelease(old_parent);
			}



			if (vendor_id_as_cfnumber)
            {
				SInt32 vid;
				if (CFNumberGetValue((CFNumberRef)vendor_id_as_cfnumber, kCFNumberSInt32Type, &vid)) {
					vendor_id = vid;
				}
				CFRelease(vendor_id_as_cfnumber);
			}

			if (product_id_as_cfnumber)
            {
				SInt32 pid;
				if (CFNumberGetValue((CFNumberRef)product_id_as_cfnumber, kCFNumberSInt32Type, &pid)) {
					product_id = pid;
				}
				CFRelease(product_id_as_cfnumber);

			}

			if (vid == vendor_id && pid == product_id)
            {
				sDevPath = bsd_path;
				IOObjectRelease(serial_port_iterator);
				return true;
			}

		}
	}


	IOObjectRelease(serial_port_iterator);
#endif
	return false;
}

bool LibCutter::OpenDevice(void)
{
	if (!LoadKeys()) {
		return false;
	}

	pLibCutterDevice = new Device::C(sDevPath.c_str());
	return pLibCutterDevice->is_open();
}

bool LibCutter::CloseDevice(void)
{
	return true;
}

bool LibCutter::Cutting_Begin(void)
{
	pLibCutterDevice->stop();
	pLibCutterDevice->start();

	ckey_type move_key = { moveKey0, moveKey1, moveKey2, moveKey3 };
	pLibCutterDevice->set_move_key(move_key);

	ckey_type line_key = { lineKey0, lineKey1, lineKey2, lineKey3 };
	pLibCutterDevice->set_line_key(line_key);

	ckey_type curve_key = { curveKey0, curveKey1, curveKey2, curveKey3 };
	pLibCutterDevice->set_curve_key(curve_key);

	return true;
}

bool LibCutter::Cutting_End(void)
{
	pLibCutterDevice->stop();
	return true;
}

bool LibCutter::Cutting_MoveTo(double xInch, double yInch)
{
	xy pt;

	pt.x = xInch;
	pt.y = yInch;
	pLibCutterDevice->move_to(pt);

	lastX = xInch;
	lastY = yInch;
	return true;
}

bool LibCutter::Cutting_LineTo(double xInch, double yInch)
{
	xy pt;

	pt.x = xInch;
	pt.y = yInch;
	pLibCutterDevice->cut_to(pt);

	lastX = xInch;
	lastY = yInch;

	return true;
}

bool LibCutter::Cutting_CurveTo(double cx1Inch, double cy1Inch, double cx2Inch, double cy2Inch, double xInch, double yInch)
{
	xy pt;
	xy cpt1;
	xy cpt2;
	xy pt2;
	pt.x = lastX;
	pt.y = lastY;

	cpt1.x = cx1Inch;
	cpt1.y = cy1Inch;

	cpt2.x = cx2Inch;
	cpt2.y = cy2Inch;

	pt2.x = xInch;
	pt2.y = yInch;

	pLibCutterDevice->curve_to(pt, cpt1, cpt2, pt2);

	lastX = xInch;
	lastY = yInch;
	return true;
}