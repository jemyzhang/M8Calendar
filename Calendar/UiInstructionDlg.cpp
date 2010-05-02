#include "UiInstructionDlg.h"
#include <cMzCommon.h>
using namespace cMzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_InstructionWnd)

#define MZ_IDC_BTN_OK   102

Ui_InstructionWnd::Ui_InstructionWnd(void)
{
	bUpdateBgWin = TRUE;
}

Ui_InstructionWnd::~Ui_InstructionWnd(void)
{
}

BOOL Ui_InstructionWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
	m_Instruction.SetPos(5,0,GetWidth()-10,GetHeight() - MZM_HEIGHT_BUTTONEX - 10);
	AddUiWin(&m_Instruction);

	int y = GetHeight() - MZM_HEIGHT_BUTTONEX-5;
	m_BtnOK.SetPos(5,y,GetWidth()-10,MZM_HEIGHT_BUTTONEX);
	m_BtnOK.SetText(L"Ã÷°×ÁË!");
	m_BtnOK.SetID(MZ_IDC_BTN_OK);
	AddUiWin(&m_BtnOK);

    return TRUE;
}

void Ui_InstructionWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BTN_OK:
			EndModal(ID_OK);
			break;
	}
	return;
}
