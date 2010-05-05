#pragma once
/*
 * @filename WidgetManager.h
 * @note 插件管理器
 * @author JEMYZHANG
 * @date 2010-5-4
 * @ver. 1.0
 * @changelog
 *  ver.1.0 初始化程序
*/
// include the MZFC library header file
#include <mzfc_inc.h>

//typedef enum tagWidgetStatus{
//	WIDGET_INSTALLED    =   0,
//	WIDGET_NOT_INSTALL  =   1,
//	WIDGET_NOT_SHOW     =   2,
//	WIDGET_OLD_VERSION  =   3,
//	WIDGET_ERR_PATH		=	4,
//	WIDGET_ERR_OTHER    =   0xff,
//}WidgetStatus;

class WidgetManager
{
private:
	WidgetManager(void);
	~WidgetManager(void);
public:
	static bool IsInstalled();
	static bool IsHide();
	static UINT VersionNumber();
	static void Hide(bool);
	static bool Install();
	static bool Uninstall();
    static void CreateExternRequestKey();
public:
	static void OpenKey();
	static void CloseKey();
};
