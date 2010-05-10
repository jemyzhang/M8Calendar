#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
using std::wstring;

#include <vector>
using std::vector;

typedef struct tagDrawFestInfo{
    wstring info;
    UCHAR t;
}DrawFestInfo, *LPDrawFestInfo;

class UiFestivalInfo : public UiEdit {
public:
    UiFestivalInfo(){
        SetEditBgType(UI_EDIT_BGTYPE_FILL_WHITE_AND_TOPSHADOW);
        SetReadOnly(true);
        EnableZoomIn(false);
    }
    ~UiFestivalInfo(){
        clearInfo();
    }
    virtual int OnLButtonDblClk(UINT fwKeys, int xPos, int yPos);
public:
    void SetDate(DWORD y, DWORD m, DWORD d);
private:
    void appendInfo(wstring info, UCHAR t);
    void appendInfo(LPCTSTR info, UCHAR t);
    void clearInfo();
    void apply();
private:
    vector<LPDrawFestInfo> infos;
};

extern const COLORREF festival_colors[8];