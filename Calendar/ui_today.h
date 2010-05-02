#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <lcal.h>
#include "UiImage.h"

class UiTitleBar  : public UiStatic{
public:
    void OnPaint(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		HBRUSH myBrush = CreateSolidBrush(RGB(153,204,255));
		FillRect(hdcDst,prcWin,myBrush);
        UiStatic::OnPaint(hdcDst,prcWin,prcUpdate);
    }
};

class Ui_TodayWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_TodayWnd);
public:
	Ui_TodayWnd(void);
	~Ui_TodayWnd(void);
public:
    UiToolBarPro m_Toolbar;
	UiTitleBar m_YearMonth;
	UiStatic m_BigDay;
	UiStatic m_GanZhiYear;	//干支年
	UiStatic m_LunarMonthDay;	//农历月日
	UiStatic m_GanZhiMonth;	//干支月日
	UiStatic m_WeekDayCN;	//星期（中）
	UiStatic m_WeekDayEN;	//星期（英）
    UiStatic m_Celi1;    //纪念日
    UiStatic m_Celi2;    //纪念日
    UiStatic m_Jieqi;    //节气
    UiStatic m_Yi;    //宜
    UiStatic m_Ji;    //忌
    UiImage m_Header;
    UiImage m_Spliter;
    UiImage m_Spliter_h;
private:
    void updateUi();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	int _year, _month, _day;
};
