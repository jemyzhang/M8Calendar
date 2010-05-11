#include "ui_history.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "m8calendar.h"
#include "ui_config.h"
#include <histoday.h>

extern CalendarConfig AppConfig;
MZ_IMPLEMENT_DYNAMIC(UI_HistoryWnd)

#define MZ_IDC_TOOLBAR_ABOUT 101
#define MZ_IDC_BUTTON_DETAIL 102
#define MZ_IDC_LIST_HISTORY 103

UI_HistoryWnd::UI_HistoryWnd() {
	int m,d;
	DateTime::getDate(0,&m,&d);
	_month = m;
	_day = d;
	_isConnected = true;
	_detailViewMode = false;
}

BOOL UI_HistoryWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
    m_CaptionTitle.SetPos(0, y, GetWidth(), MZM_HEIGHT_HEADINGBAR);
    m_CaptionTitle.SetText(L"历史上的今天");
    m_CaptionTitle.SetDrawTextFormat(m_CaptionTitle.GetDrawTextFormat() | DT_END_ELLIPSIS);
    AddUiWin(&m_CaptionTitle);

    y += MZM_HEIGHT_HEADINGBAR;
    m_ListHistory.SetPos(0, y, GetWidth(), GetHeight() - y - MZM_HEIGHT_TOOLBARPRO);
	m_ListHistory.SetID(MZ_IDC_LIST_HISTORY);
	m_ListHistory.EnableNotifyMessage(true);
    AddUiWin(&m_ListHistory);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"返回");
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ABOUT);
    AddUiWin(&m_Toolbar);

	m_BtnDetail.SetText(L"查看");
	m_BtnDetail.SetPos(0,0,80,50);
	m_BtnDetail.SetTextColor(RGB(64,64,128));
	m_BtnDetail.SetButtonType(MZC_BUTTON_DOWNLOAD);
	m_BtnDetail.SetID(MZ_IDC_BUTTON_DETAIL);
	m_BtnDetail.SetVisible(false);
	m_ListHistory.setupButton(&m_BtnDetail);

	//Detail view
	y = MZM_HEIGHT_HEADINGBAR;

	m_EdtDetail.SetPos(0,y,GetWidth(),GetHeight() - y - MZM_HEIGHT_TOOLBARPRO);
	m_EdtDetail.SetReadOnly(true);
    m_EdtDetail.EnableScrollBarV(true);
    m_EdtDetail.SetTextSize(20 + 5 * AppConfig.IniHistodayFontSize.Get());
	AddUiWin(&m_EdtDetail);
	m_EdtDetail.SetVisible(false);

    SetTimer(m_hWnd,0x1001,100,NULL);

	return TRUE;
}

void UI_HistoryWnd::OnTimer(UINT nIDEvent){
	static int cnt = 0;
	switch(nIDEvent){
		case 0x1001:
            KillTimer(m_hWnd,0x1001);
            GetHistoryList();
			break;
	}
}

void UI_HistoryWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BUTTON_DETAIL:
		{
			setupDetailView(m_ListHistory.GetSelectedIndex());
			break;
		}
        case MZ_IDC_TOOLBAR_ABOUT:
        {
            int nIndex = lParam;
            if (nIndex == TOOLBARPRO_LEFT_TEXTBUTTON) {
				if(_detailViewMode){
					_detailViewMode = false;

					m_EdtDetail.SetVisible(false);
					m_EdtDetail.Invalidate();

					m_ListHistory.SetVisible(true);
					m_ListHistory.Invalidate();

                    m_CaptionTitle.SetText(L"历史上的今天");
                    m_CaptionTitle.Invalidate();

				}else{
					// exit the modal dialog
					EndModal(ID_OK);
				}
                return;
            }
        }
    }
}
void UI_HistoryWnd::setupDetailView(int idx){
	_detailViewMode = true;
	m_ListHistory.SetVisible(false);
	m_ListHistory.Invalidate();

    HistodayDB *pldb = CreateHistodayDatabase();
	//
	LPHistoryToday ph = pldb->query_at(idx);
	HistoryToday hs;
	hs.year = ph->year;
	hs.month_day = ph->month_day;
	C::newstrcpy(&hs.title,ph->title);
	hs.content = 0;
	pldb->queryDetail(&hs);

    m_CaptionTitle.SetText(hs.title);
    m_CaptionTitle.Invalidate();

	m_EdtDetail.SetText(hs.content);
	m_EdtDetail.SetVisible(true);
	m_EdtDetail.Invalidate();

	delete[] hs.title; delete[] hs.content;
}

void UI_HistoryWnd::setupdate(DWORD month,DWORD day){
	if(month == -1 || day == -1){
		int m,d;
		DateTime::getDate(0,&m,&d);
		month = m;
		day = d;
	}
	_month = month;
	_day = day;
}

void UI_HistoryWnd::GetHistoryList(){

	wchar_t currpath[128];
	
	if(File::GetCurrentPath(currpath)){
		wsprintf(db_path,L"%s\\history.db",currpath);
	}else{
		wsprintf(db_path,DEFAULT_DB);
	}

    HistodayDB *pldb = CreateHistodayDatabase();
	if(pldb->connect(db_path)){
		pldb->recover();
		if(!pldb->checkDatabaseVersion()){
			_isConnected = false;
		}
	}else{
		//检查记录版本
		pldb->recover();
	}
	//导入记录
	wchar_t db_txt[256];
	wsprintf(db_txt,L"%s\\历史上的今天.txt",currpath);
	if(!ImportData(db_txt,m_hWnd)){
		wsprintf(db_txt,L"%s\\histoday.dat",currpath);
		ImportData(db_txt,m_hWnd);
	}
   
    pldb->queryDate(_month*100 + _day);

	m_ListHistory.RemoveAll();

    ListItem li;
    CMzString str;

    for (int i = 0; i < pldb->query_size(); i++) {
        m_ListHistory.AddItem(li);
    }
	m_ListHistory.Invalidate();
}

void UiHistoryList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate){
    HistodayDB *_db = CreateHistodayDatabase();
	if(_db == 0) return;

	LPHistoryToday ph= _db->query_at(nIndex);
	if(ph == 0) return;

    // draw the high-light background for the selected item
	bool bBtnShow = false;
    if (nIndex == GetSelectedIndex()) {
        //MzDrawSelectedBg(hdcDst, prcItem);
		if(_btn){
			_btn->SetPos(prcItem->right - _btn->GetWidth() - 10,
				prcItem->top + (prcItem->bottom - prcItem->top - _btn->GetHeight())/2 - prcWin->top,
				0,0,SP_NOSIZE);
			bBtnShow = true;
		}
    }
	RECT rcText = *prcItem;
	RECT Rect01;
	RECT Rect02;
	RECT RectYear;

	//年份
	COLORREF cr = RGB(128,128,128);
	HFONT hf = FontHelper::GetFont( 18 );
	HFONT oldfont = (HFONT)SelectObject(hdcDst,hf);
	RectYear.top = rcText.top; RectYear.bottom = rcText.top + (rcText.bottom - rcText.top)/2;
	RectYear.left = rcText.left;RectYear.right = rcText.left + 80;
	wchar_t *strYear = new wchar_t[16];
	wsprintf(strYear,L"[%d年] ",ph->year);
	MzDrawText( hdcDst , strYear, &RectYear , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
    SelectObject(hdcDst,oldfont);//恢复系统字体

	//标题
	cr = RGB(0,0,0);       
	::SetTextColor( hdcDst , cr );
	hf = FontHelper::GetFont( 25 );
	oldfont = (HFONT)SelectObject(hdcDst,hf);
	Rect01.top = rcText.top; Rect01.bottom = Rect01.top + (rcText.bottom - rcText.top)/2;
	Rect01.left = rcText.left + 80;
	if(bBtnShow){
		Rect01.right = rcText.right - _btn->GetWidth() - 20;
	}else{
		Rect01.right = rcText.right;
	}
	MzDrawText( hdcDst , ph->title, &Rect01 , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
    SelectObject(hdcDst,oldfont);//恢复系统字体

	//内容
	cr = RGB(0,0,0);
	hf = FontHelper::GetFont( 18 );
	oldfont = (HFONT)SelectObject(hdcDst,hf);
	Rect02.top = Rect01.bottom; Rect02.bottom = rcText.bottom;
	Rect02.left = rcText.left;
	if(bBtnShow){
		Rect02.right = rcText.right - _btn->GetWidth() - 20;
	}else{
		Rect02.right = rcText.right;
	}
	MzDrawText( hdcDst , ph->content, &Rect02 , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
    SelectObject(hdcDst,oldfont);//恢复系统字体
}

LRESULT UI_HistoryWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_LIST_HISTORY && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_ListHistory.IsMouseDownAtScrolling() && !m_ListHistory.IsMouseMoved()) {
                    int nIndex = m_ListHistory.CalcIndexOfPos(x, y);
                    m_ListHistory.SetSelectedIndex(nIndex);
					m_BtnDetail.SetVisible(true);
                    m_ListHistory.Invalidate();
                }
                return 0;
            }
            if (nID == MZ_IDC_LIST_HISTORY && nNotify == MZ_MN_MOUSEMOVE) {
                m_ListHistory.SetSelectedIndex(-1);
				m_BtnDetail.SetVisible(false);
                m_ListHistory.Invalidate();
                return 0;
            }
            if (nID == MZ_IDC_LIST_HISTORY && nNotify == MZ_MN_LBUTTONDBLCLK) {
                if (!m_ListHistory.IsMouseDownAtScrolling() && !m_ListHistory.IsMouseMoved()) {
                    int nIndex = m_ListHistory.CalcIndexOfPos(x, y);
					if(nIndex == -1) return 0;
					//显示详情
					setupDetailView(nIndex);
				}
	            return 0;
			}
		}
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

