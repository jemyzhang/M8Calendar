#include "UiFestivalList.h"
#include <cMzCommon.h>
using namespace cMzCommon;

FestivalItemCollection::FestivalItemCollection(){
    wchar_t currpath[MAX_PATH];
    if(File::GetCurrentPath(currpath)){
        wcscat(currpath,L"sys_festivals.txt");
    }else{
        wsprintf(currpath,L"sys_festivals.txt");
    }

    fio = new FestivalIO(currpath);
    if(!fio->load()){
        MzMessageBoxV2(0,L"����: �����б��ļ�δ�ҵ���",MZV2_MB_OK,true);
        ::PostQuitMessage(0);
    }
    ctype = ListLunarBirthday;
    updateListVector();
}

FestivalItemCollection::~FestivalItemCollection(){
    delete fio;
}

void FestivalItemCollection::SetFestivalCollectionType(FestivalListType type){
    if(ctype != type){
        ctype = type;
        updateListVector();
    }
}

bool FestivalItemCollection::updateListVector(){
    struct TypeMap{
        FestivalListType    t0;
        FestivalType        t1;
    }tmap[] = {
        {ListLunarBirthday,FestivalLunarBirth},
        {ListSolarBirthday,FestivalSolarBirth},
        {ListLunar,FestivalLunar},
        {ListSolar,FestivalSolar},
        {ListLunarHoliday,FestivalLunarHoliday},
        {ListSolarHoliday,FestivalSolarHoliday},
        {ListWeeklyReminder,FestivalSolarRemind},
        {ListMonthlyReminder,FestivalSolarRemind},
        {ListCountdown,FestivalSolarRemind},
        {ListCountup,FestivalSolarRemind},
    };
    ftype = FestivalNone;
    for(int i = 0; i < sizeof(tmap) / sizeof(tmap[0]); i++){
        if(ctype == tmap[i].t0){
            ftype = tmap[i].t1;
            break;
        }
    }
    if(ftype == FestivalNone) return false;
    if(ftype == FestivalSolarRemind){
        fio->query_all(ftype,(ReminderType)((ctype&0xf) + 1));
    }else{
        fio->query_all(ftype);
    }
    return true;
}

CMzString FestivalItemCollection::GetTitle(int nIndex){
    if(ftype == FestivalSolarRemind){
        wchar_t *remindername[] = {
            L"������",
            L"������",
            L"����ʱ",
            L"��ʱ"
        };
        return remindername[ctype&0xf];
    }else{
        return fio->query_at(nIndex)->info1.name;
    }
}

CMzString FestivalItemCollection::GetDescription(int nIndex){
    return fio->query_at(nIndex)->detail;
}

CMzString FestivalItemCollection::GetPostscript1(int nIndex){
    wchar_t s[20];
    lpFestival f = fio->query_at(nIndex);
    if(ftype == FestivalSolarRemind){
        switch(ctype&0x0f){
            case 0: //������
                {
                wchar_t *weekname[] = {L"һ",L"��",L"��",L"��",L"��",L"��",L"��"};
                wsprintf(s,L"ÿ��%s",weekname[f->info0.idx-1]);
                }
                break;
            case 1: //������
                wsprintf(s,L"ÿ��%d��",f->info0.idx);
                break;
            case 2: //����ʱ
            case 3: //��ʱ
                wsprintf(s,L"%04d-%02d-%02d",f->info0.year,f->month,f->day);
                break;
            default:
                wsprintf(s,L"δ֪");
                break;
        }
    }else{
        if(f->info0.year == 0){
            wsprintf(s,L"%02d-%02d",f->month,f->day);
        }else{
            wsprintf(s,L"%04d-%02d-%02d",f->info0.year,f->month,f->day);
        }
    }
    return s;
}

CMzString FestivalItemCollection::GetPostscript2(int nIndex){
//
    return L"";
}

int FestivalItemCollection::GetItemCount(){
    return fio->query_size();
}
