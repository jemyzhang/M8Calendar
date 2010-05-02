#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <lcal.h>
#include "UiImage.h"

#define GRID_USE_UILIST 0
#if GRID_USE_UILIST
#pragma message("Grid:使用UiList")
#else
#pragma message("Grid:直接绘图")
#endif

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
	UiGrid(void);
	~UiGrid(void);
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
	void setGridAutoSize(bool);
	void setGridSize(int width, int height);
public:
	void setGrids(int nRow,int nCol);
	int getRowCount(void);
	int getColCount(void);
public:
	virtual void SetPos(int x, int y, int w, int h, UINT flags=0);
private:
	typedef struct GridAttr{
		CMzString text;	//正中
		CMzString text1;	//下一行
		CMzString text2;	//下二行
		int textSize;
		int text1Size;
		COLORREF textColor;
		COLORREF text1Color;
		bool isSelected;
	}GridAttr_t,*GridAttr_ptr;
	GridAttr_ptr *_grids;
#if GRID_USE_UILIST
	UiButton *_colList;
#endif
private:
	COLORREF _selbg, _seltxt;
	int _rows,_cols;
	bool _isAutosize;
	int _gwidth, _gheight;
	int m_nMaxX;
	int m_nMaxY;
private:
	HDC pMemDC;             //定义内存DC指针
	HBITMAP pBitmap;        //定义内存位图指针
private:
	bool checkRange(int row, int col);
};

#define CALENDAR_USE_GRID 1
#if CALENDAR_USE_GRID
#pragma message("日历:使用日历方格")
#else
#pragma message("日历:直接输入")
#endif

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
	UiStatic m_WeekBar;
	UiGrid m_Calendar;
	UiStatic m_LunarMD;	//农历月日
	UiStatic m_GanZhiYMD;	//干支年月日
	UiImage m_ZodiacImage;
	UiYiJiLabel m_Tipyiji;
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
