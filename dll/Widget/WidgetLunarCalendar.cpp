// WidgetClock.cpp : 定义 DLL 应用程序的入口点。
//

#include "WidgetLunarCalendar.h"
#include <windows.h>

#include "WidgetCalendar.h"

HMODULE this_dll_module = 0;

UiWidget* CreateWidgetInstance( void* lpVoid )
{
    UiWidget* pWidget = new UiWidget_Calendar;
    return pWidget;
}

HMODULE GetThisModule(){
    return this_dll_module;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
    this_dll_module = (HMODULE)hModule;
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
