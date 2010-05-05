#ifndef _UI_BROWSECFG_H
#define _UI_BROWSECFG_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include <cMzConfig.h>
// Main window derived from CMzWndEx

class CalendarConfig : public AppConfigIni{
public:
	CalendarConfig()
		:AppConfigIni(L"m8calendar.ini"){
            InitIniKey();
	}
protected:
	void InitIniKey(){
		IniJieqiOrder.InitKey(L"Config",L"JieqiOrder",(DWORD)0);
        IniHistodayFontSize.InitKey(L"Config",L"HisFontSize",(DWORD)0);
        IniStartupPage.InitKey(L"Config",L"StartupPage",(DWORD)0);
		IniFirstRun.InitKey(L"Help",L"isFirstRun",1);
	}
public:
	cMzConfig IniJieqiOrder;//1: �Խ���������Ϊ��� 0: ��ũ����һΪ���
    cMzConfig IniHistodayFontSize;   //0:С 1: �� 2: ��
    cMzConfig IniStartupPage;    //0: Month 1: Today
	cMzConfig IniFirstRun;
};

class Ui_ConfigWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_ConfigWnd);
public:
	Ui_ConfigWnd();
	void updateUi();
public:
    UiToolBarPro m_Toolbar;
    UiButtonEx m_BtnStartupPage;	//��������
    UiButtonEx m_BtnJieqi;	//�������з�ʽ
    UiButtonEx m_BtnFontSize;	//��������С
    UiButtonEx m_BtnWidget;

protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
    void ShowJieqiOptionDlg();
    void ShowFontSizeOptionDlg();
    void ShowStartupOptionDlg();
    void UpdateWidgetStatus();
    UINT widget_status;
};


#endif /*_UI_BROWSECFG_H*/