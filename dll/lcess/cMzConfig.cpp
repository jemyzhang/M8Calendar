/*
* @filename appconfigini.h
* @note 应用程序配置文件读写类
* @author JEMYZHANG
* @date 2009.6.20
* @ver. 1.2
* @changelog
*  ver.1.0 初始化程序
*  ver.1.1 创建MzConfig类，简化键值读写
*  ver.1.2 初始化读取初始值时，当发现没有值时则创建，防止出现空白配置文件
*/

#include "cMzConfig.h"

#include "ReadWriteIni.h"
#include "cMzCommon.h"
using namespace cMzCommon;

#pragma comment(lib,"PlatformApi.lib")

cMzConfig::~cMzConfig(){
    if(_bupdate && checkKey()){
        if(bStrKey){
            IniWriteString((TCHAR *)_mainKey,(TCHAR *)_subKey,_valueStr,gs_inifile);
        }else{
        	IniWriteInt((TCHAR *)_mainKey,(TCHAR *)_subKey,_value,gs_inifile);
        }
    }
    if(_valueStr) delete [] _valueStr;
    if(_defaultValueStr) delete [] _defaultValueStr;
}

void cMzConfig::InitKey(LPCTSTR mainKey, LPCTSTR subKey, DWORD defaultVal){
	_mainKey = mainKey;
	_subKey = subKey;
	_defaultValue = defaultVal;
    bStrKey = false;

	//读取初始值
	_value = _defaultValue;
	if(checkKey()){
        if(!IniReadInt((TCHAR *)_mainKey,(TCHAR *)_subKey,&_value,gs_inifile)){
            IniWriteInt((TCHAR *)_mainKey,(TCHAR *)_subKey,_value,gs_inifile);
        }
	}
}

void cMzConfig::InitKey(LPCTSTR mainKey, LPCTSTR subKey, LPCTSTR defaultVal){
	_mainKey = mainKey;
	_subKey = subKey;
    bStrKey = true;

    C::newstrcpy(&_defaultValueStr,defaultVal);

	//读取初始值
    C::newstrcpy(&_valueStr, _defaultValueStr);
	if(checkKey()){
        LPTSTR v = 0;
        if(!IniReadString((TCHAR *)_mainKey,(TCHAR *)_subKey,&v,gs_inifile)){
            IniWriteString((TCHAR *)_mainKey,(TCHAR *)_subKey,_valueStr,gs_inifile);
		}else{
			C::newstrcpy(&_valueStr, v);
			delete v;
		}
	}
}

BOOL cMzConfig::Set(LPCTSTR v) {
    if(wcscmp(v,_valueStr) == 0) return FALSE;
    C::newstrcpy(&_valueStr,v);
    _bupdate = true;
    return TRUE;        
}

///////////////////////////////////////////
void AppConfigIni::SetupIniFullPath(LPTSTR iniFile){
	//setup path
	wsprintf(ini_path,L"%s",iniFile);
	gs_inifile = ini_path;
}

void AppConfigIni::SetupIniPath(LPTSTR iniFile){
	//setup path
	wchar_t currpath[128];
	if(File::GetCurrentPath(currpath)){
		if(iniFile){
			wsprintf(ini_path,L"%s\\%s",currpath,iniFile);
		}else{
			wsprintf(ini_path,L"%s\\config.ini",currpath);
		}
	}else{
		wsprintf(ini_path,L"config.ini");
	}
	gs_inifile = ini_path;
}

void AppConfigIni::CreateIni(){
	if(!File::FileExists(ini_path)){
		IniCreateFile(ini_path);
	}
}

void AppConfigIni::InitIniKey(){
	IniConfigVersion.InitKey(L"Config",L"iniVersion",100);
}