#pragma once
/*
 * @filename UiSingleOption.h
 * @note 单选列表
 * @author JEMYZHANG
 * @date 2009.10.16
 * @ver. 1.0
 * @changelog
 *  ver.1.0 初始化程序
*/
#include "UiImage.h"

class Ui_InstructionWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_InstructionWnd);
public:
	Ui_InstructionWnd(void);
	~Ui_InstructionWnd(void);
public:
	UiImage m_splitter_top;
    UiImage m_Instruction;
	UiButton m_BtnOK;
    UiStatic m_Title;
public:
    void SetInstructionImage(ImagingHelper* img){
		m_Instruction.setupImage(img);
    }
    void PaintWin(HDC hdc, RECT* prcUpdate = NULL){
        if(bUpdateBgWin == TRUE){
            bUpdateBgWin = FALSE;
            bgWin = ::ScreenSnapshot();
        }
		RECT rectWin = {0,0,0,0};
		rectWin.right = this->GetWidth();
		rectWin.bottom = this->GetHeight();
		::SetBkMode(hdc,TRANSPARENT);
		HBRUSH myBrush = CreateSolidBrush(RGB(16,16,16));
		FillRect(hdc,&rectWin,myBrush);//底色
        ::BitmapTransBlt(hdc,&rectWin,bgWin,&rectCopy,RGB(128,128,128));
        CMzWndEx::PaintWin(hdc,prcUpdate);
    }
    virtual BOOL Create(int xPos, int yPos, int width, int height, HWND hwndParent=NULL, int uID = NULL, DWORD style=WS_CHILD, DWORD exstyle=0){
        rectCopy.left = xPos;
        rectCopy.top = yPos;
        rectCopy.right = xPos + width;
        rectCopy.bottom = yPos + height;
        return CMzWndEx::Create(xPos,yPos,width,height,hwndParent,uID,style,exstyle);
    }
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
    LPTSTR m_title;
	HBITMAP bgWin;
	BOOL bUpdateBgWin;
	RECT rectCopy;
};
