#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <lcal.h>
#include "UiImage.h"

#define GRID_USE_UILIST 0

class UiYiJiLabel;
class UiGrid;
class UiWeekBar;

#define CALENDAR_USE_GRID 1

class Ui_CalendarWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_CalendarWnd);
public:
	Ui_CalendarWnd(void);
	~Ui_CalendarWnd(void);
public:
    UiToolBarPro m_Toolbar;
	UiHeadingBar m_CaptionHeader;
	UiStatic m_YearMonth;
	UiWeekBar *m_pWeekBar;
	UiGrid *m_pCalendar;
	UiStatic m_LunarMD;	//农历月日
	UiStatic m_GanZhiYMD;	//干支年月日
	UiImage m_ZodiacImage;
	UiYiJiLabel *m_pTipyiji;
    MzGridMenu m_GridMenu;
    ImageContainer m_imgContainer;
	UiEdit *m_pFestDetail;
public:
	CMzString getDate();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();
	virtual void OnTimer(UINT_PTR nIDEvent);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void updateGrid();
	void updateInfo(bool forceupdate = false);
	void showTip(bool bshow = false);
	void updateFestDetail();
private:
	int _year, _month, _day;
	bool _showMonthByJieqi;
	bool _isMouseMoving;
	short _MouseX;
	short _MouseY;
	int sel_row,sel_col;
    INT UsbNotifyMsg;
};
