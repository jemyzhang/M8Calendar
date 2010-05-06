#pragma once

// Application class derived from CMzApp
//extern wchar_t ini_reminder[256];
//extern CashReminder calendar_reminder;
// include the MZFC library header file
#include <mzfc_inc.h>

class M8CalendarApp : public CMzApp {
public:
    M8CalendarApp(){
        m_pShowWnd = 0;
    }
    // The main window of the app.
    //Ui_CalendarWnd m_MainWnd;
    CMzWndEx *m_pShowWnd;

    //check pwd
    bool checkpwd();
    // Initialization of the application
    virtual BOOL Init();
    int Done();
};
