// 包含MZFC库的头文件
#include <mzfc_inc.h>
#include <festio.h>

class UiCheckLabel : public UiButton{
public:
    UiCheckLabel() { 
        SetButtonType(MZC_BUTTON_NONE);
    }
    void SetCheckStatus(bool bchecked){
        if(ischecked != bchecked){
            ischecked = bchecked;
            Invalidate();
        }
    }
    bool GetCheckStatus(){
        return ischecked;
    }
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        RECT prcCheckBox = *prcWin;
        prcCheckBox.right = prcWin->left + 40;
        MzDrawControl(hdcDst,&prcCheckBox,MZCV2_CHECKBOX_SELECT,ischecked);

        RECT prcLabelText = *prcWin;
        prcLabelText.left = prcWin->left + 45;
        ::SetTextColor(hdcDst,RGB(0,0,0));
        MzDrawText(hdcDst,GetText().C_Str(),&prcLabelText,DT_VCENTER|DT_LEFT|DT_WORD_ELLIPSIS);
    }
private:
    bool ischecked;
};

class Ui_FestivalEdit : public CMzWndEx{
    MZ_DECLARE_DYNAMIC(Ui_FestivalEdit);
private:
    UiStatic m_lblType;
    UiButton m_btnType;
    UiSingleLineEdit m_edtName;
    UiSingleLineEdit m_edtDetail;
    UiStatic m_lblDate;
    UiButton m_btnDate;
    UiCheckLabel m_useYear;
    UiToolBarPro m_toolBar;
protected:
    virtual BOOL OnInitDialog();
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
public:
    int ShowModifyDialog(lpFestival pf,HWND hwnd = 0);
    int ShowAppendDialog(lpFestival pf,HWND hwnd = 0);
private:
    void UpdateUi();
    bool StoreValue();  //检查/保存临时值到返回值
private:
    lpFestival m_pfest;
    Festival m_temp;
};