#include "ui_today_da.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_calendar.h"

#include <festio.h>
extern FestivalIO *pfestival;

MZ_IMPLEMENT_DYNAMIC(Ui_TodayDAWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
//////

static const wchar_t* WeekDayNameCN[] = {
    L"星期一",
    L"星期二",
    L"星期三",
    L"星期四",
    L"星期五",
    L"星期六",
    L"星期日"
};
static const wchar_t* WeekDayNameEN[] = {
    L"MONDAY",
    L"TUESDAY",
    L"WEDNESDAY",
    L"THURSDAY",
    L"FRIDAY",
    L"SATURDAY",
    L"SUNDAY"
};
Ui_TodayDAWnd::Ui_TodayDAWnd(void)
{
	DateTime::getDate(&_year,&_month,&_day);
	LoadConfig();
}

Ui_TodayDAWnd::~Ui_TodayDAWnd(void)
{
    ::KillTimer(m_hWnd,0x2000);
}

BOOL Ui_TodayDAWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    //this->SetBgColor(RGB(128,128,128));
	m_bg.SetPos(0,0,GetWidth(),GetHeight());
	m_bg.EnableNotifyMessage(true);
	AddUiWin(&m_bg);

    // Then init the controls & other things in the window
    int y = 120;
	m_YearMonth.SetPos(0,y,300,50);
	m_YearMonth.SetTextSize(38);
    m_YearMonth.SetTextColor(RGB(255,255,255));
    m_YearMonth.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_YearMonth.SetTextWeight(FW_BOLD);
	AddUiWin(&m_YearMonth);

    y += 80;
    int backy = y;
    int width = 390*1/2;

    m_Spliter.SetPos(width - 1,y,2,300);

    TCHAR ImagePath[MAX_PATH];
	File::GetCurrentPath(ImagePath);
	wcscat(ImagePath,L"images\\spliter.png");
    m_Spliter.setupImagePath(ImagePath);

    AddUiWin(&m_Spliter);

    
    m_BigDay.SetPos(0,y,width,120);
	m_BigDay.SetTextSize(120);
    m_BigDay.SetDrawTextFormat(DT_CENTER|DT_TOP);
    m_BigDay.SetTextWeight(FW_BOLD);
	AddUiWin(&m_BigDay);

    y += 140;
	m_WeekDayCN.SetPos(0,y,width,40);
    m_WeekDayCN.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
	m_WeekDayCN.SetTextSize(32);
    m_WeekDayCN.SetTextWeight(FW_BOLD);
    m_WeekDayCN.SetTextColor(RGB(255,255,255));
	AddUiWin(&m_WeekDayCN);
    y += 40;
	m_WeekDayEN.SetPos(0,y,width,40);
    m_WeekDayEN.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
	m_WeekDayEN.SetTextSize(28);
    m_WeekDayEN.SetTextWeight(FW_BOLD);
    m_WeekDayEN.SetTextColor(RGB(255,255,255));
	AddUiWin(&m_WeekDayEN);

    int x = 390*1/2;
    y = backy;
    width = 390*1/2;
#define LINEHEIGHT  30
	m_GanZhiYear.SetPos(x,y,width,LINEHEIGHT);
	m_GanZhiYear.SetTextSize(24);
    m_GanZhiYear.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_GanZhiYear.SetTextColor(RGB(255,255,255));
	AddUiWin(&m_GanZhiYear);

    y+=LINEHEIGHT;
	m_LunarMonthDay.SetPos(x,y,width,LINEHEIGHT);
	m_LunarMonthDay.SetTextSize(24);
    m_LunarMonthDay.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_LunarMonthDay.SetTextColor(RGB(255,255,255));
	AddUiWin(&m_LunarMonthDay);
////
    y+= LINEHEIGHT + 20;
    m_Celi1.SetPos(x,y,width,LINEHEIGHT);
 	m_Celi1.SetTextSize(24);
    m_Celi1.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_Celi1.SetTextColor(RGB(255,255,255));
    AddUiWin(&m_Celi1);

    y+= LINEHEIGHT;
    m_Jieqi.SetPos(x,y,width,LINEHEIGHT);
 	m_Jieqi.SetTextSize(24);
    m_Jieqi.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_Jieqi.SetTextColor(RGB(255,255,255));
    AddUiWin(&m_Jieqi);

    y+= LINEHEIGHT;
    m_Celi2.SetPos(x,y,width,LINEHEIGHT);
 	m_Celi2.SetTextSize(24);
    m_Celi2.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_Celi2.SetTextColor(RGB(255,255,255));
    AddUiWin(&m_Celi2);
////
	y+= LINEHEIGHT + 20;
    m_GanZhiMonth.SetPos(x,y,width,LINEHEIGHT);
 	m_GanZhiMonth.SetTextSize(24);
    m_GanZhiMonth.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_GanZhiMonth.SetTextColor(RGB(255,255,255));
    AddUiWin(&m_GanZhiMonth);

	updateUi();
    bUpdateBgWin = true;
    ::SetTimer(m_hWnd,0x2000,1000,NULL);
    return TRUE;
}

void Ui_TodayDAWnd::updateUi(){
    int week = (DateTime::getWeekDay(_year,_month,_day))%7;	//获取星期

    wchar_t wstmp[128] = {0};
    wsprintf(wstmp,L"%04d年%d月",_year,_month);
    m_YearMonth.SetText(wstmp);

    wsprintf(wstmp,L"%d",_day);
    m_BigDay.SetText(wstmp);
    if(week < 5){
        m_BigDay.SetTextColor(RGB(255,255,255));
    }else{
	    m_BigDay.SetTextColor(RGB(255,64,64));
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

    Invalidate();
}

LRESULT Ui_TodayDAWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam){
	if(message == MZFC_WM_MESSAGE_QUIT){
		::PostQuitMessage(0);
	}
	return CMzWndEx::MzDefWndProc(message,wParam,lParam);
}

void Ui_TodayDAWnd::PaintClock(HDC hdc, int x0, int y0, int r){
	//获取当前时间
	SYSTEMTIME ctm;
	::GetLocalTime(&ctm);

	//计算各端点位置
#define PI 3.1415926
	int rSec = r * 9/10;	//秒针长度
	double xsinSec = (double)rSec * ::sin((double)ctm.wSecond * PI / 30.0);
	double ycosSec = (double)rSec * ::cos((double)ctm.wSecond * PI / 30.0);

	int rMin = r * 4/5; //分针长度
	double RadiansMin = (double)ctm.wMinute + (double)ctm.wSecond / 60.0;
	double xsinMin = (double)rMin * ::sin(RadiansMin * PI/30);
	double ycosMin = (double)rMin * ::cos(RadiansMin * PI/30);

	int rHour = r * 3/5; //时针长度
	double RadiansHour= (double)ctm.wHour + (double)ctm.wMinute / 60.0;
	double xsinHour = (double)rHour * ::sin(RadiansHour * PI/6);
	double ycosHour = (double)rHour * ::cos(RadiansHour * PI/6);
	//绘制秒针
	int szSec = 2;
	HBRUSH brush = CreateSolidBrush(colorSec);
	SelectObject(hdc,brush);
	HPEN pen = CreatePen(PS_SOLID, szSec,colorSec);
	SelectObject(hdc,pen);
	::MoveToEx(hdc,x0 - xsinSec / 3,y0 + ycosSec / 3,NULL);
	::LineTo(hdc,x0 + xsinSec,y0 - ycosSec);
	::DeleteObject(pen);
	::DeleteObject(brush);

	//绘制分针
	int szMin = 4;
	brush = CreateSolidBrush(colorMin);
	SelectObject(hdc,brush);
	pen = CreatePen(PS_SOLID, szMin,colorMin);
	SelectObject(hdc,pen);
	::MoveToEx(hdc,x0 - xsinMin / 3,y0 + ycosMin / 3,NULL);
	::LineTo(hdc,x0 + xsinMin,y0 - ycosMin);
	::DeleteObject(pen);
	::DeleteObject(brush);

	//绘制时针
	int szHour = 6;
	brush = CreateSolidBrush(colorHour);
	SelectObject(hdc,brush);
	pen = CreatePen(PS_SOLID, szHour,colorHour);
	SelectObject(hdc,pen);
	::MoveToEx(hdc,x0 - xsinHour / 3,y0 + ycosHour / 3,NULL);
	::LineTo(hdc,x0 + xsinHour,y0 - ycosHour);
	::DeleteObject(pen);
	::DeleteObject(brush);

	//绘制节点
	brush = CreateSolidBrush(colorRound);
	SelectObject(hdc,brush);
	pen = CreatePen(PS_SOLID, 0,colorRound);
	SelectObject(hdc,pen);
	::Ellipse(hdc,x0 - 3,y0 - 3,x0 + 3,y0 + 3);
	::DeleteObject(brush);
}

void UiBackGround::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
	UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
	//ImagingHelper img ;
	TCHAR ImagePath[MAX_PATH];
	File::GetCurrentPath(ImagePath);
	wcscat(ImagePath,L"images\\CalendarDaBg.png");
	ImagingHelper::DrawImage(hdcDst,prcWin,ImagePath);
}