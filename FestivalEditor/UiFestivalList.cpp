#include "UiFestivalList.h"
#include <cMzCommon.h>
using namespace cMzCommon;

//增加农历、生肖、星座信息@2010-5-8
#include "resource.h"
#include <lcal.h>

FestivalItemCollection::FestivalItemCollection(){
    wchar_t currpath[MAX_PATH];
    if(File::GetCurrentPath(currpath)){
        wcscat(currpath,L"sys_festivals.txt");
    }else{
        wsprintf(currpath,L"sys_festivals.txt");
    }

    fio = new FestivalIO(currpath);
    if(!fio->load()){
        MzMessageBoxV2(0,L"错误: 节日列表文件未找到。",MZV2_MB_OK,true);
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
            L"周提醒",
            L"月提醒",
            L"倒计时",
            L"计时"
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
            case 0: //周提醒
                {
                wchar_t *weekname[] = {L"一",L"二",L"三",L"四",L"五",L"六",L"日"};
                wsprintf(s,L"每周%s",weekname[f->info0.idx-1]);
                }
                break;
            case 1: //月提醒
                wsprintf(s,L"每月%d号",f->info0.idx);
                break;
            case 2: //倒计时
            case 3: //计时
                wsprintf(s,L"%04d-%02d-%02d",f->info0.year,f->month,f->day);
                break;
            default:
                wsprintf(s,L"未知");
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
	if(ftype == FestivalLunarBirth){
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

		lpFestival pf = fio->query_at(nIndex);
		LCAL lstm(sysTime.wYear);
		lstm.setLunarDate(sysTime.wYear,pf->month,pf->day);
		LSDate s1 = lstm.getSolarDate();
		LSDate s2;
		s2.year = sysTime.wYear; s2.month = sysTime.wMonth; s2.day = sysTime.wDay;
		if(s1.month < s2.month || (s1.month == s2.month && s1.day < s2.day)){
			lstm.setLunarDate(sysTime.wYear + 1,pf->month,pf->day);
			s1 = lstm.getSolarDate();
		}
		int dd = lstm.diffDate(s1,s2);
		WCHAR dstr[16];
		wsprintf(dstr,L"%d岁(+%d)",s1.year - pf->info0.year + 1,dd);
		return dstr;
	}else if(ftype == FestivalSolarBirth){
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

		lpFestival pf = fio->query_at(nIndex);
		LSDate s1;
		s1.year = sysTime.wYear; s1.month = pf->month; s1.day = pf->day;
		LSDate s2;
		s2.year = sysTime.wYear; s2.month = sysTime.wMonth; s2.day = sysTime.wDay;
		if(s1.month < s2.month || (s1.month == s2.month && s1.day < s2.day)){
			s1.year ++;
		}
		LCAL lstm(sysTime.wYear);
		int dd = lstm.diffDate(s1,s2);
		WCHAR dstr[16];
		wsprintf(dstr,L"%d岁(+%d)",s1.year - pf->info0.year + 1,dd);
		return dstr;
	}else{
		return L"";
	}
}

int FestivalItemCollection::GetItemCount(){
    return fio->query_size();
}

ImagingHelper* FestivalItemCollection::GetImageFirst(int nIndex)
{
	ImagingHelper* pimg = NULL;
	if(ftype == FestivalLunarBirth){
		lpFestival pf = fio->query_at(nIndex);
		LCAL lstm(pf->info0.year);
		lstm.setLunarDate(pf->info0.year,pf->month,pf->day);
		UCHAR zodiac;
		lstm.Zodiac(&zodiac);
		pimg = m_container.LoadImage(MzGetInstanceHandle(),IDB_PNG_ZODIAC_00 + zodiac,true);
	}else if(ftype == FestivalSolarBirth){
		lpFestival pf = fio->query_at(nIndex);
		UCHAR sunsign = LCAL::GetSunSign(pf->month,pf->day,NULL);
		pimg = m_container.LoadImage(MzGetInstanceHandle(),IDB_PNG_SUNSIGN_00 + sunsign,true);
	}else{
		pimg = m_container.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_NEW_BUILT, true);
	}
	return pimg;
}

ImagingHelper* FestivalItemCollection::GetImageFirst_Pressed(int nIndex)
{
	ImagingHelper* pimg = m_container.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_NEW_BUILT_PRESSED, true);
	return pimg;
}

ImagingHelper* FestivalItemCollection::GetImageSecond(int nIndex)
{
	if(ftype == FestivalLunarBirth){
		return NULL;//pimgZodiacs[0];
	}else{
		return NULL;
	}
}
ImagingHelper* FestivalItemCollection::GetImageSecond_Pressed(int nIndex)
{
	if(ftype == FestivalLunarBirth){
		return NULL;//pimgZodiacs[0];
	}else{
		return NULL;
	}
}
