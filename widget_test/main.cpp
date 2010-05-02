/************************************************************************/
/*
* Copyright (C) Meizu Technology Corporation Zhuhai China
* All rights reserved.
* �й��麣, ����Ƽ����޹�˾, ��Ȩ����.
*
* This file is a part of the Meizu Foundation Classes library.
* Author:    Michael
* Create on: 2010-01-29
*/
/************************************************************************/

//�밴���Բ������д�ʵ�����룺
//����, ��VS2005/2008����һ��Win 32�����豸��Ŀ
//����Ŀ����ѡ��M8SDK, ����ѡ����Ŀ
//Ȼ��,����Ŀ���½�һ��cpp�ļ�,���˴����뿽����cpp�ļ���
//���,����M8SDK�İ����ĵ�,������Ŀ����
//����,�������д˳�����

//����MZFC���ͷ�ļ�
#include <mzfc_inc.h>

#include <ShellWidget/ShellWidget.h>

#pragma comment(lib,"mzfc.lib")
//�˴�����ʾ�ˣ�
//  �����ͳ�ʼ��Ӧ�ó���
//  �����ͳ�ʼ������
//  ����Widget������

typedef UiWidget* (*PFNCreateWidgetFromLibrary)(void*);

UiWidget* GetWidget( TCHAR* pszFilePath ,HMODULE &h)
{
    UiWidget* pWidget = NULL;

    // ����DLL�ļ�
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

// �� CMzWndEx ��������������
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
    // �����еĲ��
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
        // ����Widget����
        m_pWidget = GetWidget(dllpath,m_hmodule);
        if(m_pWidget == NULL) return;
        int nW = 0;
        int nH = 0;
        m_pWidget->OnCalcItemSize(nW, nH);
        m_pWidget->SetPos(10, 10, DESKTOPITEM_WIDTH * nW, DESKTOPITEM_HEIGHT * nH);

        // ��Widget��ӵ�������
        AddUiWin(m_pWidget);
        // ����Widget
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
    // ���ڵĳ�ʼ��
    virtual BOOL OnInitDialog()
    {
        // �����ȵ��û���ĳ�ʼ��
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
            m_pRefresh->SetText(L"������������");
        }else{
            m_pRefresh->SetText(L"�˳�");
        }
        AddUiWin(m_pRefresh);
        return TRUE;
    }

};

MZ_IMPLEMENT_DYNAMIC(CSample1MainWnd)

// �� CMzApp ������Ӧ�ó�����
class CSample1App: public CMzApp
{
public:
    // Ӧ�ó����������
    CSample1MainWnd m_MainWnd;

    // Ӧ�ó���ĳ�ʼ��
    virtual BOOL Init()
    {
        // ��ʼ�� COM ���
        CoInitializeEx(0, COINIT_MULTITHREADED);

        // ����������
        RECT rcWork = MzGetWorkArea();
        m_MainWnd.Create(rcWork.left,rcWork.top,RECT_WIDTH(rcWork),RECT_HEIGHT(rcWork), 0, 0, 0);
        m_MainWnd.Show();

        // �ɹ��򷵻�TRUE
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

// ȫ�ֵ�Ӧ�ó������
CSample1App theApp;

