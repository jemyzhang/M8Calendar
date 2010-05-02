#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <lcal.h>
#include "UiImage.h"
#include "ui_daconfig.h"

class UiBackGround : public UiWin
{
protected:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
};

class Ui_TodayDAWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_TodayDAWnd);
public:
	Ui_TodayDAWnd(void);
	~Ui_TodayDAWnd(void);
	void PaintClock(HDC hdc, int x0, int y0, int r);
    void PaintWin(HDC hdc, RECT* prcUpdate = NULL){
        if(bUpdateBgWin == true){
            bUpdateBgWin = false;
            bgWin = ::ScreenSnapshot();
        }
        ::BitmapTransBlt(hdc,prcUpdate,bgWin,&rectCopy,RGB(128,128,128));
        CMzWndEx::PaintWin(hdc,prcUpdate);
		PaintClock(hdc,ClockXPos,ClockYPos,ClockSize);
    }
    virtual BOOL Create(int xPos, int yPos, int width, int height, HWND hwndParent=NULL, int uID = NULL, DWORD style=WS_CHILD, DWORD exstyle=0){
        rectCopy.left = xPos;
        rectCopy.top = yPos;
        rectCopy.right = xPos + width;
        rectCopy.bottom = yPos + height;
        return CMzWndEx::Create(xPos,yPos,width,height,hwndParent,uID,style,exstyle);
    }
    void OnTimer(UINT_PTR nIDEvent){
        switch(nIDEvent){
            case 0x2000:
				autoExitTimer --;
				if(autoExitTimer == 0){
					::PostQuitMessage(0);
				}
                if(::GetFocus() == m_hWnd){
                    updateUi();
                }else{
					::PostQuitMessage(0);
                    //bUpdateBgWin = true;
                }
            break;
        }
    }
	LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    UiBackGround m_bg;
    UiStatic m_YearMonth;
	UiStatic m_BigDay;
	UiStatic m_GanZhiYear;	//干支年
	UiStatic m_LunarMonthDay;	//农历月日
	UiStatic m_GanZhiMonth;	//干支月日
	UiStatic m_WeekDayCN;	//星期（中）
	UiStatic m_WeekDayEN;	//星期（英）
    UiStatic m_Celi1;    //纪念日
    UiStatic m_Celi2;    //纪念日
    UiStatic m_Jieqi;    //节气
    UiImage m_Header;
    UiImage m_Spliter;
private:
    void updateUi();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();
	virtual void OnLButtonUp(UINT fwKeys, int xPos, int yPos){
		::PostQuitMessage(0);
	}

private:
	int _year, _month, _day;
    RECT rectCopy;
    HBITMAP bgWin;
    bool bUpdateBgWin;
private:
	CalendarDAConfig cfg;
	int ClockSize,ClockXPos,ClockYPos;
	COLORREF colorSec,colorMin,colorHour,colorRound;
	int autoExitTimer;	//时间到达后自动退出

	void LoadConfig(){
		ClockSize = cfg.IniClockSize.Get();
		ClockXPos = cfg.IniClockXPos.Get();
		ClockYPos = cfg.IniClockYPos.Get();
		colorSec = static_cast<COLORREF>(cfg.IniSecColor.Get());
		colorMin = static_cast<COLORREF>(cfg.IniMinColor.Get());
		colorHour = static_cast<COLORREF>(cfg.IniHourColor.Get());
		colorRound = static_cast<COLORREF>(cfg.IniRoundColor.Get());
		autoExitTimer = cfg.IniAutoTimeout.Get();
	}
};
