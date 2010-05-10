#include "ui_calendar.h"
#include <cMzCommon.h>
using namespace cMzCommon;

#include <UsbNotifyApi.h>
#include "ui_about.h"
#include "ui_dateEdit.h"
#include "ui_history.h"
#include "ui_config.h"
#include "ui_today.h"

#include "UiInstructionDlg.h"
#include "resource.h"

extern CalendarConfig AppConfig;

#include <festio.h>
extern FestivalIO *pfestival;
using std::wstring;

#include "UiFestivalInfo.h"

class UiZodiacImage : public UiWin{
public:
    UiZodiacImage(){
        nzodiac = 0;
        TCHAR ImagePath[MAX_PATH];
        File::GetCurrentPath(ImagePath);
        TCHAR ImageFile[MAX_PATH];
        for(int i = 0; i < 12; i++){
            zodiacs[i] = NULL;
            wsprintf(ImageFile,L"%s\\images\\%d.png",ImagePath,i+1);
            if(File::FileExists(ImageFile)){
                zodiacs[i] = new ImagingHelper;
                zodiacs[i]->LoadImage(ImageFile,true,true,true);
            }
        }
    }
    ~UiZodiacImage(){
        for(int i = 0; i < 12; i++){
            if(zodiacs[i] != NULL){
                zodiacs[i]->Unload();
                delete zodiacs[i];
                zodiacs[i] = NULL;
            }
        }
    }
public:
    void SetZodiac(UCHAR idx){
        if(nzodiac != idx && idx < 12){
            nzodiac = idx;
            Invalidate();
        }
    }
public:
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        UiWin::PaintWin(hdcDst,prcWin,prcUpdate);
        if(zodiacs[nzodiac] != NULL){
            zodiacs[nzodiac]->Draw(hdcDst,prcWin,true,false);
        }
    }
private:
    ImagingHelper *zodiacs[12];
    UCHAR nzodiac;
};

#define CAL_ROW_WIDTH	67
#define CAL_ROW_HEIGHT	60

class UiWeekBar : public UiHeadingBar{
public:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiHeadingBar::PaintWin(hdcDst,prcWin,prcUpdate);
		LPCTSTR sWeekName[] = {
			L"日",L"一",L"二",L"三",L"四",L"五",L"六"
		};
		RECT tRect = {prcWin->left,prcWin->top,prcWin->left + CAL_ROW_WIDTH,prcWin->bottom};
		for(int i = 0; i < sizeof(sWeekName)/sizeof(sWeekName[0]); i++){
            MzDrawText( hdcDst,sWeekName[i], &tRect, DT_CENTER|DT_VCENTER );
			tRect.left = tRect.right;
			tRect.right += CAL_ROW_WIDTH;
		}
	}
};

class UiYiJiLabel : public UiWin
{
public:
	UiYiJiLabel(){
		_isTs = false;
	}
	~UiYiJiLabel(){
	}
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst,prcWin,prcUpdate);
		SetBkMode(hdcDst,TRANSPARENT);

		MzDrawGridDlgBG(hdcDst,prcWin);

		HFONT hf = FontHelper::GetFont( 30 );
		SelectObject( hdcDst , hf );

		RECT rect;
		int height = prcWin->bottom - prcWin->top - 20;
		int width = prcWin->right - prcWin->left;
		rect.left = prcWin->left + 20;
		rect.right = rect.left + 50;
		//宜
		rect.top = prcWin->top;
		rect.bottom = rect.top + height / 2;
		::SetTextColor( hdcDst,RGB(64,255,128));
		MzDrawText( hdcDst, L"宜", &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
		//忌
		rect.top = prcWin->top + height / 2;
		rect.bottom = prcWin->bottom - 10;
		::SetTextColor( hdcDst, RGB(255,64,64));
		MzDrawText( hdcDst, L"忌", &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
		DeleteObject( hf );

		hf = FontHelper::GetFont( 20 );
		SelectObject( hdcDst , hf );

		if(_isTs){
			rect.top = prcWin->top;
			rect.bottom = prcWin->bottom - 10;
			rect.left = prcWin->left + 60;
			rect.right = prcWin->right;
			::SetTextColor( hdcDst, RGB(255,64,255));
			MzDrawText( hdcDst, tsText.C_Str(), &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
		}else{
			::SetTextColor( hdcDst, RGB(255,255,255));

			rect.top = prcWin->top;
			rect.bottom = rect.top + height / 2;
			rect.left = prcWin->left + 60;
			rect.right = prcWin->right;
			MzDrawText( hdcDst, yiText.C_Str(), &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

			rect.top = prcWin->top + height / 2;
			rect.bottom = prcWin->bottom - 10;
			MzDrawText( hdcDst, jiText.C_Str(), &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
		}
		DeleteObject( hf );
	}
	void setText(wchar_t* yi, wchar_t* ji,bool ts = false){
		_isTs = ts;
		if(!ts){
			yiText = yi;
			jiText = ji;
		}else{
			tsText = yi;
		}
	}
private:
	CMzString yiText,jiText,tsText;
	bool _isTs;	//特殊
};

class UiGrid : public UiWin
{
public:
    UiGrid(void)
        :UiWin()
    {
        _grids = 0;
        _selbg = RGB(64,192,192);
        _seltxt = RGB(255,255,255);
        _rows = 1;
        _cols = 1;
        setGridSize(_rows,_cols);
        _gwidth = CAL_ROW_WIDTH;
        _gheight = CAL_ROW_HEIGHT - 1;
        _isAutosize = false;
		
		imgToday = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_WHEEL_SELECT, true);
    }
    ~UiGrid(void){
        setGridSize(0,0);	//release 
        if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
        if(pBitmap) DeleteObject(pBitmap);
    }
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
public:
	//if ret = false, no selection
	bool calcSelectedIndex(int x, int y,int &row, int &col);
	void setSelectedIndex(int row,int col);
	//if ret = false, no selection
	bool getSelectedIndex(int &row, int &col);
public:
	//if idx == -1, set all
	void setTextSize(int row,int col, int nSize);
	//if idx == -1 set all
	void setTextColor(int row,int col, COLORREF c);
	void setText1Color(int row,int col, COLORREF c);
	void setText(int row,int col, LPCTSTR text);
	void setText1(int row,int col, LPCTSTR text);
	int getTextSize(int row,int col);
	COLORREF getTextColor(int row,int col);
	LPCTSTR getText(int row,int col);
	void setSelectedBgColor(COLORREF c);
	void setSelectedTextColor(COLORREF c);
    void setGridAutoSize(bool a){
    	_isAutosize = a;
    }
    void setGridSize(int width, int height){
        _gwidth = width;
        _gheight = height;
    }
    void setSignStatus(int row, int col, UCHAR ino, bool status = true){
        if(!checkRange(row,col)) return;
        if(ino > 7) return;
        if(status){
            _grids[row][col].signs |= (1 << ino);
        }else{
            _grids[row][col].signs &= ~(1 << ino);
        }
    }
public:
    void setGrids(int nRow, int nCol){
        if(_grids){
            for(int i = 0; i < _rows; i++){
                for(int j = 0; j < _cols; j++){
                    _grids[i][j].text.SetBufferSize(0);
                }
                delete _grids[i];
            }
            delete _grids;
            _grids = 0;
        }
        _rows = nRow;
        _cols = nCol;
        if(_rows >= 0 && _cols >= 0){
            _grids = new GridAttr_ptr[_rows];
            for(int i = 0; i < _rows; i++){
                _grids[i] = new GridAttr_t[_cols];
                for(int j = 0; j < _cols; j++){
                    _grids[i][j].isSelected = false;
                    _grids[i][j].text = 0;
                    _grids[i][j].text1 = 0;
                    _grids[i][j].signs = 0;
                    _grids[i][j].textColor = RGB(0,0,0);
                    _grids[i][j].text1Color = RGB(128,128,128);
                    _grids[i][j].textSize = 28;
                    _grids[i][j].text1Size = 17;
                }
            }
        }
    }
	int getRowCount(void);
	int getColCount(void);
public:
    virtual void SetPos(int x, int y, int w, int h, UINT flags=0){
        UiWin::SetPos(x,y,w,h,flags);
        m_nMaxX = w;
        m_nMaxY = h;
        pMemDC = CreateCompatibleDC(GetDC(GetParentWnd()));
        pBitmap = CreateCompatibleBitmap(GetDC(GetParentWnd()),m_nMaxX,m_nMaxY);
    }
private:
	typedef struct GridAttr{
		CMzString text;	//正中
		CMzString text1;	//下一行
		BYTE signs;	//记号 //最多8个
		int textSize;
		int text1Size;
		COLORREF textColor;
		COLORREF text1Color;
		bool isSelected;
	}GridAttr_t,*GridAttr_ptr;
	GridAttr_ptr *_grids;
private:
	COLORREF _selbg, _seltxt;
	int _rows,_cols;
	bool _isAutosize;
	int _gwidth, _gheight;
	int m_nMaxX;
	int m_nMaxY;

	ImageContainer m_imgContainer;
	ImagingHelper *imgToday;
private:
	HDC pMemDC;             //定义内存DC指针
	HBITMAP pBitmap;        //定义内存位图指针
private:
    bool checkRange(int row, int col){
        if(row >= _rows || col >= _cols ||
            row < 0 || col < 0){
                return false;
        }
        return true;
    }
};

void UiGrid::setSelectedIndex(int row,int col){
	if(checkRange(row,col)){
		int oldr, oldc;
		if(getSelectedIndex(oldr,oldc)){
			_grids[oldr][oldc].isSelected = false;
		}
		_grids[row][col].isSelected = true;
	}
}

bool UiGrid::calcSelectedIndex(int x, int y, int &row, int &col){
	bool ret = true;
	int c = x / _gwidth;
	int r = y / _gheight;
	if(c > _cols || r > _rows){
		row = -1;
		col = -1;
		ret = false;
	}else{
		row = r;
		col = c;
	}
	return ret;
}

bool UiGrid::getSelectedIndex(int &row, int &col){
	bool ret = true;
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				if(_grids[i][j].isSelected){
					row = i;
					col = j;
					return ret;
				}
			}
		}
	row = -1;
	col = -1;
	return false;
}

int UiGrid::getColCount(){
	return _cols;
}

int UiGrid::getRowCount(){
	return _rows;
}

void UiGrid::setText(int row,int col, LPCTSTR text){
	if(checkRange(row,col)){
		_grids[row][col].text = text;
	}
}

void UiGrid::setText1(int row,int col, LPCTSTR text){
	if(checkRange(row,col)){
		_grids[row][col].text1 = text;
	}
}

void UiGrid::setTextColor(int row,int col, COLORREF c){
	if(checkRange(row,col)){
		_grids[row][col].textColor = c;
	}
}

void UiGrid::setText1Color(int row,int col, COLORREF c){
	if(checkRange(row,col)){
		_grids[row][col].text1Color = c;
	}
}

void UiGrid::setTextSize(int row,int col, int nSize){
	if(checkRange(row,col)){
		_grids[row][col].textSize = nSize;
	}
}

LPCTSTR UiGrid::getText(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].text;
	}
	return 0;
}

COLORREF UiGrid::getTextColor(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].textColor;
	}
	return RGB(0,0,0);
}

int UiGrid::getTextSize(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].textSize;
	}
	return 0;
}

void UiGrid::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
	UiWin::PaintWin(hdcDst,prcWin,prcUpdate);
    SelectObject(pMemDC, pBitmap);
    HBRUSH myBrush = CreateSolidBrush(RGB(255-16,255-16,255-16));
    RECT rect;
    rect.top = 0;
    rect.bottom = m_nMaxY;
    rect.left = 0;
    rect.right = m_nMaxX;

    FillRect(pMemDC,&rect,myBrush);//画之前先擦除.
    //SetTextColor(RGB(0,0,0));

    //无数据
    if(_rows <= 0 || _cols <= 0){
        BitBlt(hdcDst,0,0,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
        return;
    }
    int _gridw = _gwidth;
    int _gridh = _gheight;
    if(_isAutosize){
        int _width = prcWin->right - prcWin->left;
        int _height = prcWin->bottom - prcWin->top;
        _gridw = _width/_cols;
        _gridh = _height/_rows;
    }
    int _x = 8;//prcWin->left;
    int _y = 8;//prcWin->top;

    HPEN pen = CreatePen(PS_SOLID, 0,RGB(128,128,128));
    HPEN poldpen = (HPEN)SelectObject(pMemDC,pen);
    SetBkMode(pMemDC,TRANSPARENT);
    for(int i = 0; i < _rows; i++){
        int cx = _x;
        int cy = _y;
        for(int j = 0; j < _cols; j++){
            //格子
            RECT rect = {
				cx + (_gridw - 1) * j,
				cy + (_gridh - 1) * i,
				cx + (_gridw - 1) * j + _gridw,
				cy + (_gridh - 1) * i + _gridh
			};
            Rectangle(pMemDC,rect.left,rect.top,rect.right,rect.bottom);
            //格子背景
            RECT frect = {rect.left + 1,rect.top + 1,rect.right - 1,rect.bottom - 1};
            RECT textrect = {rect.left + 1,rect.top + 1,rect.right - 2,rect.bottom - 20};
            HBRUSH bqbrush;
            HFONT font;
            if(_grids[i][j].isSelected){	//selected
                font = FontHelper::GetFont(_grids[i][j].textSize + 2);
                ::SetTextColor(pMemDC,_seltxt);
				MzDrawSelectedBg_NoLine(pMemDC,&frect);
			}else{
                font = FontHelper::GetFont(_grids[i][j].textSize);
				::SetTextColor(pMemDC,_grids[i][j].textColor);
			}
            //text
            SelectObject(pMemDC,font);
            MzDrawText( pMemDC,_grids[i][j].text.C_Str(), &textrect, DT_CENTER|DT_VCENTER );

            //text1
            font = FontHelper::GetFont(_grids[i][j].text1Size);
            SelectObject(pMemDC,font);
            ::SetTextColor(pMemDC,_grids[i][j].text1Color);
            RECT text1rect = {rect.left+1,rect.top + 35,rect.right - 2,rect.bottom - 2};
            MzDrawText( pMemDC,_grids[i][j].text1.C_Str(), &text1rect, DT_CENTER|DT_VCENTER );
            //sign rect
            //计算bit位数
            //UCHAR bits = 0;
            //for(UCHAR signs = _grids[i][j].signs;
            //    signs != 0; signs &= signs-1){
            //        bits ++;
            //}
            //if(bits > 0){
            if(_grids[i][j].signs > 0){
                //绘制
#define SIGN_WIDTH 6
#define SIGN_HEIGHT 6
                RECT signrect = {rect.left+5,rect.top + 5,rect.left + 5 + SIGN_WIDTH,rect.top + 5 + SIGN_HEIGHT};
                UCHAR pos = 0;
                for(UCHAR signs = _grids[i][j].signs;
                    signs != 0; signs >>= 1, pos++){
                        if(signs&0x1){
                            bqbrush = CreateSolidBrush(festival_colors[pos]);
                            FillRect(pMemDC,&signrect,bqbrush);
                            signrect.top += SIGN_HEIGHT;
                            signrect.bottom = signrect.top + SIGN_HEIGHT;
                        }
                }
            }
        }
		//draw today
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				if(_grids[i][j].textColor == RGB(192,64,64)){
					RECT rectHL = {
						_x + (_gridw - 1) * j - 8,
						_y + (_gridh - 1) * i - 10,
						_x + (_gridw - 1) * j + _gridw + 8,
						_y + (_gridh - 1) * i + _gridh + 10,
					};
					imgToday->Draw(pMemDC,&rectHL,true);
				}
			}
		}
    }
    SelectObject(pMemDC,poldpen);
	BitBlt(hdcDst,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
}

//////
MZ_IMPLEMENT_DYNAMIC(Ui_CalendarWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
#define MZ_IDC_Edit_YEAR 102
#define MZ_IDC_Edit_MONTH 103
#define MZ_IDC_Edit_DAY 104
#define MZ_IDC_CALENDAR_GRID 105
#define MZ_IDC_YIJI_TIP 108
#define MZ_IDC_FEST_INFO 109

Ui_CalendarWnd::Ui_CalendarWnd(void)
{
	_isMouseMoving = false;
    _year = _month = _day = 0;
    INIT_PTR(m_pFestDetail);
    INIT_PTR(m_pTipyiji);
    INIT_PTR(m_pCalendar);
    INIT_PTR(m_pZodiacImage);
}

Ui_CalendarWnd::~Ui_CalendarWnd(void)
{
    DEL_PTR(m_pFestDetail);
    DEL_PTR(m_pTipyiji);
    DEL_PTR(m_pCalendar);
    DEL_PTR(m_pZodiacImage);
}

CMzString Ui_CalendarWnd::getDate(){
	CMzString sDate;
	wsprintf(sDate.C_Str(), L"%4d-%02d-%02d", _year, _month, _day);
	return sDate;
}

#include <UsbNotifyApi.h>

#define IDC_PPM_CONFIG 1001
#define IDC_PPM_TODATE 1002
#define IDC_PPM_DATES 1003
#define IDC_PPM_HISTORY 1004

BOOL Ui_CalendarWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    SetWindowText(L"掌上农历");
    // Then init the controls & other things in the window
    int y = 0;
	m_CaptionHeader.SetPos(0,y,GetWidth(),100);
	m_CaptionHeader.EnableNotifyMessage(true);
	AddUiWin(&m_CaptionHeader);

    m_pZodiacImage = new UiZodiacImage;
	m_pZodiacImage->SetPos(20,0,90,90);
	m_CaptionHeader.AddChild(m_pZodiacImage);

	m_YearMonth.SetPos(90 + 40,5,(GetWidth() - 90)*3/4,35);
	m_YearMonth.SetTextSize(32);
    m_YearMonth.SetTextWeight(FW_BOLD);
	m_YearMonth.EnableNotifyMessage(true);
	m_CaptionHeader.AddChild(&m_YearMonth);

	m_LunarMD.SetPos(90 + 40,5 + 35,(GetWidth() - 90)*3/4,25);
	m_LunarMD.SetTextColor(RGB(128,128,128));
	m_LunarMD.SetTextSize(24);
    //m_LunarMD.SetTextWeight(FW_BOLD);
	m_LunarMD.EnableNotifyMessage(true);
	m_CaptionHeader.AddChild(&m_LunarMD);

    m_GanZhiYMD.SetPos(90 + 40,5 + 35 + 25 + 5,(GetWidth() - 90)*3/4,25);
	m_GanZhiYMD.SetTextColor(RGB(128,128,128));
	m_GanZhiYMD.SetTextSize(24);
    //m_GanZhiYMD.SetTextWeight(FW_BOLD);
	m_GanZhiYMD.EnableNotifyMessage(true);
	m_CaptionHeader.AddChild(&m_GanZhiYMD);


	y += 100;
	m_pWeekBar = new UiWeekBar;
	m_pWeekBar->SetPos(0,y,GetWidth(),MZM_HEIGHT_HEADINGBAR);
	m_pWeekBar->SetTextSize(17);
	m_pWeekBar->SetTextColor(RGB(128,128,128));
	m_pWeekBar->EnableNotifyMessage(true);
	AddUiWin(m_pWeekBar);

	y += MZM_HEIGHT_HEADINGBAR;
	m_pCalendar = new UiGrid;
    m_pCalendar->SetPos(1, y, GetWidth()-2, CAL_ROW_HEIGHT*6 + 16);
	m_pCalendar->SetID(MZ_IDC_CALENDAR_GRID);
	m_pCalendar->EnableNotifyMessage(true);
    AddUiWin(m_pCalendar);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"今日");
    m_Toolbar.SetMiddleButton(true, true, L"工具",NULL,NULL,NULL);
    m_Toolbar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON, true, true, L"关于");
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CALENDAR);
    AddUiWin(&m_Toolbar);

	m_pFestDetail = new UiFestivalInfo;
	m_pFestDetail->SetTextSize(24);
    m_pFestDetail->SetTextWeight(FW_BOLD);
	m_pFestDetail->SetID(MZ_IDC_FEST_INFO);
	AddUiWin(m_pFestDetail);

	m_pTipyiji = new UiYiJiLabel;
	m_pTipyiji->SetID(MZ_IDC_YIJI_TIP);
	m_pTipyiji->EnableNotifyMessage(true);
	m_pTipyiji->SetVisible(false);
	AddUiWin(m_pTipyiji);

	DateTime::getDate(&_year,&_month,&_day);
	updateGrid();
	updateInfo(true);

    ImagingHelper* m_selImg = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_NEW_BUILT, true);
    ImagingHelper* m_selImgPrs = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_NEW_BUILT_PRESSED, true);
    ImagingHelper* m_dImg = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_JUMPTO, true);
    ImagingHelper* m_dImgPrs = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_JUMPTO_PRESSED, true);
    ImagingHelper* m_celImg = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_REFRESH, true);
    ImagingHelper* m_celImgPrs = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_REFRESH_PRESSED, true);
    ImagingHelper* m_secImg = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_I, true);
    ImagingHelper* m_secImgPrs = m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_I_PRESSED, true);

    // 初始化 MzGridMenu 控件
    m_GridMenu.AppendMenuItem(IDC_PPM_CONFIG, L"设置", m_selImg, m_selImgPrs);
    m_GridMenu.AppendMenuItem(IDC_PPM_TODATE, L"转到日期", m_dImg, m_dImgPrs);
    m_GridMenu.AppendMenuItem(IDC_PPM_DATES, L"相隔日期", m_celImg, m_celImgPrs);
    m_GridMenu.AppendMenuItem(IDC_PPM_HISTORY, L"历史上的今天", m_secImg, m_secImgPrs);

    //获取USB消息
    UsbNotifyMsg = RegisterUsbNotifyMsg();

    ::SetTimer(m_hWnd,0x8000,500,NULL);
    return TRUE;
}

void Ui_CalendarWnd::OnTimer(UINT_PTR nIDEvent){
	switch(nIDEvent){
		case 0x8000:
			::KillTimer(m_hWnd,0x8000);
			if(AppConfig.IniFirstRun.Get()){
				Ui_InstructionWnd dlg;
				dlg.SetInstructionImage(
					ImagingHelper::GetImageObject(MzGetInstanceHandle(),IDB_PNG_INSTRUCTIONS)
					);
				RECT rcWork = MzGetWorkArea();
				dlg.Create(rcWork.left, rcWork.top+80, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork) - 150,
					m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
				int nRet = dlg.DoModal();
				if(nRet == ID_OK){
					AppConfig.IniFirstRun.Set((DWORD)0);
				}
                InstallWidget();
			}
			break;
	}
}

#include "WidgetManager.h"

void Ui_CalendarWnd::InstallWidget(){
    UINT status = 1;
    if(WidgetManager::IsInstalled()){
        UINT ver = WidgetManager::VersionNumber();
        if(ver == 0){
            status = 1;
        }else if(ver < 1000){
            status = 2;
        }else{
            status = 0;
        }
    }
    if(status == 1){
        if(::MzMessageBoxV2(m_hWnd,L"是否安装农历插件?",MZV2_MB_YESNO,true) == 1){
            if(WidgetManager::Install()){
                ::MzMessageBoxV2(m_hWnd,
                    L"安装成功，请到桌面应用农历插件",MZV2_MB_OK,true);
            }else{
                ::MzMessageBoxV2(m_hWnd,
                    L"无法安装农历插件。",MZV2_MB_OK,true);
            }
        }else{
            ::MzMessageBoxV2(m_hWnd,
                L"您也可以到设置中手动安装农历插件",MZV2_MB_OK,true);
        }
    }else if(status == 2){
        if(::MzMessageBoxV2(m_hWnd,L"是否更新农历插件?",MZV2_MB_YESNO,true) == 1){
            if(WidgetManager::Uninstall() && WidgetManager::Install()){
                ::MzMessageBoxV2(m_hWnd,
                    L"更新成功，请到桌面应用农历插件",MZV2_MB_OK,true);
            }else{
                ::MzMessageBoxV2(m_hWnd,
                    L"无法更新农历插件。",MZV2_MB_OK,true);
            }
        }else{
            ::MzMessageBoxV2(m_hWnd,
                L"您也可以到设置中手动更新农历插件",MZV2_MB_OK,true);
        }
    }
}

void Ui_CalendarWnd::updateGrid(){
	int week = (DateTime::getWeekDay(_year,_month,1)+1)%7;	//获取1号的星期
	int days = DateTime::getDays(_year,_month);
	int rows = (week+days)/7 + ((week+days)%7 ? 1 : 0);
	m_pCalendar->setGrids(rows,7);
	int t_year,t_month,t_day;
	DateTime::getDate(&t_year,&t_month,&t_day);
	wchar_t datestr[8];
	LCAL _lstm(_year,_month);

	LS24TERM_ptr p24term = _lstm.SolarTerm();
			//update lunar info
			//update 24 jieqi

	for(int i = 0; i < days; i++){
		int r = (week + i) / 7;
		int c = (week + i) % 7;
		wsprintf(datestr,L"%d",i+1);
		m_pCalendar->setText(r,c,datestr);
		_lstm.setDay(i + 1);
		m_pCalendar->setText1(r,c,_lstm.LunarDay().C_Str());
		if(t_year == _year && t_month == _month && t_day == i+1){
			//设置今天颜色
			m_pCalendar->setTextColor(r,c,RGB(192,64,64));
		}
		if(_day == i+1){
			m_pCalendar->setSelectedIndex(r,c);
			sel_row = r;
			sel_col = c;
		}
		for(int j = 0; j < 2; j++){
			if(i == (p24term[j].day - 1)){
				m_pCalendar->setText1(r,c,p24term[j].name);
				m_pCalendar->setText1Color(r,c,RGB(200,64,128));
				break;
			}
		}

        wchar_t holidayname[20];
        memset(holidayname,0,sizeof(holidayname));
        if(pfestival){
			if(!_lstm.isLunarLeapMonth()){
				LSDate l = _lstm.getLunarDate();
                if(pfestival->LunarHoliday(l.month,l.day,holidayname,1)){
                    m_pCalendar->setSignStatus(r,c,2);
                }
                pfestival->queryBirthday(l.month,l.day,true);
                //农历生日记号
                if(pfestival->BirthdaySize()){
                    m_pCalendar->setSignStatus(r,c,0);
                }
			}
		}
        if(_lstm.LunarHoliday(holidayname)){
			m_pCalendar->setText1(r,c,holidayname);
			m_pCalendar->setText1Color(r,c,RGB(64,64,255));
		}

		memset(holidayname,0,sizeof(holidayname));

		if(pfestival){
			LSDate s = _lstm.getSolarDate();
            if(pfestival->SolarHoliday(s.month,s.day,holidayname,1)){
                m_pCalendar->setSignStatus(r,c,3);
            }
            //公历生日记号
            pfestival->queryBirthday(s.month,s.day,false);
            if(pfestival->BirthdaySize()){
                m_pCalendar->setSignStatus(r,c,1);
            }
		}
		if(_lstm.SolarHoliday(holidayname)){
			m_pCalendar->setText1(r,c,holidayname);
			m_pCalendar->setText1Color(r,c,RGB(255,0,0));
		}
	}
	m_pCalendar->Invalidate();

	updateFestDetail();
}

void Ui_CalendarWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_CALENDAR:
        {
            int nIndex = lParam;
			if(nIndex == TOOLBARPRO_LEFT_TEXTBUTTON){	//今日
                if (m_GridMenu.IsContinue()){
                    m_GridMenu.EndGridMenu();
                }
                if(AppConfig.IniStartupPage.Get() == 0){
                    DateTime::getDate(&_year,&_month,&_day);
                    updateGrid();
                    updateInfo();
                    showTip();
                    Ui_TodayWnd dlg;
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
			if(nIndex == TOOLBARPRO_MIDDLE_TEXTBUTTON){	//工具
                if (m_GridMenu.IsContinue()){
                    m_GridMenu.EndGridMenu();
                }else{
                    m_GridMenu.TrackGridMenuDialog(m_hWnd, MZM_HEIGHT_TOOLBARPRO);
                }
            }
			if(nIndex == TOOLBARPRO_RIGHT_TEXTBUTTON){	//关于
                if (m_GridMenu.IsContinue()){
                    m_GridMenu.EndGridMenu();
                }
                SetWindowText(L"关于");
                UI_AboutWnd dlg;
                RECT rcWork = MzGetWorkArea();
                dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
                // set the animation of the window
                dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
                dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
                dlg.DoModal();
			}
            break;
        }
        case IDC_PPM_CONFIG:
            {
                SetWindowText(L"设置");
                Ui_ConfigWnd dlg;
                RECT rcWork = MzGetWorkArea();
                dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
                // set the animation of the window
                dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
                dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
                int ret = dlg.DoModal();
                if(ret == ID_OK){
                    _showMonthByJieqi = (AppConfig.IniJieqiOrder.Get() > 0);
                    updateGrid();
                    updateInfo(true);
                    showTip();
                }
                break;
            }
        case IDC_PPM_TODATE:
            {
                SetWindowText(L"转到日期");
                MzOpenSip(IM_SIP_MODE_DIGIT);//IM_SIP_MODE_DIGIT);
                Ui_ToDateWnd dlg;
                dlg.setSolarDate(_year,_month,_day);
                LCAL _lt = LCAL(_year,_month,_day);
                _lt.SolarToLunar();
                LSDate lunardate = _lt.getLunarDate();
                dlg.setLunarDate(lunardate.year,lunardate.month,lunardate.day);
                RECT rcWork = MzGetVisibleDesktopRect();//MzGetWorkArea();
                dlg.Create(rcWork.left, rcWork.bottom - 250, RECT_WIDTH(rcWork),250,// RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
                // set the animation of the window
                dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
                dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
                int ret = dlg.DoModal();
                if(ret == ID_OK){
                    dlg.getDate((DWORD &)_year,(DWORD &)_month,(DWORD &)_day);
                    updateGrid();
                    updateInfo();
                    showTip();
                }
                MzCloseSip();
                break;
            }
        case IDC_PPM_DATES:
            {
                SetWindowText(L"相隔日期计算");
                MzOpenSip(IM_SIP_MODE_DIGIT);//IM_SIP_MODE_DIGIT);
                Ui_LenDateWnd dlg;
                dlg.setFromDate(_year,_month,_day);
                dlg.setToDate(_year,_month,_day);
                RECT rcWork = MzGetVisibleDesktopRect();//MzGetWorkArea();
                dlg.Create(rcWork.left, rcWork.bottom - 250, RECT_WIDTH(rcWork),250,// RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
                // set the animation of the window
                dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
                dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
                int ret = dlg.DoModal();
                if(ret == ID_OK){
                    LSDate f,t;
                    dlg.getDateFrom(f.year,f.month,f.day);
                    dlg.getDateTo(t.year,t.month,t.day);
                    int of = LCAL(f.year,f.month,f.day).SolarDaysFromBaseDate();
                    int ot = LCAL(t.year,t.month,t.day).SolarDaysFromBaseDate();
                    wchar_t msgstr[128];
                    wsprintf(msgstr,L"从%d年%d月%d日\n至%d年%d月%d日\n相隔：%d 天",f.year,f.month,f.day,
                        t.year,t.month,t.day,ot - of);
                    MzCloseSip();
                    MzMessageBoxEx(m_hWnd,msgstr,L"计算相隔时间");
                }else{
                    MzCloseSip();
                }
                break;
            }
        case IDC_PPM_HISTORY:
            {
                SetWindowText(L"历史上的今天");
                UI_HistoryWnd dlg;
                RECT rcWork = MzGetWorkArea();
                dlg.setupdate(_month,_day);

                dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
                // set the animation of the window
                dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
                dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
                dlg.DoModal();
                break;
            }
        default:
            break;
    }
    SetWindowText(L"掌上农历");
}
void Ui_CalendarWnd::showTip(bool bshow){
	//
	if(bshow){
		m_pTipyiji->SetVisible(true);
	}

	if(m_pTipyiji->IsVisible()){
		RECT r = m_pTipyiji->GetRect();

		LCAL _lstm(_year,_month,_day);
		_lstm.SolarToLunar();
		CMzString yi,ji;
		bool ret = _lstm.HuangliYiJi(yi,ji);
		m_pTipyiji->setText(yi.C_Str(),ji.C_Str(),ret);
		int row,col;
		int maxrow = m_pCalendar->getRowCount();
		m_pCalendar->getSelectedIndex(row,col);
		RECT rcWork = {0,0,0,0};
		if(row < maxrow - 2){
			rcWork.top = m_pCalendar->GetPosY() + CAL_ROW_HEIGHT * (row + 1) + 8;
		}else{
			rcWork.top = m_pCalendar->GetPosY() + CAL_ROW_HEIGHT * row  - 120 + 8;
		}
		if(col < 2){
			rcWork.left += (CAL_ROW_WIDTH * col + 2);
		}else{
			rcWork.left = 480 - 320 - 10;
		}

		rcWork.right = rcWork.left + 320;
		rcWork.bottom = rcWork.top + 120;
		m_pTipyiji->SetPos(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork));
		m_pTipyiji->Invalidate();
	}
}

LRESULT Ui_CalendarWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    if(message == UsbNotifyMsg){
        INT eventType = (INT)wParam;
        switch(eventType){
            case USB_MASSSTORAGE_ATTACH:
                ::PostQuitMessage(-1);
                break;
        }
    }

    switch (message) {
        case MZ_WM_WND_ACTIVATE:
            {
                if(LOWORD(wParam)==WA_INACTIVE){
                    if (m_GridMenu.IsContinue()){
                        m_GridMenu.EndGridMenu();
                    }
                }
                break;
            }
        case MZ_WM_MOUSE_NOTIFY:
		{
			int nID = LOWORD(wParam);
			int nNotify = HIWORD(wParam);
            short x = LOWORD(lParam);
            short y = HIWORD(lParam);
			if(nID == MZ_IDC_YIJI_TIP && nNotify == MZ_MN_LBUTTONUP){
				if (!m_pTipyiji->IsMouseDownAtScrolling() && !m_pTipyiji->IsMouseMoved()) {
					if(m_pTipyiji->IsVisible()){
						m_pTipyiji->SetVisible(false);
						m_pTipyiji->Invalidate();
					}
				}
				return 0;
			}
            if (nNotify == MZ_MN_MOUSEMOVE) {
				if(!_isMouseMoving){
					_isMouseMoving = true;
					_MouseX = x;
					_MouseY = y;
					return 0;
				}
			}

            if (nNotify == MZ_MN_LBUTTONUP) {
				if(_isMouseMoving){
					_isMouseMoving = false;
					if(x > _MouseX + 200 && y < _MouseY + 100 && y > _MouseY - 100){	//右移 上一月
						DateTime::getNextDate(_year,_month);
						_day = 1;
					}
					if(x < _MouseX - 200 && y < _MouseY + 100 && y > _MouseY - 100){	//左移 下一月
						DateTime::getPreDate(_year,_month);
						_day = 1;
					}
					if(y > _MouseY + 200 && x < _MouseX + 100 && x > _MouseX - 100){	//下移 上一年
                        if(_year == 2049) return 0;
						_year++;
						_day = 1;
					}
					if(y < _MouseY - 200 && x < _MouseX + 100 && x > _MouseX - 100){	//上移 下一年
                        if(_year == 1900) return 0;
						_year--;
						_day = 1;
					}
					updateGrid();
					updateInfo();
					showTip();
					return 0;
				}

                if (nID == MZ_IDC_CALENDAR_GRID && 
                    !m_pCalendar->IsMouseDownAtScrolling() && 
                    !m_pCalendar->IsMouseMoved()) {
					int r = 0;
					int c = 0;
					bool force = false;
					if(m_pCalendar->calcSelectedIndex(x,y,r,c)){
						if(sel_row == r && sel_col == c){
							force = true;
						}
						//check if is invalid selection
						CMzString s = m_pCalendar->getText(r,c);
						_day = _wtoi(s.C_Str());
						if(s.Length() == 0){
							return 0;
						}
						updateGrid();
						updateInfo();
						showTip(force);
					}
                }
                return 0;
            }
#if 0
            if (nID == MZ_IDC_CALENDAR_GRID && nNotify == MZ_MN_LBUTTONDBLCLK) {
                if (!m_pCalendar->IsMouseDownAtScrolling() && !m_pCalendar->IsMouseMoved()) {
					int r = 0;
					int c = 0;
					if( m_pCalendar->calcSelectedIndex(x,y,r,c)){
						//check if is invalid selection
						CMzString s = m_pCalendar->getText(r,c);
						if(s.Length() == 0){
							return 0;
						}
						_day = _wtoi(s.C_Str());
						showTip(true);
					}
                }
                return 0;
            }
#endif
			break;
		}
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_CalendarWnd::updateInfo(bool forceupdate){
	static int y = 0;
	static int m = 0;
	static int d = 0;
    _showMonthByJieqi = (AppConfig.IniJieqiOrder.Get() > 0);
	if(y != _year || 
		m != _month ||
		d != _day || forceupdate){
		LCAL _lstm(_year,_month,_day);
		_lstm.SolarToLunar();
		_lstm.setLunarMonthGanZhiMode(_showMonthByJieqi);
		unsigned char zodiac;
		CMzString zodiacName = _lstm.Zodiac(&zodiac);

		//update zodiac image
        m_pZodiacImage->SetZodiac(zodiac);

		//update datetime
		wchar_t datestr[16];
		wsprintf(datestr,L"%d年%d月%d日",_year,_month,_day);
		m_YearMonth.SetText(datestr);
		m_YearMonth.Invalidate();

		//update ganzhi datetime
		wsprintf(datestr,L"%s年 %s月 %s日",
			_lstm.GanZhiYear().C_Str(),
			_lstm.GanZhiMonth().C_Str(),
			_lstm.GanZhiDay().C_Str());
		m_GanZhiYMD.SetText(datestr);
		m_GanZhiYMD.Invalidate();
		//update lunar datetime
		wsprintf(datestr,L"农历%s%s日 %s年",
			_lstm.LunarMonth().C_Str(),
			_lstm.OriginalLunarDay().C_Str(),
			zodiacName.C_Str());
		m_LunarMD.SetText(datestr);
		m_LunarMD.Invalidate();
		y = _year;
		m = _month;
		d = _day;
	}
}

void Ui_CalendarWnd::updateFestDetail(){
	int calendarBottom = m_pCalendar->GetPosY() + 
		m_pCalendar->getRowCount() * CAL_ROW_HEIGHT + 8;
	m_pFestDetail->SetPos(0,calendarBottom,GetWidth(),GetHeight() - calendarBottom - MZM_HEIGHT_TOOLBARPRO);
    m_pFestDetail->SetDate(_year,_month,_day);
}

