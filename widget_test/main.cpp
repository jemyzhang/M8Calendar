/************************************************************************/
/*
* Copyright (C) Meizu Technology Corporation Zhuhai China
* All rights reserved.
* 中国珠海, 魅族科技有限公司, 版权所有.
*
* This file is a part of the Meizu Foundation Classes library.
* Author:    Michael
* Create on: 2010-01-29
*/
/************************************************************************/

//请按照以步骤运行此实例代码：
//首先, 打开VS2005/2008创建一个Win 32智能设备项目
//在项目向导中选择M8SDK, 并勾选空项目
//然后,在项目中新建一个cpp文件,将此处代码拷贝到cpp文件中
//最后,按照M8SDK的帮助文档,配置项目属性
//现在,可以运行此程序了

//包含MZFC库的头文件
#include <mzfc_inc.h>

#include <ShellWidget/ShellWidget.h>

#pragma comment(lib,"mzfc.lib")
//此代码演示了：
//  创建和初始化应用程序
//  创建和初始化窗体
//  测试Widget桌面插件

typedef UiWidget* (*PFNCreateWidgetFromLibrary)(void*);

UiWidget* GetWidget( TCHAR* pszFilePath ,HMODULE &h)
{
    UiWidget* pWidget = NULL;

    // 载入DLL文件
    h = LoadLibrary(pszFilePath);
    if(h)
    {
        PFNCreateWidgetFromLibrary proc = (PFNCreateWidgetFromLibrary)GetProcAddress(h, L"CreateWidgetInstance");
        if(proc)
        {
            pWidget = proc(0);
        }
    }

    return pWidget;
}

#define MZ_IDC_BUTTON_REFRESH 102

// 从 CMzWndEx 派生的主窗口类
class CSample1MainWnd: public CMzWndEx
{
    MZ_DECLARE_DYNAMIC(CSample1MainWnd);
public:
    CSample1MainWnd(){
        m_pWidget = NULL;
        m_pRefresh = NULL;
    }
    ~CSample1MainWnd(){
        delete m_pRefresh;
        FreeLibrary(m_hmodule);
    }
public:
    // 窗口中的插件
    UiWidget* m_pWidget;
    UiButton* m_pRefresh;
    HMODULE m_hmodule;
	static bool GetCurrentPath(LPTSTR szPath, HMODULE handle = NULL) 
	{ 
			if(handle == NULL) handle = GetModuleHandle(NULL);
			DWORD dwRet = GetModuleFileName(handle, szPath, MAX_PATH);
			if (dwRet == 0)
			{
					return false;
			}
			else
			{
					TCHAR* p = szPath;
					while(*p)++p; //let p point to '\0' 
					while('\\' != *p)--p; //let p point to '\\' 
					++p;
					*p = '\0'; //get the path
					return true; 
			}
	}

private:
    void AddWidget(){
        if(m_pWidget){
            RemoveUiWin(m_pWidget);
            delete m_pWidget;
            m_pWidget = 0;
            FreeLibrary(m_hmodule);
        }
        wchar_t dllpath[MAX_PATH];
#if 1
        GetCurrentPath(dllpath);
        wcscat(dllpath,L"WidgetLunarCalendar.dll");
#else
        wsprintf(dllpath,L"%s",L"\\Program Files\\M8Calendar\\WidgetLunarCalendar.dll");
#endif
        // 创建Widget对象
        m_pWidget = GetWidget(dllpath,m_hmodule);
        if(m_pWidget == NULL) return;
        int nW = 0;
        int nH = 0;
        m_pWidget->OnCalcItemSize(nW, nH);
        m_pWidget->SetPos(10, 10, DESKTOPITEM_WIDTH * nW, DESKTOPITEM_HEIGHT * nH);

        // 把Widget添加到窗口中
        AddUiWin(m_pWidget);
        // 启动Widget
        m_pWidget->StartWidget();
        this->Invalidate();
        this->UpdateWindow();
    }
protected:
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam) {
        UINT_PTR id = LOWORD(wParam);
        switch(id){
            case MZ_IDC_BUTTON_REFRESH:
                if(m_pWidget){
                    AddWidget();
                }else{
                    ::PostQuitMessage(0);
                }
                break;
        }
    }
    // 窗口的初始化
    virtual BOOL OnInitDialog()
    {
        // 必须先调用基类的初始化
        if (!CMzWndEx::OnInitDialog())
        {
            return FALSE;
        }
        this->SetBgColor(RGB(64,64,64));

        AddWidget();

        m_pRefresh = new UiButton;
        m_pRefresh->SetPos(GetWidth() / 4, GetHeight() - 100, GetWidth() / 2 , 80);
        m_pRefresh->SetID(MZ_IDC_BUTTON_REFRESH);
        if(m_pWidget){
            m_pRefresh->SetText(L"重新载入配置");
        }else{
            m_pRefresh->SetText(L"退出");
        }
        AddUiWin(m_pRefresh);
        return TRUE;
    }

};

MZ_IMPLEMENT_DYNAMIC(CSample1MainWnd)

// 从 CMzApp 派生的应用程序类
class CSample1App: public CMzApp
{
public:
    // 应用程序的主窗口
    CSample1MainWnd m_MainWnd;

    // 应用程序的初始化
    virtual BOOL Init()
    {
        // 初始化 COM 组件
        CoInitializeEx(0, COINIT_MULTITHREADED);

        // 创建主窗口
        RECT rcWork = MzGetWorkArea();
        m_MainWnd.Create(rcWork.left,rcWork.top,RECT_WIDTH(rcWork),RECT_HEIGHT(rcWork), 0, 0, 0);
        m_MainWnd.Show();

        // 成功则返回TRUE
        return TRUE;
    }

    virtual int Done()
    {
        if (m_MainWnd.m_pWidget)
        {
            m_MainWnd.m_pWidget->EndWidget();

            delete m_MainWnd.m_pWidget;
            m_MainWnd.m_pWidget = NULL;
        }

        return CMzApp::Done();
    }

};

// 全局的应用程序对象
CSample1App theApp;

