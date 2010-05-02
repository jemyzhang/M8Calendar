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
class CSampleMainWnd : public CMzWndEx
{
    MZ_DECLARE_DYNAMIC(CSampleMainWnd);
public:
    UiFestivalList m_list;
    UiButtonBar m_ButtonBar0;
    UiButtonBar m_ButtonBar1;
    UiToolBarPro m_toolBar;
private:
    FestivalItemCollection *pItemColl;
    bool delmode;
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
            L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
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
            L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
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
        m_ButtonBar1.SetButtonCount(btnbar[index].size);
        m_ButtonBar1.SetHighLightButton(selected);
        for(int i = 0; i < btnbar[index].size; i++){
            m_ButtonBar1.SetButton(i,true,true,btnbar[index].texts[i]);
        }
        pItemColl->SetFestivalCollectionType(
            (FestivalListType)(((index<<4)&0xf0) | selected )
            );
        m_list.ScrollTo(UI_SCROLLTO_TOP,0,false);
//        m_list.Invalidate();
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
        m_ButtonBar0.SetID(MZ_IDC_BUTTONBAR_MAIN);
        m_ButtonBar0.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONBAR);
        m_ButtonBar0.SetButton(0, true, true, L"����");
        m_ButtonBar0.SetButton(1, true, true, L"�ڼ���");
        m_ButtonBar0.SetButton(2, true, true, L"����");
        AddUiWin(&m_ButtonBar0);

        y += MZM_HEIGHT_BUTTONBAR;
        m_ButtonBar1.SetID(MZ_IDC_BUTTONBAR_SUB);
        m_ButtonBar1.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONBAR);
        AddUiWin(&m_ButtonBar1);

        y += MZM_HEIGHT_BUTTONBAR;
        // ��ʼ�� UiCustomList �ؼ�
        m_list.SetPos(0, y, 480, GetHeight() - y - MZM_HEIGHT_TOOLBARPRO);
        m_list.SetID(MZ_IDC_FESTLIST);
        //m_list.EnableGridlines(true);
        m_list.EnableDragModeH(true);
        m_list.SetItemAttribute(UILISTEX_ITEMTYPE_PHONE);
        m_list.UpdateItemAttribute_Del();
        m_list.SetSplitLineMode(UILISTEX_SPLITLINE_LEFT);
        //m_list.EnablePressedHoldSupport(true);
        m_list.EnableScrollBarV(true);
        //m_list.EnableUltraGridlines(false);
        //m_list.SetSelectMode(UILISTEX_SELECT_DELETE_PRESSED);
        AddUiWin(&m_list);

        // ��ʼ�� UiToolBarPro �ؼ�
        m_toolBar.SetID(MZ_IDC_TOOLBAR);
        m_toolBar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
        m_toolBar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"ѡ��");
        m_toolBar.SetMiddleButton(true, true, L"�½�",NULL,NULL,NULL);
        m_toolBar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON, true, true, L"����");
        AddUiWin(&m_toolBar);

        pItemColl = new FestivalItemCollection;
        m_list.SetCustomItemCollection(pItemColl);

        UpdateSubButtonBar(0,0);

        DP_InitCheck();
        delmode = false;
		//��ȡUSB��Ϣ
		UsbNotifyMsg = RegisterUsbNotifyMsg();

		return TRUE;
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
                        static_cast<FestivalItemCollection*>(m_list.GetCustomItemCollection());
                    Ui_FestivalEdit m_Editor;
                    int nRet = m_Editor.ShowModifyDialog(pl->fio->query_at(index),m_hWnd);
                    if(nRet == ID_OK){
                        ::MzMessageAutoBoxV2(m_hWnd,L"�����޸����",MZV2_MB_NONE,1000,true);
                        m_list.Invalidate();
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
                UpdateSubButtonBar(m_ButtonBar0.GetHighLightButton(),index);
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
                        m_list.SetMultiSelectMode(UILISTEX_MULTISELECT_LEFT);

                        m_toolBar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,L"����");
                        m_toolBar.ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,false);
                        m_toolBar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,L"ɾ��");
                        Invalidate();
                        //UpdateWindow();
                    }
                    else
                    {
                        delmode = false;
                        m_list.SetMultiSelectMode();
                        m_list.SetSelectedIndex(-1);
                        m_toolBar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,L"ѡ��");
                        m_toolBar.ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,true);
                        m_toolBar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,L"����");
                        Invalidate();
                        //UpdateWindow();
                    }   
                }else if (index == TOOLBARPRO_MIDDLE_TEXTBUTTON){
                    Ui_FestivalEdit m_Editor;
                    Festival f;
                    if(m_ButtonBar0.GetHighLightButton() == 2){
                        f.type = FestivalSolarRemind;
                        f.info1.type = (ReminderType)(m_ButtonBar1.GetHighLightButton() + 1);
                    }else if(m_ButtonBar0.GetHighLightButton() == 0){
                        f.type = (FestivalType)(FestivalLunarBirth + m_ButtonBar1.GetHighLightButton());
                    }else if(m_ButtonBar0.GetHighLightButton() == 1){
                        f.type = (FestivalType)(FestivalLunar + m_ButtonBar1.GetHighLightButton());
                    }
                    int nRet = m_Editor.ShowAppendDialog(&f,m_hWnd);
                    if(nRet == ID_OK){
                        FestivalItemCollection * pl = 
                            static_cast<FestivalItemCollection*>(m_list.GetCustomItemCollection());
                        if(pl->AppendItem(&f)){
                            ::MzMessageAutoBoxV2(m_hWnd,L"�½��������",MZV2_MB_NONE,1000,true);
                            m_list.Invalidate();
                        }else{
                            ::MzMessageAutoBoxV2(m_hWnd,L"�½�����ʧ��",MZV2_MB_NONE,1000,true);
                        }
                    }
                }else if (index == TOOLBARPRO_RIGHT_TEXTBUTTON){
                    if (delmode)
                    {
                        IItemCollection* pItemCollection = m_list.GetCustomItemCollection();
                        for (int i = 0; i < m_list.GetItemCount();)
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
                        m_list.Invalidate();
                    }else{  //����
                       FestivalItemCollection * pl = 
                           static_cast<FestivalItemCollection*>(m_list.GetCustomItemCollection());
                       if(pl->SaveList()){
                           if(CheckDeskPlugin() == DP_STATUS_OK){
                               if(::MzMessageBoxV2(m_hWnd,
                                   L"������ϣ��Ƿ�����ˢ��ũ����������",
                                   MZV2_MB_YESNO,true) == 1){
                                       PostDPRequest();
                               }
                           }else{
                               ::MzMessageAutoBoxV2(m_hWnd,L"�������",MZV2_MB_NONE,1000,true);
                           }
                       }
                    }
                }
            break;
            }
        }
    }
};

MZ_IMPLEMENT_DYNAMIC(CSampleMainWnd);

// �� CMzApp ������Ӧ�ó�����
class CSampleMainApp : public CMzApp
{
public:
    // Ӧ�ó����������
    CSampleMainWnd m_MainWnd;

    // Ӧ�ó���ĳ�ʼ��
    virtual BOOL Init()
    {
        // ��ʼ�� COM ���
        CoInitializeEx(0, COINIT_MULTITHREADED);

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