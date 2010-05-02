
#pragma once

// 包含MZFC库的头文件
#include <mzfc_inc.h>
#include <festio.h>

typedef enum tagFestivalListType{
    ListLunarBirthday   =   0x00,
    ListSolarBirthday   =   0x01,
    ListLunar           =   0x10,
    ListSolar           =   0x11,
    ListLunarHoliday    =   0x12,
    ListSolarHoliday    =   0x13,
    ListWeeklyReminder  =   0x20,
    ListMonthlyReminder =   0x21,
    ListCountdown       =   0x22,
    ListCountup         =   0x23,
}FestivalListType;

class FestivalItemCollection : public IItemCollection
{
private:
    FestivalListType ctype;
    FestivalType ftype;
public:
    FestivalItemCollection();
    ~FestivalItemCollection();
    void SetFestivalCollectionType(FestivalListType type);
    bool SaveList() {
        return fio->save();
    }
private:
    bool updateListVector();
public:
    virtual ImagingHelper* GetImageFirst(int nIndex)
    {
        ImagingHelper* pimg = m_container.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_NEW_BUILT, true);
        return pimg;
    }

    virtual ImagingHelper* GetImageSecond(int nIndex)
    {
        return NULL;
    }

    virtual ImagingHelper* GetImageFirst_Pressed(int nIndex)
    {
        ImagingHelper* pimg = m_container.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_NEW_BUILT_PRESSED, true);
        return pimg;
    }

    virtual ImagingHelper* GetImageSecond_Pressed(int nIndex)
    {
        return NULL;
    }

    virtual CMzString GetTitle(int nIndex);

    virtual CMzString GetDescription(int nIndex);

    virtual CMzString GetPostscript1(int nIndex);
    virtual CMzString GetPostscript2(int nIndex);

    virtual int GetItemCount();

    virtual void SetMultiSelect(int nIndex,bool isSelect)
    {
        fio->query_at(nIndex)->bselected = isSelect;
        return;
    }

    virtual bool IsEnableDragDelete(int nIndex)
    {
        return true;
    }

    virtual bool IsMultiSelected(int nIndex)
    {
        return fio->query_at(nIndex)->bselected;
    }

    virtual int GetItemHeight(int nIndex)
    {
        return 60;
    }

    virtual void SetItemHeight(int nIndex,int height)
    {
        return;
    }

    virtual void RemoveItem(int nIndex)
    {
        fio->query_at(nIndex)->bdeleted = true;
        updateListVector();
        return;
    }

    virtual DrawItemFun GetDrawItemFunPointer(int nIndex)
    {      
        return NULL;
    }

    virtual void UpdateItemAttribute(int nIndex,__inout ItemAttribute& itemAttr,void* pData)
    {
        //UiCustomList* pList = (UiCustomList*)pData;
        ////改变字体的颜色
        //if (nIndex == 5)
        //{
        //    itemAttr.m_tpDescription.m_fontColor = RGB(255, 50, 20);
        //}
    }

    bool AppendItem(lpFestival pf){
        bool bRet = fio->append(pf);
        if(bRet){
            updateListVector();
        }
        return bRet;
    }

private:
    ImageContainer m_container;
public:
    FestivalIO *fio;

};
