#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "histodaydb.h"

extern bool ImportData(TCHAR* filename,HWND m_hWnd);
extern HistodayDB *CreateHistodayDatabase(void);
