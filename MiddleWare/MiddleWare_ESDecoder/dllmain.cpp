// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "HAL/HAL_DirectX/lib/HAL_DirectXD(x64).lib") 
#pragma comment(lib, "libs_ES/ES_MPV/lib/ES_MPVD(x64).lib") 
#pragma comment(lib, "libs_Mpeg&DVB/Mpeg_TSPacket/lib/Mpeg_TSPacketD(x64).lib") 
#pragma comment(lib, "libs_Mpeg&DVB/Mpeg_PESPacket/lib/Mpeg_PESPacketD(x64).lib") 
#else
#pragma comment(lib, "HAL/HAL_DirectX/lib/HAL_DirectXD(x32).lib") 
#pragma comment(lib, "libs_ES/ES_MPV/lib/ES_MPVD(x32).lib") 
#pragma comment(lib, "libs_Mpeg&DVB/Mpeg_TSPacket/lib/Mpeg_TSPacketD(x32).lib") 
#pragma comment(lib, "libs_Mpeg&DVB/Mpeg_PESPacket/lib/Mpeg_PESPacketD(x32).lib") 
#endif
#else
#ifdef _WIN64
#pragma comment(lib, "HAL/HAL_DirectX/lib/HAL_DirectX(x64).lib") 
#pragma comment(lib, "libs_ES/ES_MPV/lib/ES_MPV(x64).lib") 
#pragma comment(lib, "libs_Mpeg&DVB/Mpeg_TSPacket/lib/Mpeg_TSPacket(x64).lib") 
#pragma comment(lib, "libs_Mpeg&DVB/Mpeg_PESPacket/lib/Mpeg_PESPacket(x64).lib") 
#else
#pragma comment(lib, "HAL/HAL_DirectX/lib/HAL_DirectX(x32).lib") 
#pragma comment(lib, "libs_ES/ES_MPV/lib/ES_MPV(x32).lib") 
#pragma comment(lib, "libs_Mpeg&DVB/Mpeg_TSPacket/lib/Mpeg_TSPacket(x32).lib") 
#pragma comment(lib, "libs_Mpeg&DVB/Mpeg_PESPacket/lib/Mpeg_PESPacket(x32).lib") 
#endif
#endif

//#pragma comment(lib, "thirdparty_libs/directx/libs/ddraw.lib") 
//#pragma comment(lib, "thirdparty_libs/directx/libs/dsound.lib") 
//#pragma comment(lib, "thirdparty_libs/directx/libs/dxguid.lib") 
//#pragma comment(lib, "thirdparty_libs/directx/libs/quartz.lib") 
//#pragma comment(lib, "thirdparty_libs/directx/libs/strmiids.lib") 

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

