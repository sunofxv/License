#pragma once
#define _WIN32_DCOM

#include <comdef.h>
#include <Wbemidl.h>

#include "EncodeConversion.h"

#pragma comment(lib, "wbemuuid.lib")

class WMIService
{
public:
	WMIService();
	~WMIService();

	bool WMIInit();
	std::string GetWMIInfo(string wmiClass, string wmiProperty);

private:
	HRESULT hres;
	IWbemServices* pSvc = 0;
	IWbemLocator* pLoc = 0;
};

