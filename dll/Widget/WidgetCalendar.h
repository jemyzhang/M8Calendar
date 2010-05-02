#pragma once

#include <ShellWidget/ShellWidget.h>
#include <cMzCommon.h>
using namespace cMzCommon;
#include "widgetconfig.h"
#include <lcal.h>
#include <festio.h>

#define USE_MEMDC	0

extern HMODULE GetThisModule();
// Widget必须从UiWidget派生
// 根据需要，可重载StartWidget()以自定义Widget的启动行为

class UiZodiac;
class UiWidgetString;
class UiPromoteIcon;
class UiScrollDay;
class FestivalIO;

class UiWidget_Calendar: public UiWidget
{
public:
    UiWidget_Calendar();
    virtual ~UiWidget_Calendar();

    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);

    virtual int OnLButtonDown(UINT fwKeys, int xPos, int yPos);
    virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos);
    virtual int OnMouseMove(UINT fwKeys, int xPos, int yPos);
    virtual int OnLButtonDblClk(UINT fwKeys, int xPos, int yPos);
    virtual void OnPressedHoldTimeup();

    virtual bool StartWidget();
	virtual void EndWidget();

    virtual void OnCalcItemSize(__out int &xSize, __out int &ySize);

    virtual int OnTimer(UINT_PTR nIDEvent);
    void SetShowCmd(int idx);
    void updateDate(int y, int m, int d, bool all);
#if USE_MEMDC
    virtual void SetPos(int x, int y, int w, int h, UINT flags=0);
#endif
	//virtual void Invalidate(RECT* prcUpdate=NULL){
	//	reqUpdate = true;
	//	UiWidget::Invalidate(prcUpdate);
	//}

protected:
    UiWidgetString *m_pYearMonth;
	UiScrollDay *m_pBigDay;
	UiWidgetString *m_pGanZhiYear;	//干支年
	UiWidgetString *m_pLunarMonthDay;	//农历月日
	UiWidgetString *m_pGanZhiMonth;	//干支月日
	UiWidgetString *m_pWeekDayCN;	//星期（中）
	UiWidgetString *m_pWeekDayEN;	//星期（英）
    UiWidgetString *m_pCeli1;    //纪念日
    UiWidgetString *m_pCeli2;    //纪念日
    UiWidgetString *m_pJieqi;    //节气
    UiZodiac *m_pZodiac;        //生肖
    UiPromoteIcon *m_pLunarBirth;   //农历生日
    UiPromoteIcon *m_pSolarBirth;   //公历生日
    UiPromoteIcon *m_pWeeklyReminder;   //周提醒
    UiPromoteIcon *m_pMonthlyReminder;
    UiPromoteIcon *m_pCountDown;
    UiPromoteIcon *m_pCountUp;

    UiWidgetString *m_pClockHour;    //小时
    UiWidgetString *m_pClockMinute;    //分钟
    UiWidgetString *m_pClockColon;  //冒号
    void updateUi();
    void ShowDetail(FestivalType t, ReminderType rt = ReminderNone);

    void updateClock();

	void InitItems();
    void SetupItems();
    void UnloadItems();
	void DrawBackgroundDC(HDC,RECT*);
#if USE_MEMDC
	void RefreshAllItems();
#endif
private:
	int _year, _month, _day;
    int _hour,_minute;
    int ScrollDateTimeout;  //滚动日期超时
#if USE_MEMDC
    bool reqUpdate;
#endif
private:
    WidgetConfig *pconfig;
    ImagingHelper *pbkgrndimg;
	FestivalIO *pfestival;
private:
	bool SetVersionRegKey(DWORD version){
        HKEY hKEY;
        HKEY hKeyRoot = HKEY_LOCAL_MACHINE;

        long ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
        {
            RegCloseKey(hKEY);
            return false;
        }
        DWORD RegType;
        DWORD RegData;
        DWORD RegDataLen = 4;
        //写入版本信息
        if(::RegQueryValueEx(hKEY,
            L"Version",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                //不存在则新建
                DWORD ver = version;
                RegSetValueEx(hKEY,
                    L"Version",NULL,REG_DWORD,(LPBYTE)&ver,4);
                RegCloseKey(hKEY);
                return 0;
        }
        if(RegType != REG_DWORD){ //数据类型错误
            RegCloseKey(hKEY);
            return false;
        }
        RegCloseKey(hKEY);
        return true;
	}
    bool externForceRefresh;    //外部强制刷新
    int CheckExternRequest(){
        HKEY hKEY;
        HKEY hKeyRoot = HKEY_LOCAL_MACHINE;

        long ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
        {
            RegCloseKey(hKEY);
            return 0;
        }
        DWORD RegType;
        DWORD RegData;
        DWORD RegDataLen = 4;
        //2: 获取桌面插件运行请求
        if(::RegQueryValueEx(hKEY,
            L"FestRefreshReq",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                //不存在则新建
                DWORD req = 0;
                RegSetValueEx(hKEY,
                    L"FestRefreshReq",NULL,REG_DWORD,(LPBYTE)&req,4);
                RegCloseKey(hKEY);
                return 0;
        }
        if(RegType != REG_DWORD){ //数据类型错误
            RegCloseKey(hKEY);
            return 0;
        }
        RegCloseKey(hKEY);
        return RegData;
    }
    void ClearExternReqeust(){
        HKEY hKEY;
        HKEY hKeyRoot = HKEY_LOCAL_MACHINE;

        long ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
        {
            RegCloseKey(hKEY);
            return;
        }
        DWORD req = 0;
        RegSetValueEx(hKEY,
            L"FestRefreshReq",NULL,REG_DWORD,(LPBYTE)&req,4);
        RegCloseKey(hKEY);
    }
};
