#pragma once

#include <ShellWidget/ShellWidget.h>
#include <cMzCommon.h>
using namespace cMzCommon;
#include "widgetconfig.h"
#include <lcal.h>
#include <festio.h>

#define USE_MEMDC	0

extern HMODULE GetThisModule();
// Widget�����UiWidget����
// ������Ҫ��������StartWidget()���Զ���Widget��������Ϊ

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
	UiWidgetString *m_pGanZhiYear;	//��֧��
	UiWidgetString *m_pLunarMonthDay;	//ũ������
	UiWidgetString *m_pGanZhiMonth;	//��֧����
	UiWidgetString *m_pWeekDayCN;	//���ڣ��У�
	UiWidgetString *m_pWeekDayEN;	//���ڣ�Ӣ��
    UiWidgetString *m_pCeli1;    //������
    UiWidgetString *m_pCeli2;    //������
    UiWidgetString *m_pJieqi;    //����
    UiZodiac *m_pZodiac;        //��Ф
    UiPromoteIcon *m_pLunarBirth;   //ũ������
    UiPromoteIcon *m_pSolarBirth;   //��������
    UiPromoteIcon *m_pWeeklyReminder;   //������
    UiPromoteIcon *m_pMonthlyReminder;
    UiPromoteIcon *m_pCountDown;
    UiPromoteIcon *m_pCountUp;

    UiWidgetString *m_pClockHour;    //Сʱ
    UiWidgetString *m_pClockMinute;    //����
    UiWidgetString *m_pClockColon;  //ð��
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
    int ScrollDateTimeout;  //�������ڳ�ʱ
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
        if(ret != ERROR_SUCCESS)//����޷���hKEY,����ֹ�����ִ��
        {
            RegCloseKey(hKEY);
            return false;
        }
        DWORD RegType;
        DWORD RegData;
        DWORD RegDataLen = 4;
        //д��汾��Ϣ
        if(::RegQueryValueEx(hKEY,
            L"Version",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                //���������½�
                DWORD ver = version;
                RegSetValueEx(hKEY,
                    L"Version",NULL,REG_DWORD,(LPBYTE)&ver,4);
                RegCloseKey(hKEY);
                return 0;
        }
        if(RegType != REG_DWORD){ //�������ʹ���
            RegCloseKey(hKEY);
            return false;
        }
        RegCloseKey(hKEY);
        return true;
	}
    bool externForceRefresh;    //�ⲿǿ��ˢ��
    int CheckExternRequest(){
        HKEY hKEY;
        HKEY hKeyRoot = HKEY_LOCAL_MACHINE;

        long ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//����޷���hKEY,����ֹ�����ִ��
        {
            RegCloseKey(hKEY);
            return 0;
        }
        DWORD RegType;
        DWORD RegData;
        DWORD RegDataLen = 4;
        //2: ��ȡ��������������
        if(::RegQueryValueEx(hKEY,
            L"FestRefreshReq",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                //���������½�
                DWORD req = 0;
                RegSetValueEx(hKEY,
                    L"FestRefreshReq",NULL,REG_DWORD,(LPBYTE)&req,4);
                RegCloseKey(hKEY);
                return 0;
        }
        if(RegType != REG_DWORD){ //�������ʹ���
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
        if(ret != ERROR_SUCCESS)//����޷���hKEY,����ֹ�����ִ��
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
