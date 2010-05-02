#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <list>

#define MZ_MW_TEXT_EDITED MZFC_WM_MESSAGE+0x80

class UiNumericEdit : public UiSingleLineEdit
{
protected:
	virtual int OnChar(TCHAR chCharCode, LPARAM lKeyData){
		if((chCharCode > '9' || chCharCode < '0') && chCharCode != 0x8) return 0;
		int ret = UiSingleLineEdit::OnChar(chCharCode,lKeyData);
		PostUiMessage(GetParent(),GetID());
		return ret;
	}
	virtual void OnFocused(UiWin* pWinPrev){
		preStr = GetText();
		SetText(L"\0");
		Invalidate();
		UiSingleLineEdit::OnFocused(pWinPrev);
	}
	virtual void OnLostFocus(UiWin* pWinNext){
		if(GetText().IsEmpty() && !preStr.IsEmpty()){
			SetText(preStr.C_Str());
			Invalidate();
		}
		UiSingleLineEdit::OnLostFocus(pWinNext);
	}
private:
	CMzString preStr;
};

class UiDateEdit : public UiWin
{
public:
	UiDateEdit(void){
		_y = -1;
		_m = -1;
		_d = -1;
	}
	~UiDateEdit(void) {	}
	void init();
	void setDate(DWORD year, DWORD month, DWORD day){
		_y = year;
		_m = month;
		_d = day;
	}
	void getDate(DWORD &year, DWORD &month, DWORD &day){
		_y = _wtoi(m_EdtCalendarYear.GetText().C_Str());
		_m = _wtoi(m_EdtCalendarMonth.GetText().C_Str());
		_d = _wtoi(m_EdtCalendarDay.GetText().C_Str());
		year = _y;
		month = _m;
		day = _d;
	}
	/// called when the control lost keyboard focus
//	virtual void OnLostFocus(UiWin* pWinNext){
//		checkDateText();
//	}
	/// update the control
    virtual void Invalidate(RECT* prcUpdate=NULL){
		updateDateText();
        UiWin::Invalidate(prcUpdate);
	}
    ///// update the control
    //virtual void Update(){
    //    UiWin::Update();
    //}

    // override the MZFC window messages handler
	virtual void UiWinProc(LPARAM lParam){
		if(!checkDateText()){
			PostMessageW(MZ_MW_TEXT_EDITED,GetID(),0);
		}
		UiWin::UiWinProc(lParam);
	}
private:
	void updateDateText(){
		if(_y == -1 || _m == -1 || _d == -1){
			SYSTEMTIME sysTime;
			GetLocalTime(&sysTime);
			_y = sysTime.wYear;
			_m = sysTime.wMonth;
			_d = sysTime.wDay;
		}
		wchar_t s[5];
		wsprintf(s, L"%04d", _y);
		m_EdtCalendarYear.SetText(s);
		wsprintf(s, L"%02d", _m);
		m_EdtCalendarMonth.SetText(s);
		wsprintf(s, L"%02d", _d);
		m_EdtCalendarDay.SetText(s);
		m_EdtCalendarYear.Invalidate();
		m_EdtCalendarMonth.Invalidate();
		m_EdtCalendarDay.Invalidate();
	}
public:
	bool checkDateText();
public:
    UiNumericEdit m_EdtCalendarYear;
    UiNumericEdit m_EdtCalendarMonth;
    UiNumericEdit m_EdtCalendarDay;
private:
	DWORD _y,_m,_d;
};

class UiDateEditBG : public UiWin
{
public:
	UiDateEditBG(void){}
	~UiDateEditBG(void) {}
protected:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
		MzDrawGridPopupMenuBG_480(hdcDst,prcWin);
	}
};

class Ui_ToDateWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_ToDateWnd);
public:
	Ui_ToDateWnd(void);
	~Ui_ToDateWnd(void);
public:
	UiDateEditBG m_bg;
	UiStatic m_LblSolarDate;
	UiStatic m_LblLunarDate;
	UiDateEdit m_EdtSolarDate;
	UiDateEdit m_EdtLunarDate;
	UiButton m_BtnDateTo;
	UiButton m_BtnCancel;
public:
	void setSolarDate(DWORD year, DWORD month, DWORD day){
		m_EdtSolarDate.setDate(year,month,day);
		m_EdtSolarDate.Invalidate();
	}
	void setLunarDate(DWORD year, DWORD month, DWORD day){
		m_EdtLunarDate.setDate(year,month,day);
		m_EdtLunarDate.Invalidate();
	}
	void getDate(DWORD &year, DWORD &month, DWORD &day);
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
};

class Ui_LenDateWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_LenDateWnd);
public:
	Ui_LenDateWnd(void);
	~Ui_LenDateWnd(void);
public:
	UiDateEditBG m_bg;
	UiStatic m_LblFromDate;
	UiStatic m_LblToDate;
	UiDateEdit m_EdtFromDate;
	UiDateEdit m_EdtToDate;
	UiButton m_BtnDateLen;
	UiButton m_BtnCancel;
public:
	void setFromDate(DWORD year, DWORD month, DWORD day){
		m_EdtFromDate.setDate(year,month,day);
		m_EdtFromDate.Invalidate();
	}
	void setToDate(DWORD year, DWORD month, DWORD day){
		m_EdtToDate.setDate(year,month,day);
		m_EdtToDate.Invalidate();
	}
	void getDateFrom(DWORD &year, DWORD &month, DWORD &day);
	void getDateTo(DWORD &year, DWORD &month, DWORD &day);
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
};
