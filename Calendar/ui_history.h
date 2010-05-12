#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

class UiHistoryList;
// Popup window derived from CMzWndEx

class UI_HistoryWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(UI_HistoryWnd);
public:
	UI_HistoryWnd();
    ~UI_HistoryWnd();
	void setupdate(DWORD month = -1,DWORD day = -1);
    void OnTimer(UINT nIDEvent);
protected:
    void GetHistoryList();
	void setupDetailView(int);
protected:
    UiToolBarPro m_Toolbar;
	UiHistoryList *m_pListHistory;
	UiButton m_BtnDetail;
    UiHeadingBar m_CaptionTitle;
//detail view
	UiEdit m_EdtDetail;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	wchar_t db_path[256];
	bool _isConnected;
	DWORD _month,_day;
	bool _detailViewMode;
};
