// LicenseCPPDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include "openssl/md5.h"

#include<intrin.h>
#include "WMIService.hpp"

using namespace std;
typedef unsigned char byte;

string MD5(const string& src)
{
    MD5_CTX ctx;
    string md5_string;
    unsigned char md[16] = { 0 };
    char tmp[33] = { 0 };

    MD5_Init(&ctx);
    MD5_Update(&ctx, src.c_str(), src.size());
    MD5_Final(md, &ctx);

    for (int i = 0; i < 16; ++i)
    {
        memset(tmp, 0x00, sizeof(tmp));
        sprintf(tmp, "%02X", md[i]);
        md5_string += tmp;
    }
    return md5_string;
}

string CpuId(WMIService lWMIServ)
{
    string retVal = "";
    if (retVal == "") //If no UniqueID, use ProcessorID
    {
        retVal = lWMIServ.GetWMIInfo("Win32_Processor", "ProcessorId");
        if (retVal == "") //If no ProcessorId, use Name
        {
            retVal = lWMIServ.GetWMIInfo("Win32_Processor", "Name");
            if (retVal == "") //If no Name, use Manufacturer
            {
                retVal = lWMIServ.GetWMIInfo("Win32_Processor", "Manufacturer");
            }
            //Add clock speed for extra security
            retVal += lWMIServ.GetWMIInfo("Win32_Processor", "MaxClockSpeed");
        }
    }
    return retVal;
}

string BiosId(WMIService lWMIServ)
{
    return lWMIServ.GetWMIInfo("Win32_BIOS", "Manufacturer")
        + lWMIServ.GetWMIInfo("Win32_BIOS", "SMBIOSBIOSVersion")
        //+ identifier("Win32_BIOS", "IdentificationCode")
        + lWMIServ.GetWMIInfo("Win32_BIOS", "SerialNumber")
        + lWMIServ.GetWMIInfo("Win32_BIOS", "ReleaseDate")
        + lWMIServ.GetWMIInfo("Win32_BIOS", "Version");
}

string BaseId(WMIService lWMIServ)
{
    return //identifier("Win32_BaseBoard", "Model")+
        lWMIServ.GetWMIInfo("Win32_BaseBoard", "Manufacturer")
        + lWMIServ.GetWMIInfo("Win32_BaseBoard", "Name")
        + lWMIServ.GetWMIInfo("Win32_BaseBoard", "SerialNumber");
}

string GetMachineCode(WMIService lWMIServ)
{
    string omsg = " CPU >> " + CpuId(lWMIServ) + " BIOS >> " +
        BiosId(lWMIServ) + " BASE >> " + BaseId(lWMIServ);
    return MD5(omsg);
}

int main()
{
    WMIService wmiService;
    wmiService.WMIInit();
    cout << GetMachineCode(wmiService);
    return 0;
}