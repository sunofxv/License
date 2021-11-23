#pragma once

#include <iostream>
#include <string>
using namespace std;

// LPCWSTR -> String
static LPCWSTR stringToLPCWSTR(std::string orig)
{
    size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

    return wcstring;
}
static string GetHexString(string bt)
{
    string s = "";
    for (int i = 0; i < strlen(bt.data()); i++)
    {
        byte b = bt[i];
        int n, n1, n2;
        n = (int)b;
        n1 = n & 15;
        n2 = (n >> 4) & 15;
        if (n2 > 9)    // ((char)(n2 - 10 + (int)'A'))
            s += std::to_string(n2 - 10 + (int)'A');
        else
            s += std::to_string(n2);
        if (n1 > 9)
            s += std::to_string(n1 - 10 + (int)'A');
        else
            s += std::to_string(n1);
        if ((i + 1) != strlen(bt.data()) && (i + 1) % 2 == 0)
            s += "-";
    }
    return s;
}