// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "libs_IPNet/lib/XStream_IPNetD(x64).lib") 
#pragma comment(lib, "libs_Utilities/lib/XStream_UtilitiesD(x64).lib") 
#else
#pragma comment(lib, "libs_IPNet/lib/XStream_IPNet(x64).lib") 
#pragma comment(lib, "libs_Utilities/lib/XStream_Utilities(x64).lib") 
#endif

#pragma comment(lib, "thirdparty_libs/Winpcap/lib/x64/wpcap(x64).lib")

#else

#ifdef _DEBUG
#pragma comment(lib, "libs_IPNet/lib/XStream_IPNetD(x32).lib") 
#pragma comment(lib, "libs_Utilities/lib/XStream_UtilitiesD(x32).lib") 
#else
#pragma comment(lib, "libs_IPNet/lib/XStream_IPNet(x32).lib") 
#pragma comment(lib, "libs_Utilities/lib/XStream_Utilities(x32).lib") 
#endif

#pragma comment(lib, "thirdparty_libs/Winpcap/lib/x32/wpcap(x32).lib")
#pragma comment(lib, "thirdparty_HW/Dektec/DTAPIDLL.lib") 

#endif // _DEBUG


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

