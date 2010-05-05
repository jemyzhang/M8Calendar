#include "ui_config.h"
#include <cMzCommon.h>
using namespace cMzCommon;

#include "WidgetManager.h"

#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_SCROLLWIN 102

#define MZ_IDC_BUTTON_JIEQI_MODE 103
#define MZ_IDC_LIST_CONFIG 104
#define MZ_IDC_BUTTON_FONT_SIZE 105
#define MZ_IDC_BUTTON_STARTUP   106
#define MZ_IDC_BUTTON_WIDGET 107

CalendarConfig AppConfig;

MZ_IMPLEMENT_DYNAMIC(Ui_ConfigWnd)

const wchar_t* JIEQIMODESTR[] = {
	L"ũ����һ",
	L"����������",
};

const wchar_t* FONTSIZESTR[] = {
	L"С����",
	L"������",
	L"������",
};

const wchar_t* StartupPageStr[] = {
	L"��������",
	L"���ս���",
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
    m_BtnStartupPage.SetText(L"������������");
    m_BtnStartupPage.SetTextMaxLen(0);
    m_BtnStartupPage.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnStartupPage.SetID(MZ_IDC_BUTTON_STARTUP);
    m_BtnStartupPage.SetImage2(imgArrow);
    m_BtnStartupPage.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnStartupPage.SetShowImage2(true);
    AddUiWin(&m_BtnStartupPage);

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnJieqi.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnJieqi.SetText(L"��֧����");
    m_BtnJieqi.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnJieqi.SetID(MZ_IDC_BUTTON_JIEQI_MODE);
    m_BtnJieqi.SetImage2(imgArrow);
    m_BtnJieqi.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnJieqi.SetShowImage2(true);
    AddUiWin(&m_BtnJieqi);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnFontSize.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnFontSize.SetText(L"��ʷ�ϵĽ��������С");
    m_BtnFontSize.SetTextMaxLen(0);
    m_BtnFontSize.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnFontSize.SetID(MZ_IDC_BUTTON_FONT_SIZE);
    m_BtnFontSize.SetImage2(imgArrow);
    m_BtnFontSize.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnFontSize.SetShowImage2(true);
    AddUiWin(&m_BtnFontSize);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnWidget.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnWidget.SetTextMaxLen(0);
    m_BtnWidget.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnWidget.SetID(MZ_IDC_BUTTON_WIDGET);
    m_BtnWidget.SetImage2(imgArrow);
    m_BtnWidget.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnWidget.SetShowImage2(true);
    AddUiWin(&m_BtnWidget);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
	m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"����");
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    AddUiWin(&m_Toolbar);

	updateUi();
    UpdateWidgetStatus();

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

void Ui_ConfigWnd::UpdateWidgetStatus(){
    widget_status = 1;
    if(WidgetManager::IsInstalled()){
        UINT ver = WidgetManager::VersionNumber();
        if(ver == 0){
            widget_status = 1;
        }else if(ver < 1000){
            widget_status = 2;
        }else{
            widget_status = 0;
        }
    }

    if(widget_status == 0){    //uninstall
        m_BtnWidget.SetText(L"ж��ũ�����");
    }else if(widget_status == 1){ //install
        m_BtnWidget.SetText(L"��װũ�����");
    }else{ //update
        m_BtnWidget.SetText(L"����ũ�����");
    }
    m_BtnWidget.Invalidate();
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
        case MZ_IDC_BUTTON_WIDGET:
        {
            if(widget_status == 0){
                if(::MzMessageBoxV2(m_hWnd,L"ȷʵҪж��ũ�����?",MZV2_MB_YESNO,true) == 1){
                    if(WidgetManager::Uninstall()){
                        ::MzMessageBoxV2(m_hWnd,
                            L"ũ�����ж�سɹ���",MZV2_MB_OK,true);
                    }else{
                        ::MzMessageBoxV2(m_hWnd,
                            L"�޷�ж��ũ�������",MZV2_MB_OK,true);
                    }
                }
            }else if(widget_status == 1){
                if(::MzMessageBoxV2(m_hWnd,L"ȷʵҪ��װũ�����?",MZV2_MB_YESNO,true) == 1){
                    if(WidgetManager::Install()){
                        ::MzMessageBoxV2(m_hWnd,
                            L"��װ�ɹ����뵽����Ӧ��ũ�����",MZV2_MB_OK,true);
                    }else{
                        ::MzMessageBoxV2(m_hWnd,
                            L"�޷���װũ�������",MZV2_MB_OK,true);
                    }
                }
            }else{
                if(::MzMessageBoxV2(m_hWnd,L"ȷʵҪ����ũ�����?",MZV2_MB_YESNO,true) == 1){
                    if(WidgetManager::Uninstall() &&  WidgetManager::Install()){
                        ::MzMessageBoxV2(m_hWnd,
                            L"���³ɹ����뵽����Ӧ��ũ�����",MZV2_MB_OK,true);
                    }else{
                         ::MzMessageBoxV2(m_hWnd,
                            L"�޷�����ũ�������",MZV2_MB_OK,true);
                   }
                }
            }
            UpdateWidgetStatus();
        }
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
    //�����µ����˵�����
    MzPopupMenu TestPopupMenu;
    //���õ����˵��ı���
    TestPopupMenu.SetMenuTitle(L"�趨��֧���·�ʽ");

    for(int i = 0; i < sizeof(JIEQIMODESTR)/sizeof(JIEQIMODESTR[0]); i++){
        TestPopupMenu.AppendMenuItem(MZV2_MID_MIN + i + 1,const_cast<LPTSTR>(JIEQIMODESTR[i]));
    }
    int result = TestPopupMenu.MzTrackPopupMenu(m_hWnd, TRUE);
    if(result > MZV2_MID_MIN){
        AppConfig.IniJieqiOrder.Set(result - MZV2_MID_MIN - 1);
        updateUi();
    }
}

//��ʷ�ϵĽ������ִ�С
void Ui_ConfigWnd::ShowFontSizeOptionDlg(){
    //�����µ����˵�����
    MzPopupMenu TestPopupMenu;
    //���õ����˵��ı���
    TestPopupMenu.SetMenuTitle(L"�趨������ִ�С");

    for(int i = 0; i < sizeof(FONTSIZESTR)/sizeof(FONTSIZESTR[0]); i++){
        TestPopupMenu.AppendMenuItem(MZV2_MID_MIN + i + 1,const_cast<LPTSTR>(FONTSIZESTR[i]));
    }
    int result = TestPopupMenu.MzTrackPopupMenu(m_hWnd, TRUE);
    if(result > MZV2_MID_MIN){
        AppConfig.IniHistodayFontSize.Set(result - MZV2_MID_MIN - 1);
        updateUi();
    }
}

//��������
void Ui_ConfigWnd::ShowStartupOptionDlg(){
    //�����µ����˵�����
    MzPopupMenu TestPopupMenu;
    //���õ����˵��ı���
    TestPopupMenu.SetMenuTitle(L"�趨��������");

    for(int i = 0; i < sizeof(StartupPageStr)/sizeof(StartupPageStr[0]); i++){
        TestPopupMenu.AppendMenuItem(MZV2_MID_MIN + i + 1,const_cast<LPTSTR>(StartupPageStr[i]));
    }
    int result = TestPopupMenu.MzTrackPopupMenu(m_hWnd, TRUE);
    if(result > MZV2_MID_MIN){
        AppConfig.IniStartupPage.Set(result - MZV2_MID_MIN - 1);
        updateUi();
    }
}