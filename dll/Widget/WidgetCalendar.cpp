#include "WidgetCalendar.h"

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

#if USE_MEMDC
HDC pMemDC = 0;             //定义内存DC指针
HBITMAP pBitmap = 0;        //定义内存位图指针
int m_nMaxX;
int m_nMaxY;
#endif

class UiWidgetItem : public UiWin{
public:
    UiWidgetItem(UiWin *parent,ItemConfig *cfg){
        pcfg = cfg;
        if(parent && cfg && cfg->Show.Get()){
            SetPos(cfg->X.Get(), cfg->Y.Get(),
                cfg->W.Get(), cfg->H.Get());
            parent->AddChild(this);
        }
    }
    virtual ~UiWidgetItem(){
    }
public:
    bool isShow(){
        if(pcfg){
            return pcfg->Show.Get() != 0;
        }else{
            return false;
        }
    }
    virtual void Invalidate(RECT* prcUpdate=NULL){
		if(isShow()){
#if USE_MEMDC
			reqUpdate = true;
#endif
			UiWin::Invalidate(prcUpdate);
		}
    }
private:
    ItemConfig *pcfg;
#if USE_MEMDC
    bool reqUpdate;
#endif

    friend class UiWidgetIcon;
    friend class UiWidgetString;
    friend class UiScrollDay;
};

class UiWidgetIcon : public UiWidgetItem{
public:
    UiWidgetIcon(UiWin *parent,IconItemConfig *cfg)
        :UiWidgetItem(parent,cfg){
            pimg = 0;
#if USE_MEMDC
            reqUpdate = false;
#endif
    }
    ~UiWidgetIcon(){
        if(pimg){
            pimg->Unload();
            delete pimg;
            pimg = 0;
        }
    }
public:
    void SetImage(ImagingHelper* img){
        if(pimg == img) return;
        if(pimg){
            pimg->Unload();
            delete pimg;
        }
        pimg = img;
        Invalidate();
    }
public:
#if USE_MEMDC
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        if(reqUpdate){
            reqUpdate = false;
            SelectObject(pMemDC, pBitmap);
            SetBkMode(pMemDC,TRANSPARENT);
            RECT rcDraw = {pcfg->X.Get(),pcfg->Y.Get(),
                pcfg->X.Get() + pcfg->W.Get(),pcfg->Y.Get() + pcfg->H.Get()};
            HBRUSH myBrush = CreateSolidBrush(RGB(0,0,0));
            FillRect(pMemDC,&rcDraw,myBrush);	//clear
            DeleteObject(myBrush);
            if(pimg) pimg->Draw(pMemDC,&rcDraw);
        }
        UiWidgetItem::PaintWin(hdcDst, prcWin, prcUpdate);
    }
#else
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        if(pimg) pimg->Draw(hdcDst,prcWin);
    }
#endif
private:
    ImagingHelper *pimg;
};

class UiWidgetString : public UiWidgetItem {
public:
    UiWidgetString(UiWin *parent,StringItemConfig *cfg)
        :UiWidgetItem(parent,cfg){
            if(parent && cfg && cfg->Show.Get()){
                SetTextSize(cfg->FontSize.Get());
                SetTextColor(cfg->Color.Get());
                SetDrawTextFormat(cfg->Align.Get());
                SetTextWeight(cfg->Weight.Get());
            }
#if USE_MEMDC
            reqUpdate = false;
#endif
    }
    bool isShow(){
        if(pcfg){
            return pcfg->Show.Get() != 0;
        }else{
            return false;
        }
    }
public:
#if USE_MEMDC
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        if(reqUpdate){
            reqUpdate = false;
            SelectObject(pMemDC, pBitmap);
            SetBkMode(pMemDC,TRANSPARENT);
            RECT rcDraw = {pcfg->X.Get(),pcfg->Y.Get(),
                pcfg->X.Get() + pcfg->W.Get(),pcfg->Y.Get() + pcfg->H.Get()};

            HBRUSH myBrush = CreateSolidBrush(RGB(0,0,0));
            FillRect(pMemDC,&rcDraw,myBrush);	//clear
            DeleteObject(myBrush);
            //绘制文字
            ::SetTextColor(pMemDC,GetTextColor());
            HFONT font = FontHelper::GetFont(GetTextSize(),GetTextWeight());
            SelectObject(pMemDC,font);
            DrawText(pMemDC,
                GetText().C_Str(),GetText().Length(),
                &rcDraw,GetDrawTextFormat());
            DeleteObject(font);
        }
        UiWidgetItem::PaintWin(hdcDst, prcWin, prcUpdate);
    }
#else
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		::SetBkMode(hdcDst,TRANSPARENT);
		HFONT font = FontHelper::GetFont(GetTextSize(),GetTextWeight());
		SelectObject(hdcDst,font);

		::SetTextColor(hdcDst,GetTextColor());
		DrawText(hdcDst,GetText().C_Str(),GetText().Length(),prcWin,GetDrawTextFormat());
	}
#endif
    virtual void SetText(LPCTSTR text){
        if(lstrcmp(text,GetText().C_Str()) != 0){
            UiWidgetItem::SetText(text);
            Invalidate();
        }
    }
private:
    friend class UiScrollDay;
};

class UiScrollDay : public UiWidgetString {
public:
    UiScrollDay(UiWin *parent,StringItemConfig *cfg)
        : UiWidgetString(parent,cfg){
            DateTime::getDate(&_year1,&_month1,&_day1);
            _year2 = _year1; _month2 = _month1; _day2 = _day1;
            bPressed = false;
            speed = 0;
            scrolled = 0;
            scrolling = false;
            scrollend = false;
            SetID(SW_GetFreeID());
    }
    ~UiScrollDay(){
        SW_ReleaseID(GetID());
    }
public:
    void ResetScrolling(){
        scrolling = false;
        scrollend = false;
        DateTime::getDate(&_year1,&_month1,&_day1);
        _year2 = _year1; _month2 = _month1; _day2 = _day1;
    }
    //
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        if(!scrolling){
            UiWidgetString::PaintWin(hdcDst,prcWin,prcUpdate);
            return;
        }
#if USE_MEMDC
        if(reqUpdate){
            reqUpdate = false;
#endif
            if(scrollend){
#if USE_MEMDC
                SelectObject(pMemDC, pBitmap);
                SetBkMode(pMemDC,TRANSPARENT);
                RECT rcDraw = {pcfg->X.Get(),pcfg->Y.Get(),
                    pcfg->X.Get() + pcfg->W.Get(),pcfg->Y.Get() + pcfg->H.Get()};

                HBRUSH myBrush = CreateSolidBrush(RGB(0,0,0));
                FillRect(pMemDC,&rcDraw,myBrush);	//clear
                DeleteObject(myBrush);

                HFONT font = FontHelper::GetFont(GetTextSize(),GetTextWeight());
                SelectObject(pMemDC,font);
                ::SetTextColor(pMemDC,GetTextColor());
                wchar_t strval[10] = {0};
                swprintf(strval,L"%d",_day);
                DrawText(pMemDC,strval,lstrlen(strval),&rcDraw,GetDrawTextFormat());
#else
			::SetBkMode(hdcDst,TRANSPARENT);
            HFONT font = FontHelper::GetFont(GetTextSize(),GetTextWeight());
            SelectObject(hdcDst,font);

            ::SetTextColor(hdcDst,GetTextColor());
            wchar_t strval[10] = {0};
            swprintf(strval,L"%d",_day);
            DrawText(hdcDst,strval,lstrlen(strval),prcWin,GetDrawTextFormat());
#endif
            }else{
#if USE_MEMDC
                HDC pScrollDC = CreateCompatibleDC(pMemDC);
                HBITMAP pScrollBitmap = CreateCompatibleBitmap(pMemDC,pcfg->W.Get(),pcfg->H.Get() * 2);
#else
                HDC pScrollDC = CreateCompatibleDC(hdcDst);
                HBITMAP pScrollBitmap = CreateCompatibleBitmap(hdcDst,pcfg->W.Get(),pcfg->H.Get() * 2);
#endif
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(pScrollDC, pScrollBitmap);

                RECT rcScroll = {0,0,pcfg->W.Get(),pcfg->H.Get() * 2};
                HBRUSH myBrush = CreateSolidBrush(RGB(255-64,255-64,255-64));
                FillRect(pScrollDC,&rcScroll,myBrush);	//clear
                DeleteObject(myBrush);

                SetBkMode(pScrollDC,TRANSPARENT);

                HFONT font = FontHelper::GetFont(GetTextSize(),GetTextWeight());
                SelectObject(pScrollDC,font);
                ::SetTextColor(pScrollDC,RGB(128,128,128));

                rcScroll.bottom = pcfg->H.Get();
                wchar_t strval[10] = {0};
                swprintf(strval,L"%d",_day1);
                DrawText(pScrollDC,strval,lstrlen(strval),&rcScroll,GetDrawTextFormat());

                rcScroll.top = pcfg->H.Get();
                rcScroll.bottom = pcfg->H.Get()*2;
                swprintf(strval,L"%d",_day2);
                DrawText(pScrollDC,strval,lstrlen(strval),&rcScroll,GetDrawTextFormat());

                int offset = scrolled;
                if(offset < 0){
                    offset = 0 - offset;
                }else{
                    offset = pcfg->H.Get() - offset;
                }
#if USE_MEMDC
                ::BitBlt(pMemDC,pcfg->X.Get(),pcfg->Y.Get(),pcfg->W.Get(),pcfg->H.Get(),
                    pScrollDC,0,offset,SRCCOPY);
#else
				::TransparentBlt(hdcDst,prcWin->left,prcWin->top,RECT_WIDTH(*prcWin),RECT_HEIGHT(*prcWin),
                    pScrollDC,0,offset,RECT_WIDTH(*prcWin),RECT_HEIGHT(*prcWin),RGB(255-64,255-64,255-64));
#endif

                SelectObject(pScrollDC, hOldBitmap);
                ::DeleteObject(pScrollBitmap);
                ::DeleteDC(pScrollDC);
            }
#if USE_MEMDC
        }
#endif
    }
    //
    virtual int OnLButtonDown(UINT fwKeys, int xPos, int yPos){
        int iRet = 0;//UiWidgetString::OnLButtonDown(fwKeys,xPos,yPos);
        if((bPressed = isContain(xPos,yPos))){
            syPos = yPos;
            speedPos = yPos;
            t_scrolled = 0;
            scrolled = 0;
            direction = 0;
            tickcount = ::GetTickCount();
            scrolling = true;
            scrollend = false;
            iRet = 1;
        }
        return iRet;
    }
    virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
        int iRet = 0;//UiWidgetString::OnLButtonUp(fwKeys,xPos,yPos);
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
#if 0
            //result show
            if(direction < 0){
                updateParent(_year1,_month1,_day1,true);
            }else{
                updateParent(_year2,_month2,_day2,true);
            }
#endif
            iRet = 1;
        }
        return iRet;
    }
    virtual int OnMouseMove(UINT fwKeys, int xPos, int yPos){
        int iRet = UiWidgetString::OnMouseMove(fwKeys,xPos,yPos);
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
        UiWidget_Calendar *p = static_cast<UiWidget_Calendar *>(GetParent());
        p->updateDate(y,m,d,b);
        if(b){
            scrollend = true;
            _day = d;
        }
    }
    bool isContain(int xPos, int yPos){
        if(isShow() && this->IsVisible()){
            if(xPos >= GetPosX() &&
                xPos <= GetPosX() + GetWidth() - 20 &&
                yPos >= GetPosY() &&
                yPos <= GetPosY() + GetHeight()){
                    return true;
            }
        }
        return false;
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

class UiZodiac : public UiWidgetIcon
{
public:
    UiZodiac(UiWin *parent,IconItemConfig *cfg) :
      UiWidgetIcon(parent,cfg){
          zodiacidx = -1;
      }
      void SetZodiac(int idx){
          if(zodiacidx == idx) return;
          zodiacidx = idx;
          ImagingHelper* pimg = new ImagingHelper;

          TCHAR ImagePath[MAX_PATH];
          File::GetCurrentPath(ImagePath,GetThisModule());
          TCHAR ImageName[MAX_PATH];
          wsprintf(ImageName,L"images\\%d.png",idx + 1);
          wcscat(ImagePath,ImageName);
          pimg->LoadImage(ImagePath,true,true,true);

          SetImage(pimg);
      }
private:
    int zodiacidx;
};

class UiPromoteIcon : public UiWidgetIcon{
public:
    UiPromoteIcon(UiWin *parent,IconItemConfig *cfg) :
      UiWidgetIcon(parent,cfg){
          n = 0;
          bPressed = false;
          this->SetVisible(false);
      }
public:
    void SetPromoteNumber(int n){
        if(this->n != n){
            this->n = n;
            if(this->n < 1){
                this->SetVisible(false);
            }else{
                this->SetVisible(true);
            }
            Invalidate();
        }
    }
    void SetImageID(int idx){
        if(idx > 6 || idx < 0) return;
        nID = idx;
        ImagingHelper* pimg = new ImagingHelper;

        TCHAR ImagePath[MAX_PATH];
        File::GetCurrentPath(ImagePath,GetThisModule());
        TCHAR ImageName[MAX_PATH];
        wsprintf(ImageName,L"images\\sign%d.png",idx + 1);
        wcscat(ImagePath,ImageName);
        pimg->LoadImage(ImagePath,true,true,true);

        SetImage(pimg);
    }
protected:
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        UiWidgetIcon::PaintWin(hdcDst, prcWin, prcUpdate);
        if(n < 2) return;
        ::SetBkMode(hdcDst,TRANSPARENT);
        ::SetTextColor(hdcDst,RGB(255,64,64));
        RECT textrect;
        textrect.top = prcWin->top;
        textrect.bottom = prcWin->top + GetHeight() - 2;
        textrect.left = prcWin->left + 2;
        textrect.right = prcWin->left + GetWidth() - 2;
        HFONT font = FontHelper::GetFont(GetHeight()/2);
        SelectObject(hdcDst,font);
        wchar_t strval[10] = {0};
        swprintf(strval,L"%d",n);
        DrawText(hdcDst,strval,lstrlen(strval),&textrect,DT_RIGHT|DT_TOP);
        DeleteObject(font);
    }
public:
    bool isContain(int xPos, int yPos){
        if(isShow() && this->IsVisible()){
            if(xPos >= GetPosX() &&
                xPos <= GetPosX() + GetWidth() &&
                yPos >= GetPosY() &&
                yPos <= GetPosY() + GetHeight()){
                    return true;
            }
        }
        return false;
    }
    virtual int OnLButtonDown( UINT fwKeys, int xPos, int yPos ){
        int iRet = 0;//UiWin::OnLButtonDown(fwKeys,xPos,yPos);
        bPressed = isContain(xPos,yPos);
        if(bPressed){
            iRet = 1;
        }
        return iRet;
    }
    virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
        int iRet = 0;//UiWin::OnLButtonUp(fwKeys,xPos,yPos);
        if(bPressed && isContain(xPos,yPos)){
            //do actions
            UiWidget_Calendar *p = static_cast<UiWidget_Calendar *>(GetParent());
            p->SetShowCmd(nID);
            iRet = 1;
        }
        return iRet;
    }
private:
    int n;
    int nID;
    bool bPressed;
};


////////////////////////////////////////////////////

UiWidget_Calendar::UiWidget_Calendar()
{
    pbkgrndimg = NULL;
    pconfig = new WidgetConfig;
	pfestival = NULL;
    InitItems();
}

UiWidget_Calendar::~UiWidget_Calendar()
{
    if(pconfig){
        delete pconfig;
        pconfig = 0;
    }
}
bool UiWidget_Calendar::StartWidget()
{
    UINT id = SW_GetFreeID();
    if(id==0)
        return false;

    ScrollDateTimeout = 0;
    _year = 0; _month = 0; _day = 0;
    _hour = 0; _minute = 0;
    externForceRefresh = false;

    SetID(id);
    //this->EnableNotifyMessage(true);
    this->EnableGrabMouseEventOfChildren(true);
    this->EnablePressedHoldSupport(true);
    this->SetPressedHoldTime(2000);

    SetupItems();
    ///----------------------
    ::SetTimer(GetParentWnd(),GetID(),1000,NULL);
	::SetTimer(GetParentWnd(),GetID() + 0x1000,1000*10,NULL);

	if(pfestival == 0){
        wchar_t currpath[MAX_PATH];
        if(File::GetCurrentPath(currpath,GetThisModule())){
            wcscat(currpath,L"sys_festivals.txt");
        }else{
            wsprintf(currpath,L"sys_festivals.txt");
        }
        pfestival = new FestivalIO(currpath);
        if(!pfestival->load()){
            delete pfestival;
            pfestival = 0;
        }
    }

    return TRUE;
}

void UiWidget_Calendar::DrawBackgroundDC(HDC hDC, RECT* prcDraw){
    //draw background
    if(pbkgrndimg == NULL){
        pbkgrndimg = new ImagingHelper;
        TCHAR ImagePath[MAX_PATH];
        File::GetCurrentPath(ImagePath,GetThisModule());
        wcscat(ImagePath,L"images\\background.png");
        pbkgrndimg->LoadImage(ImagePath,true,true,true);
    }

    pbkgrndimg->Draw(hDC,prcDraw);
}

void UiWidget_Calendar::EndWidget(){
    ::KillTimer(GetParentWnd(),GetID());
    ::SW_ReleaseID(GetID());
#if USE_MEMDC
    if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
    if(pBitmap) DeleteObject(pBitmap);
    pMemDC = 0; pBitmap = 0;
#endif
    UnloadItems();

    if(pbkgrndimg){
        pbkgrndimg->Unload();
        delete pbkgrndimg;
        pbkgrndimg = NULL;
    }
    if(pfestival) delete pfestival;

}

void UiWidget_Calendar::InitItems(){
    //初始化控件
    UiWin **items[] = {
        (UiWin**)&m_pZodiac,(UiWin**)&m_pYearMonth,(UiWin**)&m_pBigDay,
        (UiWin**)&m_pWeekDayCN,(UiWin**)&m_pWeekDayEN,
        (UiWin**)&m_pGanZhiYear,(UiWin**)&m_pLunarMonthDay,
        (UiWin**)&m_pCeli1,(UiWin**)&m_pJieqi,(UiWin**)&m_pCeli2,
        (UiWin**)&m_pGanZhiMonth,(UiWin**)&m_pLunarBirth,(UiWin**)&m_pSolarBirth,
        (UiWin**)&m_pWeeklyReminder,
        (UiWin**)&m_pMonthlyReminder,(UiWin**)&m_pCountDown,
        (UiWin**)&m_pCountUp,(UiWin**)&m_pClockHour,
        (UiWin**)&m_pClockMinute,(UiWin**)&m_pClockColon
    };

    for(int i = 0; i < sizeof(items)/sizeof(items[0]); i++){
        *(items[i]) = 0;
    }
}

void UiWidget_Calendar::SetupItems(){
    if(m_pBigDay == 0){
        m_pBigDay = new UiScrollDay(this,&pconfig->Day);
    }
    if(m_pZodiac == 0){
        m_pZodiac = new UiZodiac(this,&pconfig->Zodiac);
    }
    struct tagStringItems{
        UiWidgetString* *pstring;
        StringItemConfig* cfg;
    }strItems[] = {
        {&m_pYearMonth, &pconfig->YearMonth  },
        {&m_pWeekDayCN,  &pconfig->WeekdayCN },
        {&m_pWeekDayEN,  &pconfig->WeekdayEn },
        {&m_pGanZhiYear, &pconfig->LunarYear },
        {&m_pLunarMonthDay,  &pconfig->LunarMonthDay },
        {&m_pCeli1,      &pconfig->SolarHoliday  },
        {&m_pJieqi,      &pconfig->Jieqi },
        {&m_pCeli2,      &pconfig->LunarHoliday  },
        {&m_pGanZhiMonth,    &pconfig->GanzhiMonthDay    },
        {&m_pCeli2,      &pconfig->LunarHoliday  },
        {&m_pGanZhiMonth,    &pconfig->GanzhiMonthDay    },
        {&m_pClockHour,  &pconfig->ClockHour },
        {&m_pClockMinute,    &pconfig->ClockMinute   },
        {&m_pClockColon, &pconfig->ClockColon    },
    };
    for(int i = 0; i < sizeof(strItems)/sizeof(strItems[0]); i++){
        if(*(strItems[i].pstring) == 0){
            *(strItems[i].pstring) = new UiWidgetString(this,strItems[i].cfg);
        }
    }


    struct tagPromoteIconItems{
        UiPromoteIcon* *picon;
        IconItemConfig* cfg;
    }promoteitems[] = {
        {&m_pLunarBirth, &pconfig->LunarBirth    },
        {&m_pSolarBirth, &pconfig->SolarBirth   },
        {&m_pWeeklyReminder, &pconfig->WeekReminder  },
        {&m_pMonthlyReminder,    &pconfig->MonthReminder },
        {&m_pCountDown,  &pconfig->CountDown },
        {&m_pCountUp,    &pconfig->CountUp   },
    };

    for(int i = 0; i < sizeof(promoteitems)/sizeof(promoteitems[0]); i++){
        if(*(promoteitems[i].picon) == 0){
            *(promoteitems[i].picon) = new UiPromoteIcon(this,promoteitems[i].cfg);
            (*(promoteitems[i].picon))->SetImageID(i);
        }
    }
}

void UiWidget_Calendar::UnloadItems(){
    UiWin **items[] = {
        (UiWin**)&m_pZodiac,(UiWin**)&m_pYearMonth,(UiWin**)&m_pBigDay,
        (UiWin**)&m_pWeekDayCN,(UiWin**)&m_pWeekDayEN,
        (UiWin**)&m_pGanZhiYear,(UiWin**)&m_pLunarMonthDay,
        (UiWin**)&m_pCeli1,(UiWin**)&m_pJieqi,(UiWin**)&m_pCeli2,
        (UiWin**)&m_pGanZhiMonth,(UiWin**)&m_pLunarBirth,(UiWin**)&m_pSolarBirth,
        (UiWin**)&m_pWeeklyReminder,
        (UiWin**)&m_pMonthlyReminder,(UiWin**)&m_pCountDown,
        (UiWin**)&m_pCountUp,(UiWin**)&m_pClockHour,
        (UiWin**)&m_pClockMinute,(UiWin**)&m_pClockColon
    };

    for(int i = 0; i < sizeof(items)/sizeof(items[0]); i++){
        if(*items[i]){
            //            RemoveChild(widgets[i]);    //移除控件
            delete (*items[i]);  //删除控件
            *(items[i]) = 0;
        }
    }
}

void UiWidget_Calendar::updateClock(){
    SYSTEMTIME ctm;
    ::GetLocalTime(&ctm);
    if(ctm.wHour == _hour && 
        ctm.wMinute == _minute){
            return;
    }
    _hour = ctm.wHour;
    _minute = ctm.wMinute;
    if(m_pClockHour->isShow()){
        wchar_t s[20] = {0};
        wsprintf(s,L"%02d",_hour);
        m_pClockHour->SetText(s);
    }
    if(m_pClockMinute->isShow()){
        wchar_t s[20] = {0};
        wsprintf(s,L"%02d",_minute);
        m_pClockMinute->SetText(s);
    }
    if(m_pClockColon->isShow()){
        m_pClockColon->SetText(L":");
    }
}


void UiWidget_Calendar::updateUi(){
    updateClock();

    if(!externForceRefresh){
        //是否正在滚动日期，不需要刷新到今天
        if(ScrollDateTimeout == 0){
            int year,month,day;

            DateTime::getDate(&year,&month,&day);
            if(year == _year && month == _month && day == _day) return;
            _year = year; _month = month; _day = day;
        }
    }else{
        externForceRefresh = false;
    }

    wchar_t wstmp[128] = {0};
    if(m_pYearMonth->isShow()){
        wsprintf(wstmp,L"%04d年%d月",_year,_month);
        m_pYearMonth->SetText(wstmp);
    }

    int week = (DateTime::getWeekDay(_year,_month,_day))%7;	//获取星期
    if(m_pBigDay->isShow()){
        wsprintf(wstmp,L"%d",_day);
        m_pBigDay->SetText(wstmp);
        //设置星期颜色
        if(week < 5){
            m_pBigDay->SetTextColor((COLORREF)pconfig->Day.Color.Get());
        }else{
            m_pBigDay->SetTextColor((COLORREF)pconfig->Day.Color2.Get());
        }
        m_pBigDay->Invalidate();
    }


    if(m_pWeekDayCN->isShow()){
        m_pWeekDayCN->SetText(WeekDayNameCN[week]);
    }

    if(m_pWeekDayEN->isShow()){
        m_pWeekDayEN->SetText(WeekDayNameEN[week]);
    }

    LCAL _lstm(_year,_month,_day);
    _lstm.SolarToLunar();
    unsigned char zodiacidx = 0;

    if(m_pGanZhiYear->isShow()){
        wsprintf(wstmp,L"%s(%s)年",
            _lstm.GanZhiYear().C_Str(),
            _lstm.Zodiac(&zodiacidx).C_Str());
        m_pGanZhiYear->SetText(wstmp);
    }

    if(m_pZodiac->isShow()){
        m_pZodiac->SetZodiac(zodiacidx);
    }

    if(m_pLunarMonthDay->isShow()){
        wsprintf(wstmp,L"%s%s",
            _lstm.LunarMonth().C_Str(),
            _lstm.OriginalLunarDay().C_Str());
        m_pLunarMonthDay->SetText(wstmp);
    }

    wchar_t holidayname[20];
    memset(holidayname,0,sizeof(holidayname));
    if(m_pCeli1->isShow()){
		bool b = false;
		if(pfestival){
			if(!_lstm.isLunarLeapMonth()){
				LSDate l = _lstm.getLunarDate();
				b = pfestival->LunarHoliday(l.month,l.day,holidayname);
			}
		}
		if(!b){
			b = _lstm.LunarHoliday(holidayname);
		}
		if(b){
			m_pCeli1->SetText(holidayname);
		}else{
			m_pCeli1->SetText(L"\0");
		}

        memset(holidayname,0,sizeof(holidayname));
    }

    if(m_pCeli2->isShow()){
		bool b = false;
		if(pfestival){
			LSDate s = _lstm.getSolarDate();
			b = pfestival->SolarHoliday(s.month,s.day,holidayname);
		}
		if(!b){
			b = _lstm.SolarHoliday(holidayname);
		}
		if(b){
			m_pCeli1->SetText(holidayname);
		}else{
			m_pCeli1->SetText(L"\0");
		}
    }

    if(m_pJieqi->isShow()){
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
        m_pJieqi->SetText(wstmp);
    }


    if(m_pGanZhiMonth->isShow()){
        wsprintf(wstmp,L"%s月 %s日",_lstm.GanZhiMonth().C_Str(),_lstm.GanZhiDay().C_Str());
        m_pGanZhiMonth->SetText(wstmp);
    }

    //生日
	if(pfestival){
		if(m_pLunarBirth->isShow()){
			if(!_lstm.isLunarLeapMonth()){
				pfestival->queryBirthday(_lstm.getLunarDate().month,_lstm.getLunarDate().day,true);
				m_pLunarBirth->SetPromoteNumber(pfestival->BirthdaySize());
			}
		}
		if(m_pSolarBirth->isShow()){
			pfestival->queryBirthday(_lstm.getSolarDate().month,_lstm.getSolarDate().day,false);
			m_pSolarBirth->SetPromoteNumber(pfestival->BirthdaySize());
		}
		if(m_pWeeklyReminder->isShow()){
			pfestival->queryReminder(_lstm.getSolarDate().weekday,0,ReminderWeekly);
			m_pWeeklyReminder->SetPromoteNumber(pfestival->ReminderSize());
		}
		if(m_pMonthlyReminder->isShow()){
			pfestival->queryReminder(0,_lstm.getSolarDate().day,ReminderMonthly);
			m_pMonthlyReminder->SetPromoteNumber(pfestival->ReminderSize());
		}
		if(m_pCountDown->isShow()){
			pfestival->queryReminder(0,0,ReminderCountdown);
			int rsz = pfestival->ReminderSize();
			int nSize = 0;
			for(int i = 0; i < rsz; i++){
				lpFestival p = pfestival->Reminder(i);
				LSDate d = LSDate(p->info0.year,p->month,p->day);
				LSDate s = LSDate(_year,_month,_day);
				int days = _lstm.diffDate(d,s);
				if(days >= 0) nSize++;
			}
			m_pCountDown->SetPromoteNumber(nSize);
		}
		if(m_pCountUp->isShow()){
			pfestival->queryReminder(0,0,ReminderCountup);
			int rsz = pfestival->ReminderSize();
			int nSize = 0;
			for(int i = 0; i < rsz; i++){
				lpFestival p = pfestival->Reminder(i);
				LSDate d = LSDate(p->info0.year,p->month,p->day);
				LSDate s = LSDate(_year,_month,_day);
				int days = _lstm.diffDate(s,d);
				if(days >= 0) nSize++;
			}
			m_pCountUp->SetPromoteNumber(nSize);
		}
	}
#if USE_MEMDC
	reqUpdate = true;
#endif
}

int UiWidget_Calendar::OnLButtonDown( UINT fwKeys, int xPos, int yPos )
{
    int iRet = UiWidget::OnLButtonDown(fwKeys, xPos, yPos);
    if(m_pBigDay->isShow()){
        if(m_pBigDay->OnLButtonDown(fwKeys, xPos, yPos)){
            this->EnableScrollableParent(false);
            return 0;
        }
    }
    UiPromoteIcon* pGrapIcon[] = {
        m_pLunarBirth,m_pSolarBirth,m_pWeeklyReminder,m_pMonthlyReminder,m_pCountDown,m_pCountUp
    };
    for(int i = 0; i < sizeof(pGrapIcon)/sizeof(pGrapIcon[0]); i++){
        if(pGrapIcon[i]->isShow()){
            if(pGrapIcon[i]->OnLButtonDown(fwKeys, xPos, yPos)){
                return 0;
            }
        }
    }

    return iRet;
}

int UiWidget_Calendar::OnLButtonUp( UINT fwKeys, int xPos, int yPos )
{
    int iRet = UiWidget::OnLButtonUp(fwKeys, xPos, yPos);
    if(m_pBigDay->isShow()){
        if(m_pBigDay->OnLButtonUp(fwKeys, xPos, yPos)){
            this->EnableScrollableParent(true);
            return 0;
        }
    }

    UiPromoteIcon* pGrapIcon[] = {
        m_pLunarBirth,m_pSolarBirth,m_pWeeklyReminder,m_pMonthlyReminder,m_pCountDown,m_pCountUp
    };
    for(int i = 0; i < sizeof(pGrapIcon)/sizeof(pGrapIcon[0]); i++){
        if(pGrapIcon[i]->isShow()){
            if(pGrapIcon[i]->OnLButtonUp(fwKeys, xPos, yPos)){
                return 0;
            }
        }
    }

    return iRet;
}

int UiWidget_Calendar::OnMouseMove( UINT fwKeys, int xPos, int yPos )
{
    int iRet = UiWidget::OnMouseMove(fwKeys, xPos, yPos);
    if(m_pBigDay->isShow()){
        if(m_pBigDay->OnMouseMove(fwKeys, xPos, yPos)){
            return 0;
        }
    }

    return iRet;
}

int UiWidget_Calendar::OnLButtonDblClk(UINT fwKeys, int xPos, int yPos){
    int iRet = UiWidget::OnLButtonDblClk(fwKeys, xPos, yPos);
    PROCESS_INFORMATION pi;
    LPCTSTR exename = pconfig->LinkedExe.GetStrVal();
    ::CreateProcess(exename, 
        NULL , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
    return iRet;
}

void UiWidget_Calendar::OnPressedHoldTimeup(){
    UiWidget::OnPressedHoldTimeup();
    PROCESS_INFORMATION pi;
    wchar_t exepath[MAX_PATH];
    if(File::GetCurrentPath(exepath,GetThisModule())){
        wcscat(exepath,L"FestivalEditor.exe");
    }else{
        wsprintf(exepath,L"FestivalEditor.exe");
    }
    ::CreateProcess(exepath, 
    	NULL , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
}

void UiWidget_Calendar::OnCalcItemSize( __out int &xSize, __out int &ySize )
{
    xSize = pconfig->WidgetSizeW.Get();
    ySize = pconfig->WidgetSizeH.Get();
    if(xSize > 4 || xSize < 1) xSize = 4;
    if(ySize > 4 || ySize < 1) ySize = 4;
}

#define REQ_RELOADFEST 0x9481

int UiWidget_Calendar::OnTimer(UINT_PTR nIDEvent){
    UINT_PTR id = GetID();
    if(id == nIDEvent){
		::KillTimer(GetParentWnd(),id);
		::SetTimer(GetParentWnd(),id,1000,NULL);
        if(ScrollDateTimeout){
            ScrollDateTimeout --;
            if(ScrollDateTimeout == 0){
                m_pBigDay->ResetScrolling();
            }
        }else{
            updateUi();
        }
	}else if(id + 0x1000 == nIDEvent){
		::KillTimer(GetParentWnd(),id + 0x1000);
		::SetTimer(GetParentWnd(),id + 0x1000,1000*10,NULL);
		if(CheckExternRequest() == REQ_RELOADFEST) {
			ClearExternReqeust();
			if(pfestival){
				pfestival->reload();
			}
			externForceRefresh = true;
		}
	}
    return 0;
}

#if USE_MEMDC
void UiWidget_Calendar::RefreshAllItems(){
    UiWidgetItem *items[] = {
        m_pZodiac,m_pYearMonth,m_pBigDay,
        m_pWeekDayCN,m_pWeekDayEN,
        m_pGanZhiYear,m_pLunarMonthDay,
        m_pCeli1,m_pJieqi,m_pCeli2,
        m_pGanZhiMonth,m_pLunarBirth,m_pSolarBirth,
        m_pWeeklyReminder,
        m_pMonthlyReminder,m_pCountDown,
        m_pCountUp,m_pClockHour,
        m_pClockMinute,m_pClockColon
    };

    for(int i = 0; i < sizeof(items)/sizeof(items[0]); i++){
        if(items[i] && items[i]->isShow()){
            items[i]->Invalidate();
        }
    }
}
void UiWidget_Calendar::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
    //UiWidget::PaintWin(hdcDst,prcWin,prcUpdate);
    if(reqUpdate){
        reqUpdate = false;
        //设置子项目invalidate
        RefreshAllItems();
    }
    DrawBackgroundDC(hdcDst,prcWin);
    TransparentBlt(hdcDst,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,
        pMemDC,0,0,m_nMaxX,m_nMaxY,RGB(0,0,0));
    //    UiWidget::PaintWin(hdcDst,prcWin,prcUpdate);
}
#else
void UiWidget_Calendar::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
    DrawBackgroundDC(hdcDst,prcWin);
    UiWidget::PaintWin(hdcDst,prcWin,prcUpdate);
}
#endif

void UiWidget_Calendar::ShowDetail(FestivalType t, ReminderType rt){
    MzPopupMenu FestivalDetail;
    LCAL _lstm(_year,_month,_day);

    switch(t){
        case FestivalLunarBirth:
            {
				FestivalDetail.SetMenuTitle(L"农历生日");
				if(pfestival){
					_lstm.SolarToLunar();
					if(!_lstm.isLunarLeapMonth()){
						pfestival->queryBirthday(_lstm.getLunarDate().month,_lstm.getLunarDate().day,true);
						int bsz = pfestival->BirthdaySize();
						for(int i = 0; i < bsz; i++){
							lpFestival p = pfestival->Birthday(i);
							if(p->detail && p->info0.year <= _year){
								wchar_t msg[200] = {0};
								wsprintf(msg,p->detail,_year - p->info0.year + 1);
								FestivalDetail.AppendMenuItem(MZV2_MID_MIN + i, msg);
							}
						}
					}
				}
            }
            break;
        case FestivalSolarBirth:
            {
                FestivalDetail.SetMenuTitle(L"公历生日");
				if(pfestival){
					pfestival->queryBirthday(_lstm.getSolarDate().month,_lstm.getSolarDate().day,false);
					int bsz = pfestival->BirthdaySize();
					for(int i = 0; i < bsz; i++){
						lpFestival p = pfestival->Birthday(i);
						if(p->detail && p->info0.year <= _year){
							wchar_t msg[200] = {0};
							wsprintf(msg,p->detail,_year - p->info0.year + 1);
							FestivalDetail.AppendMenuItem(MZV2_MID_MIN + i, msg);
						}
					}
				}
            }
            break;
        case FestivalSolarRemind:
            {
				if(pfestival){
					if(rt == ReminderCountdown){
						FestivalDetail.SetMenuTitle(L"倒计时");
						pfestival->queryReminder(0,0,rt);
						int rsz = pfestival->ReminderSize();
						for(int i = 0; i < rsz; i++){
							lpFestival p = pfestival->Reminder(i);
							LSDate d = LSDate(p->info0.year,p->month,p->day);
							LSDate s = LSDate(_year,_month,_day);
							int days = _lstm.diffDate(d,s);
							if(days < 0) continue;
							wchar_t msg[200] = {0};
							wsprintf(msg,p->detail,days);
							FestivalDetail.AppendMenuItem(MZV2_MID_MIN + i, msg);
						}
					}else if(rt == ReminderCountup){
						FestivalDetail.SetMenuTitle(L"顺计时");
						pfestival->queryReminder(0,0,rt);
						int rsz = pfestival->ReminderSize();
						for(int i = 0; i < rsz; i++){
							lpFestival p = pfestival->Reminder(i);
							LSDate d = LSDate(p->info0.year,p->month,p->day);
							LSDate s = LSDate(_year,_month,_day);
							int days = _lstm.diffDate(s,d);
							if(days < 0) continue;
							wchar_t msg[200] = {0};
							wsprintf(msg,p->detail,days);
							FestivalDetail.AppendMenuItem(MZV2_MID_MIN + i, msg);
						}
					}else{
						FestivalDetail.SetMenuTitle(L"提醒");
						if(rt == ReminderWeekly){
							pfestival->queryReminder(_lstm.getSolarDate().weekday,0,rt);
						}else{
							pfestival->queryReminder(0,_lstm.getSolarDate().day,rt);
						}
						int rsz = pfestival->ReminderSize();
						for(int i = 0; i < rsz; i++){
							lpFestival p = pfestival->Reminder(i);
							FestivalDetail.AppendMenuItem(MZV2_MID_MIN + i, p->detail);
						}
					}
				}
                break;
            }
        default:
            return;
    }
    FestivalDetail.AddUserButton(L"关闭");
    FestivalDetail.MzTrackPopupMenu(GetParentWnd(), false,0,true);
}

void UiWidget_Calendar::SetShowCmd(int idx){
    switch(idx){
        case 0:
            this->ShowDetail(FestivalLunarBirth);
            break;
        case 1:
            this->ShowDetail(FestivalSolarBirth);
            break;
        case 2:
            this->ShowDetail(FestivalSolarRemind,ReminderWeekly);
            break;
        case 3:
            this->ShowDetail(FestivalSolarRemind,ReminderMonthly);
            break;
        case 4:
            this->ShowDetail(FestivalSolarRemind,ReminderCountdown);
            break;
        case 5:
            this->ShowDetail(FestivalSolarRemind,ReminderCountup);
            break;
    }
}

void UiWidget_Calendar::updateDate(int y, int m, int d, bool all){
    _year = y; _month = m; _day = d;

    ScrollDateTimeout = 10;

    if(all){
        updateUi();
    }else{
        wchar_t wstmp[128] = {0};
        wsprintf(wstmp,L"%04d年%d月",_year,_month);
        m_pYearMonth->SetText(wstmp);
        m_pYearMonth->Invalidate();
    }
}

#if USE_MEMDC
void UiWidget_Calendar::SetPos(int x, int y, int w, int h, UINT flags){
    UiWin::SetPos(x,y,w,h,flags);
    m_nMaxX = w;
    m_nMaxY = h;
    printf("%s,%d\n",__FUNCTION__,__LINE__);
    if(pMemDC == 0){
        pMemDC = CreateCompatibleDC(GetDC(GetParentWnd()));
    }
    if(pBitmap == 0){
        pBitmap = CreateCompatibleBitmap(GetDC(GetParentWnd()),w,h);
    }
}
#endif