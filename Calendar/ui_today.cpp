#include "ui_today.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_calendar.h"
#include "ui_config.h"

#include <festio.h>
extern FestivalIO *pfestival;

MZ_IMPLEMENT_DYNAMIC(Ui_TodayWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
//////
extern CalendarConfig AppConfig;

const wchar_t* WeekDayNameCN[] = {
    L"星期一",
    L"星期二",
    L"星期三",
    L"星期四",
    L"星期五",
    L"星期六",
    L"星期日"
};
const wchar_t* WeekDayNameEN[] = {
    L"MONDAY",
    L"TUESDAY",
    L"WEDNESDAY",
    L"THURSDAY",
    L"FRIDAY",
    L"SATURDAY",
    L"SUNDAY"
};
Ui_TodayWnd::Ui_TodayWnd(void)
{
	DateTime::getDate(&_year,&_month,&_day);
}

Ui_TodayWnd::~Ui_TodayWnd(void)
{
}

BOOL Ui_TodayWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    SetWindowText(L"掌上农历-今日");
    TCHAR ImagePath[MAX_PATH];

   	m_Header.SetPos(0,0,GetWidth(),35);
	File::GetCurrentPath(ImagePath);
	wcscat(ImagePath,L"images\\header.png");
    m_Header.setupImagePath(ImagePath);
	AddUiWin(&m_Header);

    SetBgColor(RGB(255,255,255));
    // Then init the controls & other things in the window
    int y = 35;
	m_YearMonth.SetPos(0,y,GetWidth(),60);
	m_YearMonth.SetTextSize(40);
    m_YearMonth.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_YearMonth.SetTextWeight(FW_BOLD);
	AddUiWin(&m_YearMonth);

    y += 80;
    int backy = y;
    int width = GetWidth()*1/2;

    m_Spliter.SetPos(width - 1,y,2,300);
	File::GetCurrentPath(ImagePath);
	wcscat(ImagePath,L"images\\spliter.png");
    m_Spliter.setupImagePath(ImagePath);
	AddUiWin(&m_Spliter);

    
    m_BigDay.SetPos(0,y,width,160);
	m_BigDay.SetTextSize(160);
    m_BigDay.SetDrawTextFormat(DT_CENTER|DT_TOP);
    m_BigDay.SetTextWeight(FW_BOLD);
	AddUiWin(&m_BigDay);

    y += 200;
	m_WeekDayCN.SetPos(0,y,width,50);
    m_WeekDayCN.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
	m_WeekDayCN.SetTextSize(40);
    m_WeekDayCN.SetTextWeight(FW_BOLD);
	AddUiWin(&m_WeekDayCN);
    y += 50;
	m_WeekDayEN.SetPos(0,y,width,50);
    m_WeekDayEN.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
	m_WeekDayEN.SetTextSize(36);
    m_WeekDayEN.SetTextWeight(FW_BOLD);
	AddUiWin(&m_WeekDayEN);

    int x = GetWidth()*1/2;
    y = backy;
    width = GetWidth()*1/2;
#define LINEHEIGHT  50
	m_GanZhiYear.SetPos(x,y,width,LINEHEIGHT);
	m_GanZhiYear.SetTextSize(32);
    m_GanZhiYear.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
	AddUiWin(&m_GanZhiYear);

    y+=LINEHEIGHT;
	m_LunarMonthDay.SetPos(x,y,width,LINEHEIGHT);
	m_LunarMonthDay.SetTextSize(32);
    m_LunarMonthDay.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
	AddUiWin(&m_LunarMonthDay);
////
    y+= LINEHEIGHT + 20;
    m_Celi1.SetPos(x,y,width,LINEHEIGHT);
 	m_Celi1.SetTextSize(32);
    m_Celi1.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    AddUiWin(&m_Celi1);

    y+= LINEHEIGHT;
    m_Jieqi.SetPos(x,y,width,LINEHEIGHT);
 	m_Jieqi.SetTextSize(32);
    m_Jieqi.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    AddUiWin(&m_Jieqi);

    y+= LINEHEIGHT;
    m_Celi2.SetPos(x,y,width,LINEHEIGHT);
 	m_Celi2.SetTextSize(32);
    m_Celi2.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    AddUiWin(&m_Celi2);
////
	y+= LINEHEIGHT + 20;
    m_GanZhiMonth.SetPos(x,y,width,LINEHEIGHT);
 	m_GanZhiMonth.SetTextSize(32);
    m_GanZhiMonth.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    AddUiWin(&m_GanZhiMonth);

    m_Spliter_h.SetPos(0,y + 60,GetWidth(),2);
	File::GetCurrentPath(ImagePath);
	wcscat(ImagePath,L"images\\spliter_h.png");
    m_Spliter_h.setupImagePath(ImagePath);
	AddUiWin(&m_Spliter_h);

	y+= LINEHEIGHT + 30;
    width = GetWidth() - 40;
    m_Yi.SetPos(20,y,width,40);
 	m_Yi.SetTextSize(28);
    m_Yi.SetTextColor(RGB(64,255,64));
    m_Yi.SetDrawTextFormat(DT_LEFT|DT_VCENTER);
    AddUiWin(&m_Yi);

    y+= 40;
    m_Ji.SetPos(20,y,width,40);
 	m_Ji.SetTextSize(28);
    m_Ji.SetTextColor(RGB(255,64,64));
    m_Ji.SetDrawTextFormat(DT_LEFT|DT_VCENTER);
    AddUiWin(&m_Ji);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"前一天");
    if(AppConfig.IniStartupPage.Get() == 0){
        m_Toolbar.SetMiddleButton(true, true, L"返回月历",NULL,NULL,NULL);
    }else{
        m_Toolbar.SetMiddleButton(true, true, L"显示月历",NULL,NULL,NULL);
    }
    m_Toolbar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON, true, true, L"后一天");
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CALENDAR);
    AddUiWin(&m_Toolbar);

	updateUi();
    return TRUE;
}

void Ui_TodayWnd::updateUi(){
    int week = (DateTime::getWeekDay(_year,_month,_day))%7;	//获取星期

    wchar_t wstmp[128] = {0};
    wsprintf(wstmp,L"%04d年%d月",_year,_month);
    m_YearMonth.SetText(wstmp);

    wsprintf(wstmp,L"%d",_day);
    m_BigDay.SetText(wstmp);
    if(week < 5){
        m_BigDay.SetTextColor(RGB(0,0,0));
    }else{
	    m_BigDay.SetTextColor(RGB(255,0,0));
    }

    m_WeekDayCN.SetText(WeekDayNameCN[week]);
    m_WeekDayEN.SetText(WeekDayNameEN[week]);

    LCAL _lstm(_year,_month,_day);
    _lstm.SolarToLunar();
    
    wsprintf(wstmp,L"%s(%s)年",
        _lstm.GanZhiYear().C_Str(),
        _lstm.Zodiac().C_Str());
    m_GanZhiYear.SetText(wstmp);

    wsprintf(wstmp,L"%s%s",
        _lstm.LunarMonth().C_Str(),
        _lstm.OriginalLunarDay().C_Str());
    m_LunarMonthDay.SetText(wstmp);

    wchar_t holidayname[20];
    memset(holidayname,0,sizeof(holidayname));
	bool b = false;
	if(pfestival){
		if(!_lstm.isLunarLeapMonth()){
			LSDate l = _lstm.getLunarDate();
			b = pfestival->LunarHoliday(l.month,l.day,holidayname,1);
		}
	}
	if(!b){
		b = _lstm.LunarHoliday(holidayname);
	}
	if(b){
		m_Celi1.SetText(holidayname);
	}else{
		m_Celi1.SetText(L"\0");
	}

	memset(holidayname,0,sizeof(holidayname));
	b = false;

	if(pfestival){
		LSDate s = _lstm.getSolarDate();
		b = pfestival->SolarHoliday(s.month,s.day,holidayname,1);
	}
	if(!b){
		b = _lstm.SolarHoliday(holidayname);
	}
	if(b){
		m_Celi2.SetText(holidayname);
	}else{
		m_Celi2.SetText(L"\0");
	}

    LS24TERM_ptr p24term = _lstm.SolarTerm();
    if(_day < p24term[0].day){
        wsprintf(wstmp,L"%s(%d)",p24term[0].name,p24term[0].day);
    }else if(_day == p24term[0].day){
        wsprintf(wstmp,L"%s",p24term[0].name);
    }else if(_day < p24term[1].day){
        wsprintf(wstmp,L"%s(%d)",p24term[1].name,p24term[1].day);
    }else if(_day == p24term[1].day){
        wsprintf(wstmp,L"%s",p24term[1].name);
    }else{
        wsprintf(wstmp,L"%s(%d)",p24term[1].name,p24term[1].day);
    }
    m_Jieqi.SetText(wstmp);
    

    wsprintf(wstmp,L"%s月 %s日",_lstm.GanZhiMonth().C_Str(),_lstm.GanZhiDay().C_Str());
    m_GanZhiMonth.SetText(wstmp);

	CMzString yi,ji;
	bool ret = _lstm.HuangliYiJi(yi,ji);

    if(ret){
        wsprintf(wstmp,L"%s",yi.C_Str());
        m_Ji.SetText(wstmp);
        m_Yi.SetText(L"\0");
        m_Ji.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    }else{
        wsprintf(wstmp,L"宜：%s",yi.C_Str());
        m_Yi.SetText(wstmp);

        wsprintf(wstmp,L"忌：%s",ji.C_Str());
        m_Ji.SetText(wstmp);
        m_Ji.SetDrawTextFormat(DT_LEFT|DT_VCENTER);
    }

    Invalidate();
    UpdateWindow();
}

void Ui_TodayWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_CALENDAR:
        {
            int nIndex = lParam;
			if(nIndex == TOOLBARPRO_LEFT_TEXTBUTTON){	//前一天
                SYSTEMTIME s;
                s.wDay = _day; s.wMonth = _month; s.wYear = _year;
                DateTime::OneDayDate(s,true);
                _day = s.wDay; _month = s.wMonth; _year = s.wYear;
                updateUi();
				return;
			}
			if(nIndex == TOOLBARPRO_MIDDLE_TEXTBUTTON){	//返回月历
                if(AppConfig.IniStartupPage.Get() != 0){
                    Ui_CalendarWnd dlg;
                    RECT rcWork = MzGetWorkArea();
                    dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                        m_hWnd, 0, WS_POPUP);
                    // set the animation of the window
                    dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
                    dlg.SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
                    dlg.DoModal();
                }else{
                    EndModal(ID_OK);
                }
				return;
			}
			if(nIndex == TOOLBARPRO_RIGHT_TEXTBUTTON){	//后一天
                SYSTEMTIME s;
                s.wDay = _day; s.wMonth = _month; s.wYear = _year;
                DateTime::OneDayDate(s,false);
                _day = s.wDay; _month = s.wMonth; _year = s.wYear;
                updateUi();
				return;
			}
		}
	}
}
