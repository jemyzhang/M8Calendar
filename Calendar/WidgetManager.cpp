#include "WidgetManager.h"
#include <cMzCommon.h>
using namespace cMzCommon;

static HKEY hKEY = NULL;

WidgetManager::WidgetManager(void)
{
}

WidgetManager::~WidgetManager(void)
{
}

void WidgetManager::OpenKey(){
	if(hKEY) return;

	HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
	//检测桌面插件是否已安装
	LONG ret = ::RegOpenKeyEx(hKeyRoot,
		L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
		KEY_READ,&hKEY);
	if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
	{
		hKEY = NULL;
	}
	return;
}

void WidgetManager::CloseKey(){
	if(hKEY){
		RegCloseKey(hKEY);
	}
	hKEY = NULL;
}

bool WidgetManager::IsInstalled(){
	OpenKey();
	bool ret = (hKEY != NULL);
    CloseKey();
	return ret;
}

bool WidgetManager::IsHide(){
	bool isHide = 0;
	OpenKey();
	if(hKEY)//如果无法打开hKEY,则中止程序的执行
	{
		//获取版本号
		DWORD RegType;
		DWORD RegData;
		DWORD RegDataLen = 4;
		LONG ret = ::RegQueryValueEx(hKEY,
			L"IsHide",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen);

		if(ret == ERROR_SUCCESS && RegType == REG_DWORD){
			isHide = (RegData & 0xffffffff) > 0;
		}
	}
    CloseKey();
	return isHide;
}

void WidgetManager::Hide(bool b){
	OpenKey();
	if(hKEY)//如果无法打开hKEY,则中止程序的执行
	{
		DWORD hide = b ? 1 : 0;
        RegSetValueEx(hKEY,
            L"IsHide",NULL,REG_DWORD,(LPBYTE)&hide,4);
	}
    CloseKey();
}

UINT WidgetManager::VersionNumber(){
	UINT ver = 0;
	OpenKey();
	if(hKEY)//如果无法打开hKEY,则中止程序的执行
	{
		//获取版本号
		DWORD RegType;
		DWORD RegData;
		DWORD RegDataLen = 4;
		LONG ret = ::RegQueryValueEx(hKEY,
			L"Version",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen);

		if(ret == ERROR_SUCCESS && RegType == REG_DWORD){
			ver = (UINT)(RegData & 0xffffffff);
		}
	}
    CloseKey();
	return ver;
}

void WidgetManager::CreateExternRequestKey(){
    HKEY hKeySoft;
    HKEY hKEY;
    HKEY hKeyRoot = HKEY_LOCAL_MACHINE;

    long ret = ::RegOpenKeyEx(hKeyRoot,
        L"SOFTWARE\\iDapRc\\M8Calendar",0,
        KEY_READ,&hKEY);
    if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
    {
        ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE",0,
            KEY_READ,&hKeySoft);
        DWORD dw;
        ret = ::RegCreateKeyEx(hKeySoft,L"iDapRc\\M8Calendar", 0, REG_NONE,
            REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,&hKEY,&dw);
        RegCloseKey(hKeySoft);
    }
    if(hKEY){
        DWORD RegType;
        DWORD RegData;
        DWORD RegDataLen = 4;
        if(::RegQueryValueEx(hKEY,
            L"FestRefreshReq",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                //不存在则新建
                DWORD req = 0;
                RegSetValueEx(hKEY,
                    L"FestRefreshReq",NULL,REG_DWORD,(LPBYTE)&req,4);
                RegCloseKey(hKEY);
        }
    }
    return;
}

bool WidgetManager::Install(){
	OpenKey();
	if(hKEY == NULL){
		HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
		HKEY hKeyMain = NULL;
		LONG ret = ::RegOpenKeyEx(hKeyRoot,
			L"SOFTWARE\\Meizu\\MiniOneShell\\Main",0,
			KEY_READ,&hKeyMain);
		if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
		{
			return false;
		}
		DWORD dw;
		ret = ::RegCreateKeyEx(hKeyMain,L"WidgetLunarCalendar", 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,&hKEY,&dw);
		if(ret != ERROR_SUCCESS)//如果无法创建hKEY
		{
			return false;
		}
		RegCloseKey(hKeyMain);
	}

	/*
	"ExecFileName"="\\Disk\\Programs\\Tools\\M8Calendar\\WidgetLunarCalendar.dll"
	"DefaultIcon"="\\Disk\\Programs\\Tools\\M8Calendar\\WidgetLunarCalendar.png"
	"IsWidget"=dword:00000001
	"DisplayName"="农历"
	"Height"=dword:00000002
	"Width"=dword:00000004
	"AnimateIcon"=dword:00000000
	"IsHide"=dword:00000001
	"ProgramID"="{506D1D44-8211-4cfc-939F-267AC576EA47}"
	*/
	wchar_t currpath[MAX_PATH];
	File::GetCurrentPath(currpath);

	wchar_t exename[MAX_PATH];
	wsprintf(exename,L"%s%s",currpath,L"WidgetLunarCalendar.dll");
	RegSetValueEx(hKEY,
		L"ExecFileName",	NULL,REG_SZ,(LPBYTE)&exename,(lstrlen(exename)+1)*2);
    ::RegFlushKey(hKEY);

	wchar_t icon[MAX_PATH];
	wsprintf(icon,L"%s%s",currpath,L"WidgetLunarCalendar.png");
	RegSetValueEx(hKEY,
		L"DefaultIcon",		NULL,REG_SZ,(LPBYTE)&icon,(lstrlen(icon)+1)*2);
    ::RegFlushKey(hKEY);

	DWORD iswidget = 1;
	RegSetValueEx(hKEY,
		L"IsWidget",		NULL,REG_DWORD,(LPBYTE)&iswidget,4);

	wchar_t name[8];
	wsprintf(name,L"%s",L"农历");
	RegSetValueEx(hKEY,
		L"DisplayName",		NULL,REG_SZ,(LPBYTE)&name,(lstrlen(name)+1)*2);
    ::RegFlushKey(hKEY);

	DWORD height = 2;
	RegSetValueEx(hKEY,
		L"Height",			NULL,REG_DWORD,(LPBYTE)&height,4);

	DWORD width = 4;
	RegSetValueEx(hKEY,
		L"Width",			NULL,REG_DWORD,(LPBYTE)&width,4);

	DWORD animate = 0;
	RegSetValueEx(hKEY,
		L"AnimateIcon",		NULL,REG_DWORD,(LPBYTE)&animate,4);

	DWORD ishide = 1;
	RegSetValueEx(hKEY,
		L"IsHide",			NULL,REG_DWORD,(LPBYTE)&ishide,4);

	wchar_t pgid[64];
	wsprintf(pgid,L"%s",L"{506D1D44-8211-4cfc-939F-267AC576EA47}");
	RegSetValueEx(hKEY,
		L"ProgramID",		NULL,REG_SZ,(LPBYTE)&pgid,(lstrlen(pgid)+1)*2);
    ::RegFlushKey(hKEY);

	DWORD version = 1000;
	RegSetValueEx(hKEY,
		L"Version",		NULL,REG_DWORD,(LPBYTE)&version,4);

    CloseKey();

    CreateExternRequestKey();

    return true;
}

bool WidgetManager::Uninstall(){
	if(!IsInstalled()){
		return false;
	}
	OpenKey();

	RegDeleteValue(hKEY,L"ExecFileName");
	RegDeleteValue(hKEY,L"DefaultIcon");
	RegDeleteValue(hKEY,L"IsWidget");
	RegDeleteValue(hKEY,L"DisplayName");
	RegDeleteValue(hKEY,L"Height");
	RegDeleteValue(hKEY,L"Width");
	RegDeleteValue(hKEY,L"AnimateIcon");
	RegDeleteValue(hKEY,L"IsHide");
	RegDeleteValue(hKEY,L"ProgramID");
	RegDeleteValue(hKEY,L"Version");
	CloseKey();

    HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
	HKEY hKeyMain = NULL;
	LONG ret = ::RegOpenKeyEx(hKeyRoot,
		L"SOFTWARE\\Meizu\\MiniOneShell\\Main",0,
		KEY_READ,&hKeyMain);
	if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
	{
		return false;
	}
	RegDeleteKey(hKeyMain,L"WidgetLunarCalendar");
	RegCloseKey(hKeyMain);


    return true;
}