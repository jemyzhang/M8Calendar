// ����MZFC���ͷ�ļ�
#include <mzfc_inc.h>
#include <festio.h>

class UiCheckLabel;

class Ui_FestivalEdit : public CMzWndEx{
    MZ_DECLARE_DYNAMIC(Ui_FestivalEdit);
public:
	Ui_FestivalEdit();
	~Ui_FestivalEdit();
private:
    UiStatic m_lblType;
    UiButton m_btnType;
    UiSingleLineEdit m_edtName;
    UiSingleLineEdit m_edtDetail;
    UiStatic m_lblDate;
    UiButton m_btnDate;
    UiCheckLabel *m_puseYear;
    UiToolBarPro m_toolBar;
	UiStatic *m_plblDetail;
protected:
    virtual BOOL OnInitDialog();
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
public:
    int ShowModifyDialog(lpFestival pf,HWND hwnd = 0);
    int ShowAppendDialog(lpFestival pf,HWND hwnd = 0);
private:
    void UpdateUi();
    bool StoreValue();  //���/������ʱֵ������ֵ
	void UpdateDetail();	//ũ��/����������Ϣ
private:
    lpFestival m_pfest;
    Festival m_temp;
};