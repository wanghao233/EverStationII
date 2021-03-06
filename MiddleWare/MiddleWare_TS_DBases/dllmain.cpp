// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "HAL/HAL_XML/lib/HAL_XMLD(x64).lib")

#pragma comment(lib, "libs_Mpeg&DVB/MPEG_TSPacket/lib/MPEG_TSPacketD(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PESPacket/lib/MPEG_PESPacketD(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_DVB_Section/lib/MPEG_DVB_SectionD(x64).lib")
#pragma comment(lib, "libs_Utilities/lib/XStream_UtilitiesD(x64).lib")

#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTableD(x64).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/thirdparty_sqlite3D(x64).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/thirdparty_tinyxml2D(x64).lib")

#else
#pragma comment(lib, "HAL/HAL_XML/lib/HAL_XML(x64).lib")

#pragma comment(lib, "libs_Mpeg&DVB/MPEG_TSPacket/lib/MPEG_TSPacket(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PESPacket/lib/MPEG_PESPacket(x64).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_DVB_Section/lib/MPEG_DVB_Section(x64).lib")
#pragma comment(lib, "libs_Utilities/lib/XStream_Utilities(x64).lib")

#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTable(x64).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/thirdparty_sqlite3(x64).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/thirdparty_tinyxml2(x64).lib")

#endif

#else

#ifdef _DEBUG
#pragma comment(lib, "HAL/HAL_XML/lib/HAL_XMLD(x32).lib")

#pragma comment(lib, "libs_Mpeg&DVB/MPEG_TSPacket/lib/MPEG_TSPacketD(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PESPacket/lib/MPEG_PESPacketD(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_DVB_Section/lib/MPEG_DVB_SectionD(x32).lib")
#pragma comment(lib, "libs_Utilities/lib/XStream_UtilitiesD(x32).lib")

#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTableD(x32).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/thirdparty_sqlite3D(x32).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/thirdparty_tinyxml2D(x32).lib")

#else

#pragma comment(lib, "HAL/HAL_XML/lib/HAL_XML(x32).lib")

#pragma comment(lib, "libs_Mpeg&DVB/MPEG_TSPacket/lib/MPEG_TSPacket(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_PESPacket/lib/MPEG_PESPacket(x32).lib")
#pragma comment(lib, "libs_Mpeg&DVB/MPEG_DVB_Section/lib/MPEG_DVB_Section(x32).lib")
#pragma comment(lib, "libs_Utilities/lib/XStream_Utilities(x32).lib")

#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTable(x32).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/thirdparty_sqlite3(x32).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/thirdparty_tinyxml2(x32).lib")
#endif

#endif

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

