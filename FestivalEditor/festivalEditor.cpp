// 包含MZFC库的头文件
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

// 从 CMzWndEx 派生的主窗口类
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
    bool bChanged;  //编辑/新增/删除后true
	UINT UsbNotifyMsg;
private:
    enum tagDPStatus{
        DP_STATUS_OK    =   0,
        DP_NOT_INSTALL  =   1,
        DP_NOT_SHOW     =   2,
        DP_OLD_VERSION  =   3,
        DP_ERR_OTHER    =   4,
    };
    //根据注册表内容确定桌面插件是否安装
    //1: Not Install
    //2: Not Install to Desktop
    //3: Version too old
    //4: Other
    int CheckDeskPlugin(){
        HKEY hKEY;
        HKEY hKeyRoot = HKEY_LOCAL_MACHINE;
        //1: 检测桌面插件是否已经启用
        long ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\iDapRc\\M8Calendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
        {
            RegCloseKey(hKEY);
            return DP_NOT_INSTALL;
        }
        DWORD RegType;
        DWORD RegData;
        DWORD RegDataLen = 4;
        //2: 获取桌面插件运行请求   //若请求键不存在，则提示需要更新桌面插件
        if(::RegQueryValueEx(hKEY,
            L"FestRefreshReq",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                RegCloseKey(hKEY);
                return DP_OLD_VERSION;
        }
        if(RegType != REG_DWORD){ //数据类型错误
            RegCloseKey(hKEY);
            return DP_ERR_OTHER;
        }
        RegCloseKey(hKEY);

        ret = ::RegOpenKeyEx(hKeyRoot,
            L"SOFTWARE\\Meizu\\MiniOneShell\\Main\\WidgetLunarCalendar",0,
            KEY_READ,&hKEY);
        if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
        {
            RegCloseKey(hKEY);
            return DP_NOT_INSTALL;
        }
        if(::RegQueryValueEx(hKEY,
            L"IsHide",NULL,&RegType,(LPBYTE)&RegData,&RegDataLen) != ERROR_SUCCESS){
                RegCloseKey(hKEY);
                return DP_NOT_INSTALL;
        }
        if(RegType != REG_DWORD){ //数据类型错误
            RegCloseKey(hKEY);
            return DP_ERR_OTHER;
        }
        if(RegData == 1){    //插件隐藏状态
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
        if(ret != ERROR_SUCCESS)//如果无法打开hKEY,则中止程序的执行
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
            {2, {L"农历生日",L"公历生日"}},
            {4, {L"农历节日",L"公历节日",L"农历假日",L"公历假日"}},
            {4, {L"周提醒",L"月提醒",L"倒计时",L"计时"}},
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
            MzMessageBoxV2(m_hWnd,L"错误: 农历桌面插件未安装。",MZV2_MB_OK,true);
            ::PostQuitMessage(0);        
        }
#endif
        if(DP_OLD_VERSION == nRet){
            MzMessageBoxV2(m_hWnd,L"警告: 农历桌面插件版本太低，不支持立即刷新节日功能，请尽快升级至最新版。",MZV2_MB_OK,true);
        }
    }
protected:
    // 窗口初始化
    virtual BOOL OnInitDialog()
    {
        // 必须先调用基类的初始化
        if (!CMzWndEx::OnInitDialog())
        {
	        return FALSE;
        }
        int y = 0;
        // 初始化 UiButtonBar 控件
        m_pButtonBar0 = new UiButtonBar;
        m_pButtonBar0->SetID(MZ_IDC_BUTTONBAR_MAIN);
        m_pButtonBar0->SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONBAR);
        m_pButtonBar0->SetButton(0, true, true, L"生日");
        m_pButtonBar0->SetButton(1, true, true, L"节假日");
        m_pButtonBar0->SetButton(2, true, true, L"提醒");
        AddUiWin(m_pButtonBar0);

        y += MZM_HEIGHT_BUTTONBAR;
        m_pButtonBar1 = new UiButtonBar;
        m_pButtonBar1->SetID(MZ_IDC_BUTTONBAR_SUB);
        m_pButtonBar1->SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONBAR);
        AddUiWin(m_pButtonBar1);

        y += MZM_HEIGHT_BUTTONBAR;
        m_plist = new UiFestivalList;
        // 初始化 UiCustomList 控件
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

        // 初始化 UiToolBarPro 控件
        m_ptoolBar = new UiToolBarPro;
        m_ptoolBar->SetID(MZ_IDC_TOOLBAR);
        m_ptoolBar->SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
        m_ptoolBar->SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"选择");
        m_ptoolBar->SetMiddleButton(true, true, L"新建",NULL,NULL,NULL);
        m_ptoolBar->SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON, true, true, L"退出");
        AddUiWin(m_ptoolBar);

        pItemColl = new FestivalItemCollection;
        m_plist->SetCustomItemCollection(pItemColl);

        UpdateSubButtonBar(0,0);

        DP_InitCheck();
        delmode = false;
        bChanged = false;
		//获取USB消息
		UsbNotifyMsg = RegisterUsbNotifyMsg();

		return TRUE;
    }

    void UpdateToolbar(){
        if(!delmode){
            m_ptoolBar->SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,L"选择");
            m_ptoolBar->ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,true);
            if(bChanged){
                m_ptoolBar->SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,L"保存");
            }else{
                m_ptoolBar->SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,L"退出");
            }
            m_ptoolBar->Invalidate();
        }else{
            m_ptoolBar->SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,L"返回");
            m_ptoolBar->ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,false);
            m_ptoolBar->SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,L"删除");
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
                        ::MzMessageAutoBoxV2(m_hWnd,L"节日修改完成",MZV2_MB_NONE,1000,true);
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

    // 重载 MZFC 的命令消息处理函数
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam)
    {
        UINT_PTR id = LOWORD(wParam);
        switch(id)
        {
	    // 处理 UiButtonBar 的消息
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
        // 处理 UiToolBarPro 的消息
        case MZ_IDC_TOOLBAR:
            {
                int index = lParam;
                if (index == TOOLBARPRO_LEFT_TEXTBUTTON)    //选择/返回
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
                            ::MzMessageAutoBoxV2(m_hWnd,L"新建节日完成",MZV2_MB_NONE,1000,true);
                            m_plist->Invalidate();
                            bChanged = true;
                            UpdateToolbar();
                        }else{
                            ::MzMessageAutoBoxV2(m_hWnd,L"新建节日失败",MZV2_MB_NONE,1000,true);
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
                    }else{  //保存
                        if(bChanged){
                            bChanged = false;
                            FestivalItemCollection * pl = 
                                static_cast<FestivalItemCollection*>(m_plist->GetCustomItemCollection());
                            if(pl->SaveList()){
                                if(CheckDeskPlugin() == DP_STATUS_OK){
                                    if(::MzMessageBoxV2(m_hWnd,
                                        L"保存完毕，是否立即刷新农历插件？",
                                        MZV2_MB_YESNO,true) == 1){
                                            PostDPRequest();
                                    }
                                }else{
                                    ::MzMessageAutoBoxV2(m_hWnd,L"保存完毕",MZV2_MB_NONE,1000,true);
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

// 从 CMzApp 派生的应用程序类
class CSampleMainApp : public CMzApp
{
public:
    // 应用程序的主窗口
    FestivalEditorWnd m_MainWnd;

    // 应用程序的初始化
    virtual BOOL Init()
    {
        // 初始化 COM 组件
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
        // 创建主窗口
        RECT rcWork = MzGetWorkArea();
        m_MainWnd.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);
        m_MainWnd.SetBgColor(MzGetThemeColor(TCI_WINDOW_BG));
        m_MainWnd.SetWindowText(L"节日编辑器");
        m_MainWnd.Show();

        // 成功则返回 TRUE
        return TRUE;
    }
};

// 全局的应用程序对象
CSampleMainApp theApp;