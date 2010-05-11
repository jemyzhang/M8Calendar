#include "ui_today.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_calendar.h"
#include "ui_config.h"

#include <festio.h>
extern FestivalIO *pfestival;

#include "UiFestivalInfo.h"

#define MSG_UPDATE_DATE 0xFFF20000

///////////////////////////////////////////////////
class UiScrollDay : public UiWin {
public:
    UiScrollDay(){
        DateTime::getDate(&_year1,&_month1,&_day1);
        _year2 = _year1; _month2 = _month1; _day2 = _day1;
        bPressed = false;
        speed = 0;
        scrolled = 0;
        scrolling = false;
        scrollend = false;
    }
    ~UiScrollDay(){
    }
public:
    void ResetScrolling(){
        scrolling = false;
        scrollend = false;
        DateTime::getDate(&_year1,&_month1,&_day1);
        _year2 = _year1; _month2 = _month1; _day2 = _day1;
    }
private:
    int x,y,w,h;
public:
    virtual void SetPos(int x, int y, int w, int h, UINT flags=0){
        UiWin::SetPos(x,y,w,h,flags);
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    //
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        if(!scrolling){
            ::SetBkMode(hdcDst,TRANSPARENT);
            HFONT font = FontHelper::GetFont(GetTextSize(),GetTextWeight());
            HFONT oldfont = (HFONT)SelectObject(hdcDst,font);

            ::SetTextColor(hdcDst,GetTextColor());
            DrawText(hdcDst,GetText().C_Str(),GetText().Length(),prcWin,GetDrawTextFormat());
            SelectObject(hdcDst,oldfont);//恢复系统字体
            return;
        }

        if(scrollend){
            ::SetBkMode(hdcDst,TRANSPARENT);
            HFONT font = FontHelper::GetFont(GetTextSize(),GetTextWeight());
            HFONT oldfont = (HFONT)SelectObject(hdcDst,font);

            ::SetTextColor(hdcDst,GetTextColor());
            wchar_t strval[10] = {0};
            swprintf(strval,L"%d",_day);
            DrawText(hdcDst,strval,lstrlen(strval),prcWin,GetDrawTextFormat());
            SelectObject(hdcDst,oldfont);//恢复系统字体
        }else{
            HDC pScrollDC = CreateCompatibleDC(hdcDst);
            HBITMAP pScrollBitmap = CreateCompatibleBitmap(hdcDst,w,h * 2);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(pScrollDC, pScrollBitmap);

            RECT rcScroll = {0,0,w,h * 2};
            HBRUSH myBrush = CreateSolidBrush(RGB(255-64,255-64,255-64));
            FillRect(pScrollDC,&rcScroll,myBrush);	//clear
            DeleteObject(myBrush);

            SetBkMode(pScrollDC,TRANSPARENT);

            HFONT font = FontHelper::GetFont(GetTextSize(),GetTextWeight());
            HFONT oldfont = (HFONT)SelectObject(pScrollDC,font);
            ::SetTextColor(pScrollDC,RGB(128,128,128));

            rcScroll.bottom = h;
            wchar_t strval[10] = {0};
            swprintf(strval,L"%d",_day1);
            DrawText(pScrollDC,strval,lstrlen(strval),&rcScroll,GetDrawTextFormat());

            rcScroll.top = h;
            rcScroll.bottom = h*2;
            swprintf(strval,L"%d",_day2);
            DrawText(pScrollDC,strval,lstrlen(strval),&rcScroll,GetDrawTextFormat());

            int offset = scrolled;
            if(offset < 0){
                offset = 0 - offset;
            }else{
                offset = h - offset;
            }
            ::TransparentBlt(hdcDst,prcWin->left,prcWin->top,RECT_WIDTH(*prcWin),RECT_HEIGHT(*prcWin),
                pScrollDC,0,offset,RECT_WIDTH(*prcWin),RECT_HEIGHT(*prcWin),RGB(255-64,255-64,255-64));

            SelectObject(hdcDst,oldfont);//恢复系统字体
            SelectObject(pScrollDC, hOldBitmap); //恢复系统BITMAP
            ::DeleteObject(pScrollBitmap);
            ::DeleteDC(pScrollDC);
        }
    }
    //
    virtual int OnLButtonDown(UINT fwKeys, int xPos, int yPos){
        int iRet = UiWin::OnLButtonDown(fwKeys,xPos,yPos);
        if(!bPressed){
            bPressed = true;
            syPos = yPos;
            speedPos = yPos;
            t_scrolled = 0;
            scrolled = 0;
            direction = 0;
            tickcount = ::GetTickCount();
            scrolling = true;
            scrollend = false;
            iRet = true;
        }
        return iRet;
    }
    virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
        int iRet = UiWin::OnLButtonUp(fwKeys,xPos,yPos);
        if(bPressed){
            if(direction > 0){ //向下
                speed = (yPos - speedPos)*10 / (GetTickCount() - tickcount);
            }else{  //向上
                speed = (speedPos - yPos)*10 / (GetTickCount() - tickcount);
            }

            if(speed < 10) speed = 10;
            bPressed = false;
            //start auto scroll
            ::SetTimer(GetParentWnd(),GetID(),50,NULL);
            iRet = 1;
        }
        return iRet;
    }
    virtual int OnMouseMove(UINT fwKeys, int xPos, int yPos){
        int iRet = UiWin::OnMouseMove(fwKeys,xPos,yPos);
        if(bPressed){
            onMove(yPos);
            iRet = 1;
        }
        return iRet;
    }
    virtual int OnTimer(UINT_PTR nIDEvent){
        if(nIDEvent == GetID()){
            bool stopped = false;
            bool usey1 = false;
            if(speed > 4){
                speed --;
            }
            if(direction > 0){
                if(scrolled < GetHeight() / 2){
                    scrolled -= speed;
                    if(scrolled < 0){   //向上滚回year2
                        stopped = true;
                    }
                }else{
                    scrolled += speed; //向下滚至year1
                    if(scrolled > GetHeight()){
                        usey1 = true;
                        stopped = true;
                    }
                }
            }else{
                if(scrolled * (-1) < GetHeight() / 2){
                    scrolled += speed; //向下滚至year1
                    if(scrolled > 0){
                        usey1 = true;
                        stopped = true;
                    }
                }else{
                    scrolled -= speed; //向上滚回year2
                    if(scrolled < (-1)*GetHeight()){
                        stopped = true;                        
                    }
                }
            }
            if(stopped){
                ::KillTimer(GetParentWnd(),GetID());
                if(usey1){
                    _year2 = _year1; _month2 = _month1; _day2 = _day1;
                    updateParent(_year1,_month1,_day1,true);
                }else{
                    _year1 = _year2; _month1 = _month2; _day1 = _day2;
                    updateParent(_year2,_month2,_day2,true);
                }
            }else{
                Invalidate();
            }
        }
        return 0;
    }
private:
    int syPos;  //初始
    int speed;  //滚动速度
    bool bPressed;
    int scrolltime;
    DWORD tickcount;
    int t_scrolled;
    int scrolled;
    int direction;  //0: no move 1: down -1: up
    bool scrolling;
    int speedPos;   //初速度位置
    bool scrollend;
private:
    void updateParent(int y, int m, int d, bool b){
        //TODO: send message
        WPARAM wparam = MAKEWPARAM(d,m);
        LPARAM lparam = MAKELPARAM(b,y);
        ::PostMessage(GetParentWnd(),MSG_UPDATE_DATE,wparam,lparam);
        if(b){
            scrollend = true;
            _day = d;
        }
        Invalidate();
    }
    void onMove(int yPos){
        int tdis = (yPos - syPos) * 2; //倍速
        if(direction == 0){ //刚开始移动
            if(tdis > 0){  //向下移动
                direction = 1;
                SYSTEMTIME t;
                _year2 = _year1; _month2 = _month1; _day2 = _day1;
                t.wYear = _year1; t.wMonth = _month1; t.wDay = _day1;
                DateTime::OneDayDate(t);
                _year1 = t.wYear; _month1 = t.wMonth; _day1 = t.wDay;
            }else if(tdis < 0){ //向上移动
                direction = -1;
                SYSTEMTIME t;
                _year1 = _year2; _month1 = _month2; _day1 = _day2;
                t.wYear = _year2; t.wMonth = _month2; t.wDay = _day2;
                DateTime::OneDayDate(t,true);
                _year2 = t.wYear; _month2 = t.wMonth; _day2 = t.wDay;
            }
        }else{
            if(direction > 0){  //向下移动
                if(t_scrolled > tdis){  //反向移动
                    speedPos = yPos;
                    direction = -1;
                }else{
                    if(scrolled > tdis % GetHeight()){
                        //超过一个height高度，日期后移
                        _year2 = _year1; _month2 = _month1; _day2 = _day1;
                        SYSTEMTIME t;
                        t.wYear = _year1; t.wMonth = _month1; t.wDay = _day1;
                        DateTime::OneDayDate(t);
                        _year1 = t.wYear; _month1 = t.wMonth; _day1 = t.wDay;
                        updateParent(_year2,_month2,_day2,false);
                    }
                }
            }else{  //向上移动
                if(t_scrolled < tdis){  //反向移动
                    speedPos = yPos;
                    direction = 1;
                }else{
                    if(scrolled < tdis % GetHeight()){
                        //超过一个height高度，日期前移
                        _year1 = _year2; _month1 = _month2; _day1 = _day2;
                        SYSTEMTIME t;
                        t.wYear = _year2; t.wMonth = _month2; t.wDay = _day2;
                        DateTime::OneDayDate(t,true);
                        _year2 = t.wYear; _month2 = t.wMonth; _day2 = t.wDay;
                        updateParent(_year1,_month1,_day1,false);
                    }
                }
            }
        }
        t_scrolled = tdis;
        scrolled = t_scrolled % GetHeight();
        Invalidate();
        //printf("d1:%d,d2:%d,scrolled:%d,tdis:%d,dir:%d\n",
        //    _day1,_day2,scrolled,tdis,direction);
    }
private:
    int _year1;
    int _month1;
    int _day1;
    int _year2;
    int _month2;
    int _day2;
    int _day;
};

///////////////////////////////////////////////////
MZ_IMPLEMENT_DYNAMIC(Ui_TodayWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
#define MZ_IDC_SCROLL_DAY 102
#define MZ_IDC_FEST_INFO 103
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
    INIT_PTR(m_pBigDay);
    INIT_PTR(m_pFestDetail);
}

Ui_TodayWnd::~Ui_TodayWnd(void)
{
    DEL_PTR(m_pBigDay);
    DEL_PTR(m_pFestDetail);
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

    m_pBigDay = new UiScrollDay();
    m_pBigDay->SetPos(0,y,width,160);
    m_pBigDay->SetTextSize(160);
    m_pBigDay->SetDrawTextFormat(DT_CENTER|DT_TOP);
    m_pBigDay->SetTextWeight(FW_BOLD);
    m_pBigDay->SetID(MZ_IDC_SCROLL_DAY);
    m_pBigDay->EnableNotifyMessage(true);
    AddUiWin(m_pBigDay);

    y += 200;
    m_WeekDayCN.SetPos(0,y,width,50);
    m_WeekDayCN.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_WeekDayCN.SetTextSize(40);
    m_WeekDayCN.SetTextWeight(FW_BOLD);
    AddUiWin(&m_WeekDayCN);
    y += 50;
    m_WeekDayEN.SetPos(0,y,width,50);
    m_WeekDayEN.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    m_WeekDayEN.SetTextSize(32);
    m_WeekDayEN.SetTextWeight(FW_BOLD);
    AddUiWin(&m_WeekDayEN);

    int x = GetWidth()*1/2;
    y = backy;
    width = GetWidth()*1/2;
#define LINEHEIGHT  46
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
    y+= LINEHEIGHT + 10;
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
    y+= LINEHEIGHT + 10;
    m_GanZhiMonth.SetPos(x,y,width,LINEHEIGHT);
    m_GanZhiMonth.SetTextSize(32);
    m_GanZhiMonth.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    AddUiWin(&m_GanZhiMonth);

    //m_Spliter_h.SetPos(0,y + 60,GetWidth(),2);
    //File::GetCurrentPath(ImagePath);
    //wcscat(ImagePath,L"images\\spliter_h.png");
    //m_Spliter_h.setupImagePath(ImagePath);
    //AddUiWin(&m_Spliter_h);

    y+= LINEHEIGHT + 10;
	m_pFestDetail = new UiFestivalInfo;
    m_pFestDetail->SetPos(0,y,GetWidth(),GetHeight() - MZM_HEIGHT_TOOLBARPRO - y);
	m_pFestDetail->SetTextSize(24);
    m_pFestDetail->SetTextWeight(FW_BOLD);
	m_pFestDetail->SetID(MZ_IDC_FEST_INFO);
	AddUiWin(m_pFestDetail);

    //width = GetWidth() - 40;
    //m_Yi.SetPos(20,y,width,40);
    //m_Yi.SetTextSize(28);
    //m_Yi.SetTextColor(RGB(64,255,64));
    //m_Yi.SetDrawTextFormat(DT_LEFT|DT_VCENTER);
    //AddUiWin(&m_Yi);

    //y+= 40;
    //m_Ji.SetPos(20,y,width,40);
    //m_Ji.SetTextSize(28);
    //m_Ji.SetTextColor(RGB(255,64,64));
    //m_Ji.SetDrawTextFormat(DT_LEFT|DT_VCENTER);
    //AddUiWin(&m_Ji);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    if(AppConfig.IniStartupPage.Get() == 0){
        m_Toolbar.SetMiddleButton(true, true, L"返回月历",NULL,NULL,NULL);
    }else{
        m_Toolbar.SetMiddleButton(true, true, L"显示月历",NULL,NULL,NULL);
    }
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
    m_pBigDay->SetText(wstmp);
    if(week < 5){
        m_pBigDay->SetTextColor(RGB(0,0,0));
    }else{
        m_pBigDay->SetTextColor(RGB(255,0,0));
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

    bool b = _lstm.LunarHoliday(holidayname);
    if(b){
        m_Celi1.SetText(holidayname);
    }else{
        m_Celi1.SetText(L"\0");
    }

    memset(holidayname,0,sizeof(holidayname));

    b = _lstm.SolarHoliday(holidayname);
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

    //CMzString yi,ji;
    //bool ret = _lstm.HuangliYiJi(yi,ji);

    //if(ret){
    //    wsprintf(wstmp,L"%s",yi.C_Str());
    //    m_Ji.SetText(wstmp);
    //    m_Yi.SetText(L"\0");
    //    m_Ji.SetDrawTextFormat(DT_CENTER|DT_VCENTER);
    //}else{
    //    wsprintf(wstmp,L"宜：%s",yi.C_Str());
    //    m_Yi.SetText(wstmp);

    //    wsprintf(wstmp,L"忌：%s",ji.C_Str());
    //    m_Ji.SetText(wstmp);
    //    m_Ji.SetDrawTextFormat(DT_LEFT|DT_VCENTER);
    //}

    m_pFestDetail->SetDate(_year,_month,_day);
    Invalidate();
    UpdateWindow();
}

void Ui_TodayWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_CALENDAR:
            {
                int nIndex = lParam;
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
            }
    }
}

LRESULT Ui_TodayWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    if(MSG_UPDATE_DATE == message){
        int y, m, d;
        bool b;
        y = HIWORD(lParam);
        b = LOWORD(lParam) > 0;
        m = HIWORD(wParam);
        d = LOWORD(wParam);
        _year = y; _month = m; _day = d;
        if(b){
            updateUi();
        }else{
            wchar_t wstmp[128] = {0};
            wsprintf(wstmp,L"%04d年%d月",_year,_month);
            m_YearMonth.SetText(wstmp);
            m_YearMonth.Invalidate();
        }
        ::KillTimer(m_hWnd,0x1001);
        ::SetTimer(m_hWnd,0x1001,10 * 1000,NULL);
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_TodayWnd::OnTimer(UINT_PTR nIDEvent){
    if(nIDEvent == 0x1001){
        ::KillTimer(m_hWnd,0x1001);
        m_pBigDay->ResetScrolling();
        DateTime::getDate(&_year,&_month,&_day);
        updateUi();
    }
}