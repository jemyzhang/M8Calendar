#include "m8calendar.h"
#include "ui_config.h"
#include "ui_today.h"
#include "ui_calendar.h"
#include "ui_today_da.h"

#include <cMzCommon.h>
using namespace cMzCommon;
// The global variable of the application.
M8CalendarApp theApp;
//CashReminder calendar_reminder;

extern CalendarConfig AppConfig;

#include <festio.h>
FestivalIO *pfestival = NULL;

BOOL M8CalendarApp::Init() {
    // Init the COM relative library.
    CoInitializeEx(0, COINIT_MULTITHREADED);

	wchar_t currpath[MAX_PATH];
	if(File::GetCurrentPath(currpath)){
		wcscat(currpath,L"sys_festivals.txt");
	}else{
		wsprintf(currpath,L"sys_festivals.txt");
	}
	pfestival = new FestivalIO(currpath);
	if(!pfestival->load()){
		delete pfestival;
		pfestival = 0;
	}

	// 判断是否是提醒调用
	LPWSTR str = GetCommandLine();
	if(lstrlen(str)){
        if(wcscmp(str,L"-da") == 0){
			//检查是否已经打开
			m_pShowWnd = new Ui_TodayDAWnd;

			HANDLE m_hCHDle = CreateMutex(NULL,true,L"M8CalendarDlg");
			if(GetLastError() == ERROR_ALREADY_EXISTS)
			{
				HWND pWnd=FindWindow(m_pShowWnd->GetMzClassName(),NULL);
				if(pWnd)
				{
					PostMessage(pWnd,MZFC_WM_MESSAGE_QUIT,NULL,NULL);
				}
				PostQuitMessage(0);
				return true; 
			}
            int dlgX,dlgY;
	        RECT rcWork = MzGetWorkArea();
            //检测是否横屏
            DEVMODE settings;
            memset(&settings, 0, sizeof(DEVMODE));
            settings.dmSize = sizeof(DEVMODE);

            settings.dmFields = DM_DISPLAYORIENTATION;
            ChangeDisplaySettingsEx(NULL, &settings, NULL, CDS_TEST, NULL);
            if(settings.dmDisplayOrientation == 0 || settings.dmDisplayOrientation == 2){
                //横屏
                dlgX = rcWork.left + 140;
                dlgY = 0;
            }else{
                dlgX = rcWork.left + 20;
                dlgY = rcWork.top + 50;
            }
            //启动

            m_pShowWnd->Create(dlgX, dlgY, 440, 460, 0, 0, 0,WS_EX_TOPMOST);
            m_pShowWnd->SetAnimateType_Show(MZ_ANIMTYPE_NONE);
            m_pShowWnd->SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
            m_pShowWnd->Show();
		    return true;
        }
	}

    //正常启动程序
	//检测程序是否已经运行
    if(AppConfig.IniStartupPage.Get() != 0){
        m_pShowWnd = new Ui_TodayWnd;
    }else{
        m_pShowWnd = new Ui_CalendarWnd;
    }
	HANDLE m_hCHDle = CreateMutex(NULL,true,L"M8Calendar");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND pWnd=FindWindow(m_pShowWnd->GetMzClassName(),NULL);
		//HWND pWnd=FindWindow(NULL,L"M8Cash");
		if(pWnd)
		{
			SetForegroundWindow(pWnd);
			PostMessage(pWnd,WM_NULL,NULL,NULL);
		}
		PostQuitMessage(0);
		return true; 
	}
	// Create the main window
	RECT rcWork = MzGetWorkArea();

    m_pShowWnd->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);
    m_pShowWnd->SetAnimateType_Show(MZ_ANIMTYPE_ZOOM_IN);
    m_pShowWnd->SetAnimateType_Hide(MZ_ANIMTYPE_NONE);
    m_pShowWnd->Show();

    // return TRUE means init success.
    return TRUE;
}