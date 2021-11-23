#include "WMIService.hpp"
#include <comutil.h>

WMIService::WMIService()
{
}

WMIService::~WMIService()
{
}

bool WMIService::WMIInit()
{
    // Initialize COM.
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        cout << "Failed to initialize COM library. "
            << "Error code = 0x"
            << hex << hres << endl;
        return false;              // Program has failed.
    }

    // Initialize 
    hres = CoInitializeSecurity(
        NULL,
        -1,      // COM negotiates service                  
        NULL,    // Authentication services
        NULL,    // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
        RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
        NULL,             // Authentication info 
        EOAC_NONE,        // Additional capabilities
        NULL              // Reserved
    );


    if (FAILED(hres))
    {
        cout << "Failed to initialize security. "
            << "Error code = 0x"
            << hex << hres << endl;
        CoUninitialize();
        return false;          // Program has failed.
    }

    // Obtain the initial locator to Windows Management
    // on a particular host computer.
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object. "
            << "Error code = 0x"
            << hex << hres << endl;
        CoUninitialize();
        return false;       // Program has failed.
    }

    // Connect to the root\cimv2 namespace with the
    // current user and obtain pointer pSvc
    // to make IWbemServices calls.

    hres = pLoc->ConnectServer(

        _bstr_t(L"ROOT\\CIMV2"), // WMI namespace
        NULL,                    // User name
        NULL,                    // User password
        0,                       // Locale
        NULL,                    // Security flags                 
        0,                       // Authority       
        0,                       // Context object
        &pSvc                    // IWbemServices proxy
    );

    if (FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x"
            << hex << hres << endl;
        pLoc->Release();
        CoUninitialize();
        return false;                // Program has failed.
    }

    cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

    // Set the IWbemServices proxy so that impersonation
    // of the user (client) occurs.
    hres = CoSetProxyBlanket(

        pSvc,                         // the proxy to set
        RPC_C_AUTHN_WINNT,            // authentication service
        RPC_C_AUTHZ_NONE,             // authorization service
        NULL,                         // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,       // authentication level
        RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
        NULL,                         // client identity 
        EOAC_NONE                     // proxy capabilities     
    );

    if (FAILED(hres))
    {
        cout << "Could not set proxy blanket. Error code = 0x"
            << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;               // Program has failed.
    }
    return true;
}

std::string WMIService::GetWMIInfo(string wmiClass, string wmiProperty)
{
    string result;
    // For example, query for all the running processes
    IEnumWbemClassObject* pEnumerator = NULL;
    string str = "SELECT * FROM " + wmiClass;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t(str.data()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        cout << "Query for processes failed. "
            << "Error code = 0x"
            << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return nullptr;               // Program has failed.
    }
    else
    {
        IWbemClassObject* pclsObj;
        ULONG uReturn = 0;

        while (pEnumerator)
        {
            hres = pEnumerator->Next(WBEM_INFINITE, 1,
                &pclsObj, &uReturn);

            if (0 == uReturn)
            {
                break;
            }

            VARIANT vtProp;

            // Get the value of the Name property
            LPCWSTR lStr = stringToLPCWSTR(wmiProperty);
            hres = pclsObj->Get(lStr, 0, &vtProp, 0, 0);
            //wcout << "Process Name : " << vtProp.bstrVal << endl;
            result = _com_util::ConvertBSTRToString(vtProp.bstrVal);
            VariantClear(&vtProp);

            pclsObj->Release();
            pclsObj = NULL;
        }
    }
    return result;
}
