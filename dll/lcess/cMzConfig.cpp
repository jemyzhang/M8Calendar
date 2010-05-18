/*
* @filename appconfigini.h
* @note Ӧ�ó��������ļ���д��
* @author JEMYZHANG
* @date 2009.6.20
* @ver. 1.2
* @changelog
*  ver.1.0 ��ʼ������
*  ver.1.1 ����MzConfig�࣬�򻯼�ֵ��д
*  ver.1.2 ��ʼ����ȡ��ʼֵʱ��������û��ֵʱ�򴴽�����ֹ���ֿհ������ļ�
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

	//��ȡ��ʼֵ
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

	//��ȡ��ʼֵ
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