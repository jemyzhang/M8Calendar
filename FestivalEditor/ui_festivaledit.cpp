#include "ui_festivaledit.h"
#include <mzfc\MzSetDaily.h>

#include <cMzCommon.h>
using namespace cMzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_FestivalEdit);

#define MZ_IDC_BUTTON_TYPE	101
#define MZ_IDC_BUTTON_DATE	102
#define MZ_IDC_USE_YEAR	103
#define MZ_IDC_EDIT_NAME	104
#define MZ_IDC_EDIT_DETAIL  105

#define MZ_IDC_TOOLBAR	108

static const LPCTSTR names[] = {
    L"农历生日",L"公历生日",L"农历节日",L"公历节日",L"农历假日",L"公历假日",
    L"每周",L"每月",L"倒计时",L"计时"
};

#define LINE_HEIGHT MZM_HEIGHT_BUTTON_GRAY

BOOL Ui_FestivalEdit::OnInitDialog(){
        if (!CMzWndEx::OnInitDialog())
        {
	        return FALSE;
        }
        int y = 0;
        m_lblType.SetPos(0,y,100,LINE_HEIGHT);
        m_lblType.SetText(L"节日类型");
        AddUiWin(&m_lblType);

        m_btnType.SetPos(110,y,GetWidth() - 120,LINE_HEIGHT);
        m_btnType.SetButtonType(MZC_BUTTON_DOWNLOAD);
        m_btnType.SetTextColor(RGB(0,0,0));
        m_btnType.SetID(MZ_IDC_BUTTON_TYPE);
        AddUiWin(&m_btnType);

        y += LINE_HEIGHT + 15;
        m_edtName.SetPos(0,y,GetWidth(),LINE_HEIGHT);
        m_edtName.SetLeftInvalid(80);
        m_edtName.SetTip2(L"名称");
        m_edtName.SetID(MZ_IDC_EDIT_NAME);
        m_edtName.SetMaxChars(6);
        AddUiWin(&m_edtName);

        y += LINE_HEIGHT + 15;
        m_edtDetail.SetPos(0,y,GetWidth(),LINE_HEIGHT);
        m_edtDetail.SetLeftInvalid(80);
        m_edtDetail.SetTip2(L"详情");
        m_edtDetail.SetID(MZ_IDC_EDIT_DETAIL);
        m_edtDetail.SetMaxChars(40);
        m_edtDetail.SetTextSize(20);
        AddUiWin(&m_edtDetail);

        y += LINE_HEIGHT + 15;
        m_lblDate.SetPos(0,y,100,LINE_HEIGHT);
        m_lblDate.SetText(L"日期");
        AddUiWin(&m_lblDate);

        m_btnDate.SetPos(100,y,GetWidth() - 250,LINE_HEIGHT);
        m_btnDate.SetID(MZ_IDC_BUTTON_DATE);
        m_btnDate.SetTextColor(RGB(0,0,0));
        m_btnDate.SetButtonType(MZC_BUTTON_DOWNLOAD);
        AddUiWin(&m_btnDate);

        m_useYear.SetPos(GetWidth() - 140,y,140,LINE_HEIGHT);
        m_useYear.SetID(MZ_IDC_USE_YEAR);
        m_useYear.SetText(L"使用年份");
        AddUiWin(&m_useYear);

        m_toolBar.SetID(MZ_IDC_TOOLBAR);
        m_toolBar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
        m_toolBar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"取消");
        m_toolBar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON, true, true, L"完成");
        AddUiWin(&m_toolBar);
        UpdateUi();
		return TRUE;
}

int Ui_FestivalEdit::ShowModifyDialog(lpFestival pf,HWND hwnd){
    m_btnType.SetEnable(false);
    m_pfest = pf;
    m_temp = *m_pfest;
    //防止字符串被解构
    if(m_temp.type != FestivalSolarRemind){
        m_temp.info1.name = 0;
        C::newstrcpy(&m_temp.info1.name,m_pfest->info1.name);
    }
    m_temp.detail = 0;
    C::newstrcpy(&m_temp.detail,m_pfest->detail);

    RECT rcWorkDate = MzGetWorkArea();
    Create(rcWorkDate.left, rcWorkDate.top, RECT_WIDTH(rcWorkDate), RECT_HEIGHT(rcWorkDate), hwnd, 0, WS_POPUP);
    SetAnimateType_Show(MZ_ANIMTYPE_FADE);
    SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
    return DoModal();
}

int Ui_FestivalEdit::ShowAppendDialog(lpFestival pf,HWND hwnd){
    SetWindowText(L"新增");
    m_btnType.SetEnable(true);
    m_pfest = pf;
    m_temp = *m_pfest;

    RECT rcWorkDate = MzGetWorkArea();
    Create(rcWorkDate.left, rcWorkDate.top, RECT_WIDTH(rcWorkDate), RECT_HEIGHT(rcWorkDate), hwnd, 0, WS_POPUP);
    SetAnimateType_Show(MZ_ANIMTYPE_FADE);
    SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
    return DoModal();
}

void Ui_FestivalEdit::UpdateUi(){
    //类型
    int nidx = m_temp.type - 1;
    if(m_temp.type == FestivalSolarRemind){
        nidx += m_temp.info1.type - 1;
    }
    m_btnType.SetText(names[nidx]);
    //名称
    if(m_temp.type == FestivalSolarRemind){
        m_edtName.SetEnable(false);
        m_edtName.SetText(L"");
    }else{
        m_edtName.SetEnable(true);
        m_edtName.SetText(m_temp.info1.name);
    }
    //详情
    m_edtDetail.SetText(m_temp.detail);
    //日期

    wchar_t s[20];
    if(m_temp.type == FestivalSolarRemind){
        m_useYear.SetVisible(false);
        if(m_temp.info1.type == ReminderCountdown ||
            m_temp.info1.type == ReminderCountup){
                wsprintf(s, L"%04d年%02d月%02d日", m_temp.info0.year, m_temp.month, m_temp.day);
        }else if(m_temp.info1.type == ReminderWeekly){
            wchar_t *weekname[] = {L"一",L"二",L"三",L"四",L"五",L"六",L"日"};
            wsprintf(s,L"每周%s",weekname[m_temp.info0.idx-1]);
        }else{
            wsprintf(s,L"每月%d号",m_temp.info0.idx);
        }
    }else{
        m_useYear.SetVisible(true);
        if(m_temp.info0.year == 0){
            m_useYear.SetCheckStatus(false);
            wsprintf(s, L"%02d月%02d日", m_temp.month, m_temp.day);
        }else{
            m_useYear.SetCheckStatus(true);
            wsprintf(s, L"%04d年%02d月%02d日", m_temp.info0.year, m_temp.month, m_temp.day);
        }
    }

    if(m_temp.type == FestivalLunarBirth ||
        m_temp.type == FestivalSolarBirth){
            m_useYear.SetCheckStatus(true);
            m_useYear.SetEnable(false);
    }else{
        m_useYear.SetEnable(true);
    }

    if(m_temp.info0.year == 0 && m_temp.month == 0 && m_temp.day == 0){
        m_btnDate.SetText(0);
    }else{
        m_btnDate.SetText(s);
    }
    Invalidate();
}

bool Ui_FestivalEdit::StoreValue(){
    if(m_btnDate.GetText().Length()  == 0){
        ::MzMessageAutoBoxV2(m_hWnd,L"请选择时间",MZV2_MB_NONE,1000,true);
        return false;
    }
    if(m_temp.type != FestivalSolarRemind &&
        m_edtName.GetText().Length() == 0){
        ::MzMessageAutoBoxV2(m_hWnd,L"请输入名称",MZV2_MB_NONE,1000,true);
        return false;
    }
    if(m_edtDetail.GetText().Length() == 0){
        ::MzMessageAutoBoxV2(m_hWnd,L"请输入详情",MZV2_MB_NONE,1000,true);
        return false;
    }
    m_pfest->info0.year = m_temp.info0.year;

    //检查年份是否有效
    if(m_temp.type != FestivalSolarRemind){
        if(!m_useYear.GetCheckStatus()){
            m_pfest->info0.year = 0;
        }
    }

    m_pfest->day = m_temp.day;
    m_pfest->month = m_temp.month;
    m_pfest->type = m_temp.type;
    m_pfest->bselected = m_temp.bselected;
    m_pfest->bdeleted = m_temp.bdeleted;

    C::newstrcpy(&m_pfest->detail,m_edtDetail.GetText().C_Str());
    if(m_temp.type != FestivalSolarRemind){
        C::newstrcpy(&m_pfest->info1.name,m_edtName.GetText().C_Str());
    }else{
        m_pfest->info1.type = m_temp.info1.type;
    }
    return true;
}

void Ui_FestivalEdit::OnMzCommand(WPARAM wParam, LPARAM lParam){
        UINT_PTR id = LOWORD(wParam);
        switch(id)
        {
        case MZ_IDC_USE_YEAR:
            m_useYear.SetCheckStatus(!m_useYear.GetCheckStatus());
            m_useYear.Invalidate();
            break;
        case MZ_IDC_TOOLBAR:
            {
                int index = lParam;
                if(index == TOOLBARPRO_LEFT_TEXTBUTTON){
                    EndModal(ID_CANCEL);
                }else if(index == TOOLBARPRO_RIGHT_TEXTBUTTON){
                    if(StoreValue()){
                        EndModal(ID_OK);
                    }
                }
            }
            break;
        case MZ_IDC_BUTTON_TYPE:
            {
                //创建新弹出菜单对象
                MzPopupMenu m_PopupMenu;
                //设置弹出菜单的标题
                m_PopupMenu.SetMenuTitle(L"节假日类型");

                for(int i = 0; i < sizeof(names)/sizeof(names[0]); i++){
                    m_PopupMenu.AppendMenuItem(MZV2_MID_MIN + i, names[i]);
                }
                //获得弹出菜单的返回值
                int result = m_PopupMenu.MzTrackPopupMenu(m_hWnd, TRUE);
                if(result >= MZV2_MID_MIN){
                    if(result < MZV2_MID_MIN + 6){
                        m_temp.type = (FestivalType)(result - MZV2_MID_MIN + 1);
                    }else{
                        m_temp.type = FestivalSolarRemind;
                        m_temp.info1.type = (ReminderType)(result - MZV2_MID_MIN + 1 - 6);
                    }
                }
                int nidx = m_temp.type - 1;
                if(m_temp.type == FestivalSolarRemind){
                    nidx += m_temp.info1.type - 1;
                }
                m_btnType.SetText(names[nidx]);
                m_btnType.Invalidate();
                m_temp.info0.year = m_temp.month =  m_temp.day = 0;
                m_btnDate.SetText(0);
                m_btnDate.Invalidate();
                if(m_temp.type == FestivalSolarRemind){
                    m_useYear.SetVisible(false);
                }else{
                    m_useYear.SetVisible(true);
                    if(m_temp.type == FestivalLunarBirth ||
                        m_temp.type == FestivalSolarBirth){
                            m_useYear.SetCheckStatus(true);
                            m_useYear.SetEnable(false);
                    }else{
                        m_useYear.SetEnable(true);
                    }
                }
                m_useYear.Invalidate();
            }
            break;
        case MZ_IDC_BUTTON_DATE:
            {
                // 日期设置对话框的初始化数据
                MzSetDaily setDateDlg;
                int items = 3;
                if(m_temp.type != FestivalSolarRemind){
                    if(!m_useYear.GetCheckStatus()){
                        items = 2;
                    }
                }else{
                    if(m_temp.info1.type != ReminderCountdown &&
                        m_temp.info1.type != ReminderCountup){
                        items = 1;
                    }
                }
                if(items > 1){
                    setDateDlg.m_InitData.iItemCounter = 3;
                    setDateDlg.m_InitData.iItem1Max = 2100 - 1923; // 年份是从1923年开始计算的
                    setDateDlg.m_InitData.iItem2Max = 12;

                    // 设置对话框初始显示日期的项index，index值从0开始
                    if(m_temp.info0.year == 0 && m_temp.month == 0 && m_temp.day == 0){
                        //新建/切换类型时
                        SYSTEMTIME m_sTime;
                        GetLocalTime(&m_sTime);
                        setDateDlg.m_InitData.iItem3Max = setDateDlg.MaxDaysInMonth(m_sTime.wMonth, m_sTime.wYear);
                        setDateDlg.m_InitData.iItem1Data = m_sTime.wYear - 1923;
                        setDateDlg.m_InitData.iItem2Data = m_sTime.wMonth - 1;
                        setDateDlg.m_InitData.iItem3Data = m_sTime.wDay - 1;
                    }else{
                        setDateDlg.m_InitData.iItem3Max = setDateDlg.MaxDaysInMonth(m_temp.month, m_temp.info0.year);
                        setDateDlg.m_InitData.iItem1Data = m_temp.info0.year - 1923;
                        setDateDlg.m_InitData.iItem2Data = m_temp.month - 1;
                        setDateDlg.m_InitData.iItem3Data = m_temp.day - 1;
                    }

                    setDateDlg.m_InitData.sItem1 = L"年";
                    setDateDlg.m_InitData.sItem2 = L"月";
                    setDateDlg.m_InitData.sItem3 = L"日";
                    setDateDlg.EnableDisplayYearData(true);
                    if(items == 3){
                        setDateDlg.m_InitData.iItem1Width = setDateDlg.m_InitData.iItem2Width = setDateDlg.m_InitData.iItem3Width = 140;
                    }else{
                        setDateDlg.m_InitData.iItem1Width = 0;
                        setDateDlg.m_InitData.iItem2Width = setDateDlg.m_InitData.iItem3Width = 210;
                    }
                }else{
                    setDateDlg.m_InitData.iItemCounter = 1;
                    if(m_temp.info1.type == ReminderWeekly){
                        setDateDlg.m_InitData.iItem1Max = 7+2;
                        setDateDlg.m_InitData.iItem1Min = 1;
                        setDateDlg.m_InitData.sItem1 = L"周";
                    }else{
                        setDateDlg.m_InitData.iItem1Max = 31+2;
                        setDateDlg.m_InitData.iItem1Min = 1;
                        setDateDlg.m_InitData.sItem1 = L"号";
                    }

                    // 设置对话框初始显示日期的项index，index值从0开始
                    setDateDlg.m_InitData.iItem1Data = m_temp.info0.idx - 1;

                    setDateDlg.m_InitData.iItem1Width = 420;
                    setDateDlg.EnableDisplayYearData(false);
                }
                setDateDlg.SetEnableChangeItemData(true);

                // 创建日期设置对话框的窗口
                RECT rcWorkDate = MzGetWorkArea();
                setDateDlg.Create(rcWorkDate.left, rcWorkDate.top, RECT_WIDTH(rcWorkDate), RECT_HEIGHT(rcWorkDate), m_hWnd, 0, WS_POPUP);
                // 设置窗口淡出的动画效果
                setDateDlg.SetAnimateType_Show(MZ_ANIMTYPE_FADE);
                setDateDlg.SetAnimateType_Hide(MZ_ANIMTYPE_FADE);

                // 如果对话框点击“确定”，显示对话框设置的日期
                if (ID_OK == setDateDlg.DoModal())
                {
                    CMzString str(128);

                    if(items > 1){
                        m_temp.info0.year = setDateDlg.m_InitData.iItem1Data;
                        m_temp.month = setDateDlg.m_InitData.iItem2Data;
                        m_temp.day = setDateDlg.m_InitData.iItem3Data;
                        if(items == 3){
                            wsprintf(str.C_Str(), L"%04d年%02d月%02d日", m_temp.info0.year, m_temp.month, m_temp.day);
                        }else{
                            wsprintf(str.C_Str(), L"%02d月%02d日", m_temp.month, m_temp.day);
                        }
                    }else{
                        m_temp.info0.idx = setDateDlg.m_InitData.iItem1Data + 1;
                        if(m_temp.info1.type == ReminderWeekly){
                            wchar_t *weekname[] = {L"一",L"二",L"三",L"四",L"五",L"六",L"日"};
                            wsprintf(str.C_Str(),L"每周%s",weekname[m_temp.info0.idx - 1]);
                        }else{
                            wsprintf(str.C_Str(),L"每月%d号",m_temp.info0.idx);
                        }
                    }
                    m_btnDate.SetText(str.C_Str());
                    m_btnDate.Invalidate();
                }
            }
            break;
        }
}