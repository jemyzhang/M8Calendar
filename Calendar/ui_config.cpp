#include "ui_config.h"
#include <cMzCommon.h>

using namespace cMzCommon;

#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_SCROLLWIN 102

#define MZ_IDC_BUTTON_JIEQI_MODE 103
#define MZ_IDC_LIST_CONFIG 104
#define MZ_IDC_BUTTON_FONT_SIZE 105
#define MZ_IDC_BUTTON_STARTUP   106

CalendarConfig AppConfig;

MZ_IMPLEMENT_DYNAMIC(Ui_ConfigWnd)

const wchar_t* JIEQIMODESTR[] = {
	L"农历初一",
	L"节气交界日",
};

const wchar_t* FONTSIZESTR[] = {
	L"小字体",
	L"中字体",
	L"大字体",
};

const wchar_t* StartupPageStr[] = {
	L"月历界面",
	L"今日界面",
};

Ui_ConfigWnd::Ui_ConfigWnd(){
}

BOOL Ui_ConfigWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    ImagingHelper *imgArrow = ImagingHelper::GetImageObject(GetMzResModuleHandle(), MZRES_IDR_PNG_ARROW_RIGHT, true);
    // Then init the controls & other things in the window
    int y = 0;

    m_BtnStartupPage.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnStartupPage.SetText(L"程序启动界面");
    m_BtnStartupPage.SetTextMaxLen(0);
    m_BtnStartupPage.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnStartupPage.SetID(MZ_IDC_BUTTON_STARTUP);
    m_BtnStartupPage.SetImage2(imgArrow);
    m_BtnStartupPage.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnStartupPage.SetShowImage2(true);
    AddUiWin(&m_BtnStartupPage);

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnJieqi.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnJieqi.SetText(L"干支纪月");
    m_BtnJieqi.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnJieqi.SetID(MZ_IDC_BUTTON_JIEQI_MODE);
    m_BtnJieqi.SetImage2(imgArrow);
    m_BtnJieqi.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnJieqi.SetShowImage2(true);
    AddUiWin(&m_BtnJieqi);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnFontSize.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnFontSize.SetText(L"历史上的今天字体大小");
    m_BtnFontSize.SetTextMaxLen(0);
    m_BtnFontSize.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnFontSize.SetID(MZ_IDC_BUTTON_FONT_SIZE);
    m_BtnFontSize.SetImage2(imgArrow);
    m_BtnFontSize.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnFontSize.SetShowImage2(true);
    AddUiWin(&m_BtnFontSize);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
	m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"返回");
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    AddUiWin(&m_Toolbar);

	updateUi();

    return TRUE;
}

void Ui_ConfigWnd::updateUi(){
	m_BtnJieqi.SetText2(JIEQIMODESTR[AppConfig.IniJieqiOrder.Get()]);
	m_BtnJieqi.Invalidate();

    m_BtnFontSize.SetText2(FONTSIZESTR[AppConfig.IniHistodayFontSize.Get()]);
	m_BtnFontSize.Invalidate();

    m_BtnStartupPage.SetText2(StartupPageStr[AppConfig.IniStartupPage.Get()]);
	m_BtnStartupPage.Invalidate();

}

void Ui_ConfigWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BUTTON_JIEQI_MODE:
            ShowJieqiOptionDlg();
			break;
		case MZ_IDC_BUTTON_FONT_SIZE:
            ShowFontSizeOptionDlg();
			break;
        case MZ_IDC_BUTTON_STARTUP:
            ShowStartupOptionDlg();
            break;
        case MZ_IDC_TOOLBAR_MAIN:
        {
            int nIndex = lParam;
            if (nIndex == TOOLBARPRO_LEFT_TEXTBUTTON) {
                // exit the modal dialog
                EndModal(ID_OK);
                return;
            }
            break;
		}
    }
}

void Ui_ConfigWnd::ShowJieqiOptionDlg(){
    //创建新弹出菜单对象
    MzPopupMenu TestPopupMenu;
    //设置弹出菜单的标题
    TestPopupMenu.SetMenuTitle(L"设定干支纪月方式");

    for(int i = 0; i < sizeof(JIEQIMODESTR)/sizeof(JIEQIMODESTR[0]); i++){
        TestPopupMenu.AppendMenuItem(MZV2_MID_MIN + i + 1,const_cast<LPTSTR>(JIEQIMODESTR[i]));
    }
    int result = TestPopupMenu.MzTrackPopupMenu(m_hWnd, TRUE);
    if(result > MZV2_MID_MIN){
        AppConfig.IniJieqiOrder.Set(result - MZV2_MID_MIN - 1);
        updateUi();
    }
}

//历史上的今天文字大小
void Ui_ConfigWnd::ShowFontSizeOptionDlg(){
    //创建新弹出菜单对象
    MzPopupMenu TestPopupMenu;
    //设置弹出菜单的标题
    TestPopupMenu.SetMenuTitle(L"设定浏览文字大小");

    for(int i = 0; i < sizeof(FONTSIZESTR)/sizeof(FONTSIZESTR[0]); i++){
        TestPopupMenu.AppendMenuItem(MZV2_MID_MIN + i + 1,const_cast<LPTSTR>(FONTSIZESTR[i]));
    }
    int result = TestPopupMenu.MzTrackPopupMenu(m_hWnd, TRUE);
    if(result > MZV2_MID_MIN){
        AppConfig.IniHistodayFontSize.Set(result - MZV2_MID_MIN - 1);
        updateUi();
    }
}

//启动界面
void Ui_ConfigWnd::ShowStartupOptionDlg(){
    //创建新弹出菜单对象
    MzPopupMenu TestPopupMenu;
    //设置弹出菜单的标题
    TestPopupMenu.SetMenuTitle(L"设定启动界面");

    for(int i = 0; i < sizeof(StartupPageStr)/sizeof(StartupPageStr[0]); i++){
        TestPopupMenu.AppendMenuItem(MZV2_MID_MIN + i + 1,const_cast<LPTSTR>(StartupPageStr[i]));
    }
    int result = TestPopupMenu.MzTrackPopupMenu(m_hWnd, TRUE);
    if(result > MZV2_MID_MIN){
        AppConfig.IniStartupPage.Set(result - MZV2_MID_MIN - 1);
        updateUi();
    }
}