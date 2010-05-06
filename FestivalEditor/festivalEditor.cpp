// ����MZFC���ͷ�ļ�
#include <mzfc_inc.h>

#include "UiFestivalList.h"
#include "ui_festivaledit.h"
#include <UsbNotifyApi.h>

#define MZ_IDC_BUTTONBAR_MAIN	101
#define MZ_IDC_BUTTONBAR_SUB	102
#define MZ_IDC_FESTLIST     103
#define MZ_IDC_TOOLBAR	104

class UiFestivalList : public UiCustomList {
public:
};

// �� CMzWndEx ��������������
class FestivalEditorWnd : public CMzWndEx
{
    MZ_DECLARE_DYNAMIC(FestivalEditorWnd);
public:
    FestivalEditorWnd(){
        m_plist = 0;
        m_pButtonBar0 = 0;
        m_pButtonBar1 = 0;
        m_ptoolBar = 0;
    }
    ~FestivalEditorWnd(){
        if(m_plist) delete m_plist;
        if(m_pButtonBar0) delete m_pButtonBar0;
        if(m_pButtonBar1) delete m_pButtonBar1;
        if(m_ptoolBar) delete m_ptoolBar;
    }
public:
    UiFestivalList *m_plist;
    UiButtonBar *m_pButtonBar0;
    UiButtonBar *m_pButtonBar1;
    UiToolBarPro *m_ptoolBar;
private:
    FestivalItemCollection *pItemColl;
    bool delmode;
    bool bChanged;  //�༭/����/ɾ����true
	UINT UsbNotifyMsg;
private:
    enum tagDPStatus{
        DP_STATUS_OK    =   0,
        DP_NOT_INSTALL  =   1,
        DP_NOT_SHOW     =   2,
        DP_OLD_VERSION  =   3,
        DP_ERR_OTHER    =   4,
    };
    //����ע�������ȷ���������Ƿ�װ
    //1: Not Install
    //2: Not Install to Desktop
    //3: Version too old
    //4: Other
    int CheckDeskPlugin(){
        HKEY hKEY;
        HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
        //1: ����������Ƿ��Ѿ�����
        long ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\iDapRc\\M8Calendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//����޷���hKEY,����ֹ�����ִ��
        {
            RegCloseKey(hKEY);
            return DP_NOT_INSTALL;
        }
        DWORD RegType;
        DWORD RegData;
        DWORD RegDataLen = 4;
        //2: ��ȡ��������������   //������������ڣ�����ʾ��Ҫ����������
        if(::RegQueryValueEx(hKEY,
            L"FestRefreshReq",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                RegCloseKey(hKEY);
                return DP_OLD_VERSION;
        }
        if(RegType != REG_DWORD){ //�������ʹ���
            RegCloseKey(hKEY);
            return DP_ERR_OTHER;
        }
        RegCloseKey(hKEY);

        ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//����޷���hKEY,����ֹ�����ִ��
        {
            RegCloseKey(hKEY);
            return DP_NOT_INSTALL;
        }
        if(::RegQueryValueEx(hKEY,
            L"IsHide",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                RegCloseKey(hKEY);
                return DP_NOT_INSTALL;
        }
        if(RegType != REG_DWORD){ //�������ʹ���
            RegCloseKey(hKEY);
            return DP_ERR_OTHER;
        }
        if(RegData == 1){    //�������״̬
            RegCloseKey(hKEY);
            return DP_NOT_SHOW;
        }
        RegCloseKey(hKEY);
        return DP_STATUS_OK;
    }

    #define REQ_RELOADFEST 0x9481
    bool PostDPRequest(){
        HKEY hKEY;
        HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
        long ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\iDapRc\\M8Calendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//����޷���hKEY,����ֹ�����ִ��
        {
            RegCloseKey(hKEY);
            return false;
        }
        DWORD req = REQ_RELOADFEST;
        if(::RegSetValueEx(hKEY,
            L"FestRefreshReq",NULL,REG_DWORD,(LPBYTE)&req,4) != ERROR_SUCCESS){
                RegCloseKey(hKEY);
                return false;
        }
        RegCloseKey(hKEY);
        return true;
    }

    void UpdateSubButtonBar(int index,int selected = 0){
        struct BtnBar{
            int size;
            LPCTSTR texts[4];
        }btnbar[] = {
            {2, {L"ũ������",L"��������"}},
            {4, {L"ũ������",L"��������",L"ũ������",L"��������"}},
            {4, {L"������",L"������",L"����ʱ",L"��ʱ"}},
        };
        if(index >= sizeof(btnbar)/sizeof(btnbar[0])) index = 0;
        if(selected >= btnbar[index].size) selected = 0;
        m_pButtonBar1->SetButtonCount(btnbar[index].size);
        m_pButtonBar1->SetHighLightButton(selected);
        for(int i = 0; i < btnbar[index].size; i++){
            m_pButtonBar1->SetButton(i,true,true,btnbar[index].texts[i]);
        }
        pItemColl->SetFestivalCollectionType(
            (FestivalListType)(((index<<4)&0xf0) | selected )
            );
        m_plist->ScrollTo(UI_SCROLLTO_TOP,0,false);
//        m_plist->Invalidate();
    }

    void DP_InitCheck(){
        int nRet = CheckDeskPlugin();
#if 0
        if(nRet == DP_NOT_INSTALL){
            MzMessageBoxV2(m_hWnd,L"����: ũ��������δ��װ��",MZV2_MB_OK,true);
            ::PostQuitMessage(0);        
        }
#endif
        if(DP_OLD_VERSION == nRet){
            MzMessageBoxV2(m_hWnd,L"����: ũ���������汾̫�ͣ���֧������ˢ�½��չ��ܣ��뾡�����������°档",MZV2_MB_OK,true);
        }
    }
protected:
    // ���ڳ�ʼ��
    virtual BOOL OnInitDialog()
    {
        // �����ȵ��û���ĳ�ʼ��
        if (!CMzWndEx::OnInitDialog())
        {
	        return FALSE;
        }
        int y = 0;
        // ��ʼ�� UiButtonBar �ؼ�
        m_pButtonBar0 = new UiButtonBar;
        m_pButtonBar0->SetID(MZ_IDC_BUTTONBAR_MAIN);
        m_pButtonBar0->SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONBAR);
        m_pButtonBar0->SetButton(0, true, true, L"����");
        m_pButtonBar0->SetButton(1, true, true, L"�ڼ���");
        m_pButtonBar0->SetButton(2, true, true, L"����");
        AddUiWin(m_pButtonBar0);

        y += MZM_HEIGHT_BUTTONBAR;
        m_pButtonBar1 = new UiButtonBar;
        m_pButtonBar1->SetID(MZ_IDC_BUTTONBAR_SUB);
        m_pButtonBar1->SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONBAR);
        AddUiWin(m_pButtonBar1);

        y += MZM_HEIGHT_BUTTONBAR;
        m_plist = new UiFestivalList;
        // ��ʼ�� UiCustomList �ؼ�
        m_plist->SetPos(0, y, 480, GetHeight() - y - MZM_HEIGHT_TOOLBARPRO);
        m_plist->SetID(MZ_IDC_FESTLIST);
        //m_plist->EnableGridlines(true);
        m_plist->EnableDragModeH(true);
        m_plist->SetItemAttribute(UILISTEX_ITEMTYPE_PHONE);
        m_plist->UpdateItemAttribute_Del();
        m_plist->SetSplitLineMode(UILISTEX_SPLITLINE_LEFT);
        //m_plist->EnablePressedHoldSupport(true);
        m_plist->EnableScrollBarV(true);
        //m_plist->EnableUltraGridlines(false);
        //m_plist->SetSelectMode(UILISTEX_SELECT_DELETE_PRESSED);
        AddUiWin(m_plist);

        // ��ʼ�� UiToolBarPro �ؼ�
        m_ptoolBar = new UiToolBarPro;
        m_ptoolBar->SetID(MZ_IDC_TOOLBAR);
        m_ptoolBar->SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
        m_ptoolBar->SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"ѡ��");
        m_ptoolBar->SetMiddleButton(true, true, L"�½�",NULL,NULL,NULL);
        m_ptoolBar->SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON, true, true, L"�˳�");
        AddUiWin(m_ptoolBar);

        pItemColl = new FestivalItemCollection;
        m_plist->SetCustomItemCollection(pItemColl);

        UpdateSubButtonBar(0,0);

        DP_InitCheck();
        delmode = false;
        bChanged = false;
		//��ȡUSB��Ϣ
		UsbNotifyMsg = RegisterUsbNotifyMsg();

		return TRUE;
    }

    void UpdateToolbar(){
        if(!delmode){
            m_ptoolBar->SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,L"ѡ��");
            m_ptoolBar->ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,true);
            if(bChanged){
                m_ptoolBar->SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,L"����");
            }else{
                m_ptoolBar->SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,L"�˳�");
            }
            m_ptoolBar->Invalidate();
        }else{
            m_ptoolBar->SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,L"����");
            m_ptoolBar->ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,false);
            m_ptoolBar->SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,L"ɾ��");
        }
    }

    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam){
		if(message == UsbNotifyMsg){
			INT eventType = (INT)wParam;
			switch(eventType){
			case USB_MASSSTORAGE_ATTACH:
				::PostQuitMessage(-1);
				break;
			}
		}
        switch(message){
        case MZ_WM_UILIST_LBUTTONUP_SELECT:
            if(wParam == MZ_IDC_FESTLIST){
                int index = LOWORD(lParam);
                int area = HIWORD(lParam);
                if(area == 2){
                    FestivalItemCollection * pl = 
                        static_cast<FestivalItemCollection*>(m_plist->GetCustomItemCollection());
                    Ui_FestivalEdit m_Editor;
                    int nRet = m_Editor.ShowModifyDialog(pl->fio->query_at(index),m_hWnd);
                    if(nRet == ID_OK){
                        ::MzMessageAutoBoxV2(m_hWnd,L"�����޸����",MZV2_MB_NONE,1000,true);
                        m_plist->Invalidate();
                        bChanged = true;
                        UpdateToolbar();
                    }
                }
            }
            break;
        default:
            break;
        }
        return CMzWndEx::MzDefWndProc(message,wParam,lParam);
    }

    // ���� MZFC ��������Ϣ������
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam)
    {
        UINT_PTR id = LOWORD(wParam);
        switch(id)
        {
	    // ���� UiButtonBar ����Ϣ
        case MZ_IDC_BUTTONBAR_MAIN:
            {
                int index = lParam;
                UpdateSubButtonBar(index,0);
                Invalidate();
                break;
            }
        case MZ_IDC_BUTTONBAR_SUB:
            {
                int index = lParam;
                UpdateSubButtonBar(m_pButtonBar0->GetHighLightButton(),index);
                Invalidate();
                break;
            }
        // ���� UiToolBarPro ����Ϣ
        case MZ_IDC_TOOLBAR:
            {
                int index = lParam;
                if (index == TOOLBARPRO_LEFT_TEXTBUTTON)    //ѡ��/����
                {
                    if (!delmode)
                    {
                        delmode = true;
                        m_plist->SetMultiSelectMode(UILISTEX_MULTISELECT_LEFT);
                        Invalidate();
                    }else{
                        delmode = false;
                        m_plist->SetMultiSelectMode();
                        m_plist->SetSelectedIndex(-1);
                        Invalidate();
                        //UpdateWindow();
                    }
                    UpdateToolbar();
                }else if (index == TOOLBARPRO_MIDDLE_TEXTBUTTON){
                    Ui_FestivalEdit m_Editor;
                    Festival f;
                    if(m_pButtonBar0->GetHighLightButton() == 2){
                        f.type = FestivalSolarRemind;
                        f.info1.type = (ReminderType)(m_pButtonBar1->GetHighLightButton() + 1);
                    }else if(m_pButtonBar0->GetHighLightButton() == 0){
                        f.type = (FestivalType)(FestivalLunarBirth + m_pButtonBar1->GetHighLightButton());
                    }else if(m_pButtonBar0->GetHighLightButton() == 1){
                        f.type = (FestivalType)(FestivalLunar + m_pButtonBar1->GetHighLightButton());
                    }
                    int nRet = m_Editor.ShowAppendDialog(&f,m_hWnd);
                    if(nRet == ID_OK){
                        FestivalItemCollection * pl = 
                            static_cast<FestivalItemCollection*>(m_plist->GetCustomItemCollection());
                        if(pl->AppendItem(&f)){
                            ::MzMessageAutoBoxV2(m_hWnd,L"�½��������",MZV2_MB_NONE,1000,true);
                            m_plist->Invalidate();
                            bChanged = true;
                            UpdateToolbar();
                        }else{
                            ::MzMessageAutoBoxV2(m_hWnd,L"�½�����ʧ��",MZV2_MB_NONE,1000,true);
                        }
                    }
                }else if (index == TOOLBARPRO_RIGHT_TEXTBUTTON){
                    if (delmode)
                    {
                        IItemCollection* pItemCollection = m_plist->GetCustomItemCollection();
                        for (int i = 0; i < m_plist->GetItemCount();)
                        {
                            if (pItemCollection->IsMultiSelected(i))
                            {
                                pItemCollection->RemoveItem(i);
                            }
                            else
                            {
                                i++;
                            }
                        }
                        m_plist->Invalidate();
                        bChanged = true;
                        UpdateToolbar();
                    }else{  //����
                        if(bChanged){
                            bChanged = false;
                            FestivalItemCollection * pl = 
                                static_cast<FestivalItemCollection*>(m_plist->GetCustomItemCollection());
                            if(pl->SaveList()){
                                if(CheckDeskPlugin() == DP_STATUS_OK){
                                    if(::MzMessageBoxV2(m_hWnd,
                                        L"������ϣ��Ƿ�����ˢ��ũ�������",
                                        MZV2_MB_YESNO,true) == 1){
                                            PostDPRequest();
                                    }
                                }else{
                                    ::MzMessageAutoBoxV2(m_hWnd,L"�������",MZV2_MB_NONE,1000,true);
                                }
                            }
                            UpdateToolbar();
                        }else{
                            PostQuitMessage(0);
                        }
                    }
                }
            break;
            }
        }
    }
};

MZ_IMPLEMENT_DYNAMIC(FestivalEditorWnd);

// �� CMzApp ������Ӧ�ó�����
class CSampleMainApp : public CMzApp
{
public:
    // Ӧ�ó����������
    FestivalEditorWnd m_MainWnd;

    // Ӧ�ó���ĳ�ʼ��
    virtual BOOL Init()
    {
        // ��ʼ�� COM ���
        CoInitializeEx(0, COINIT_MULTITHREADED);

        HANDLE m_hCHDle = CreateMutex(NULL,true,L"FestivalEditor");
        if(GetLastError() == ERROR_ALREADY_EXISTS)
        {
            HWND pWnd=FindWindow(m_MainWnd.GetMzClassName(),NULL);
            if(pWnd)
            {
                SetForegroundWindow(pWnd);
                PostMessage(pWnd,WM_NULL,NULL,NULL);
            }
            PostQuitMessage(0);
            return true; 
        }
        // ����������
        RECT rcWork = MzGetWorkArea();
        m_MainWnd.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);
        m_MainWnd.SetBgColor(MzGetThemeColor(TCI_WINDOW_BG));
        m_MainWnd.SetWindowText(L"���ձ༭��");
        m_MainWnd.Show();

        // �ɹ��򷵻� TRUE
        return TRUE;
    }
};

// ȫ�ֵ�Ӧ�ó������
CSampleMainApp theApp;