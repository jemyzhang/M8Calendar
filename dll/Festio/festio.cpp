#include "festio.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include <fstream>
using namespace std;

extern bool CreateDefaultFestivalTxt(LPCTSTR path);

FestivalIO::FestivalIO(LPCTSTR szpath){
    pfestpath = 0;
    if(szpath){
        C::newstrcpy(&pfestpath,szpath);
    }
}

bool FestivalIO::load(){
    return parsefile(pfestpath);
}

bool FestivalIO::save(){
    return savefile(pfestpath);
}

bool FestivalIO::reload(){
    unload();
    return load();
}

void FestivalIO::unload(){
    for(int j = 0; j < FestivalMaxTypeSize-1; j++){
        for(int i = 0; i < festivals[j].size(); i++){
            lpFestival f = festivals[j].at(i);
            delete f;
        }
        festivals[j].clear();
    }
}

FestivalIO::~FestivalIO(){
    unload();
    if(pfestpath) delete pfestpath;
}

void FestivalIO::query_all(FestivalType t, ReminderType rt){
    fquery.clear();
    if(t == FestivalNone || t >= FestivalMaxTypeSize) return;

    for(int i = 0; i < festivals[t-1].size(); i++){
        lpFestival f = festivals[t-1].at(i);
        if(f->bdeleted) continue;
        if(t == FestivalSolarRemind && 
            rt == f->info1.type){
                fquery.push_back(f);
        }else if(t != FestivalSolarRemind){
            fquery.push_back(f);
        }
    }
}

bool FestivalIO::query(DWORD month, DWORD day, FestivalType t, ReminderType rt){
    fquery.clear();
    if(t == FestivalNone || t >= FestivalMaxTypeSize) return false;

    for(int i = 0; i < festivals[t-1].size(); i++){
        lpFestival f = festivals[t-1].at(i);
        if(f->bdeleted) continue;
        if(t == FestivalSolarRemind && 
            rt == f->info1.type){
            //month->weekday day->day
            switch(f->info1.type){
                case ReminderCountdown:
                case ReminderCountup:
                    fquery.push_back(f);
                    break;
                case ReminderWeekly: //每周
                    if(f->info0.idx == month){
                        fquery.push_back(f);
                    }
                    break;
                case ReminderMonthly: //每月
                    if(f->info0.idx == day){
                        fquery.push_back(f);
                    }
                    break;
            }
        }else if(t != FestivalSolarRemind){
            if(f->month == month && f->day == day){
                fquery.push_back(f);
            }
        }
    }
    return true;
}

int FestivalIO::query_size(){
    return fquery.size();
}

lpFestival FestivalIO::query_at(int idx){
    if(idx >= fquery.size()) return NULL;
    return fquery.at(idx);
}

bool FestivalIO::append(lpFestival pf){
    if(pf == 0) return false;
    ////检查重名
    //if(pf->type != FestivalSolarRemind){
    //    for(int i = 0; i < festivals[pf->type - 1].size(); i++){
    //        if(wcscmp(pf->info1.name,festivals[pf->type - 1].at(i)->info1.name) == 0)
    //            return false;
    //    }
    //}

    lpFestival npf = new Festival;
    npf->bdeleted = pf->bdeleted;
    npf->bselected = pf->bselected;
    npf->day = pf->day;
    npf->month = pf->month;
    npf->info0.year = pf->info0.year;
    npf->type = pf->type;
    C::newstrcpy(&npf->detail,pf->detail);
    if(pf->type == FestivalSolarRemind){
        npf->info1.type = pf->info1.type;
    }else{
        C::newstrcpy(&npf->info1.name,pf->info1.name);
    }
    festivals[pf->type - 1].push_back(npf);
    return true;
}

//==============应用类===============
//返回公历节日，空格分隔
bool FestivalIO::SolarHoliday(DWORD month, DWORD day, wchar_t* s, int nmax){
    if(s == NULL || nmax < 1) return false;
    int n = nmax;
    s[0] = '\0';    //清空

	query(month,day,FestivalSolar);
	for(int i = 0; i < query_size(); i++){
		if(nmax-- == 0) break;
		s[lstrlen(s)] = ' ';
		wcscat(s,query_at(i)->info1.name);
	}
	if(nmax > 0){
		query(month,day,FestivalSolarHoliday);
		for(int i = 0; i < query_size(); i++){
			if(nmax-- == 0) break;
			s[lstrlen(s)] = ' ';
			wcscat(s,query_at(i)->info1.name);
		}
	}

	return (n != nmax);
}

//返回农历节日
bool FestivalIO::LunarHoliday(DWORD month, DWORD day, wchar_t* s, int nmax){
    //if(LunarLeapMonth() == lunar.month && 
    //    lunar.month != lunar.imonth){   //农历闰月除外
    //        return false;
    //}

    if(s == NULL || nmax < 1) return false;
    int n = nmax;
    s[0] = '\0';    //清空

	query(month,day,FestivalLunar);
	for(int i = 0; i < query_size(); i++){
		if(nmax-- == 0) break;
		s[lstrlen(s)] = ' ';
		wcscat(s,query_at(i)->info1.name);
	}
	if(nmax > 0){
		query(month,day,FestivalLunarHoliday);
		for(int i = 0; i < query_size(); i++){
			if(nmax-- == 0) break;
			s[lstrlen(s)] = ' ';
			wcscat(s,query_at(i)->info1.name);
		}
	}

	return (n != nmax);
}

bool FestivalIO::queryBirthday(DWORD month, DWORD day, bool blunar){
    //if(blunar && LunarLeapMonth() == lunar.month && 
    //    lunar.month != lunar.imonth){   //农历闰月除外
    //        return 0;
    //}
	FestivalType t = FestivalSolarBirth;
	if(blunar){
		t = FestivalLunarBirth;
	}
	return query(month,day,t);
}

int FestivalIO::BirthdaySize(){
	return query_size();
}

lpFestival FestivalIO::Birthday(int idx){
	if(idx >= query_size()) return NULL;
	return query_at(idx);
}

bool FestivalIO::queryReminder(DWORD weekday, DWORD day, ReminderType type){
	return query(weekday,day,FestivalSolarRemind,type);
}

int FestivalIO::ReminderSize(){
    return query_size();
}

lpFestival FestivalIO::Reminder(int idx){
    if(idx < query_size()){
        return query_at(idx);
    }
    return 0;
}


//==============文件分析==============
int FestivalIO::identLineType(LPCTSTR ln){
    UINT nRet = LnInvalid;
    if(ln == NULL) return nRet;
    if(ln[0] == '\0' || ln[0] == '\x0d' || ln[0] == '\x0a') nRet = LnInvalid;
    if(ln[0] == '[' && ln[1] != ']') nRet = LnTag;
    if(ln[0] == '#') nRet = LnComment;
    if(ln[0] >= '0' && ln[0] <= '9') nRet = LnData;
    return nRet;
}

bool FestivalIO::getTag(LPCTSTR ln, LPTSTR &ptag){
    int len = lstrlen(ln);
    ptag = NULL;
    ptag = new WCHAR[len];  //第一位是[，所以不需要+1
    memset(ptag,0,sizeof(WCHAR) * len);
    for(int i = 1; i < len; i++){
        if(ln[i] == ']'){
            ptag[i-1] = '\0';
            return true;
        }
        ptag[i-1] = ln[i];
    }

    if(ptag) delete [] ptag;
    ptag = NULL;
    return false;
}

FestivalType FestivalIO::getTagType(LPTSTR sztag){
    struct tagFestTag{
        FestivalType id;
        LPCTSTR name;
    }festag[] = {
        {FestivalLunarBirth,L"LunarBirth"},
        {FestivalSolarBirth,L"SolarBirth"},
        {FestivalLunar,L"Lunar"},
//        {4,L"Week"},
        {FestivalSolar,L"Solar"},
        {FestivalLunarHoliday,L"LunarHoliday"},
        {FestivalSolarHoliday,L"SolarHoliday"},
 //       {8,L"Remember"},
        {FestivalSolarRemind,L"SolarRemind"},
    };
    if(sztag){
        for(int i = 0; i < sizeof(festag)/sizeof(festag[0]); i++){
            if(lstrcmp(festag[i].name,sztag) == 0){
                return festag[i].id;
            }
        }
    }
    return FestivalNone;
}

ReminderType FestivalIO::getReminderType(LPTSTR sztype){
    struct tagFestReminder{
        ReminderType type;
        LPCTSTR name;
    }festreminder[] = {
        {ReminderWeekly,L"每周"},
        {ReminderMonthly,L"每月"},
        {ReminderCountdown,L"倒计"},
        {ReminderCountup,L"顺计"}
    };
    if(sztype){
        for(int i = 0; i < sizeof(festreminder)/sizeof(festreminder[0]); i++){
            if(lstrcmp(festreminder[i].name,sztype) == 0){
                return festreminder[i].type;
            }
        }
    }
    return ReminderNone;
}

void FestivalIO::parsedata(LPCTSTR szdata, FestivalType type){
    if(szdata == 0 || lstrlen(szdata) == 0) return;
    if(type > 7 || type == 0) return;
    //分3部分
    LPTSTR ln = 0;
    C::newstrcpy(&ln,szdata);
    LPTSTR p[3] = {ln, 0, 0};
    int cnt = 1;
    int lendata = lstrlen(ln);
    if(ln[lendata-1] == 0x0d){
        ln[lendata-1] = '\0';
        lendata --;
    }
    for(int i = 0; i < lendata; i++){
        if(ln[i] == '|'){
            ln[i] = '\0';
            p[cnt++] = ln + i + 1;
        }
        if(cnt > 2) break;
    }
    if(p[1] == 0 || p[2] == 0){
        delete ln;
        return;  //没有3部分
    }
    //获取数据
    if(type == FestivalSolarRemind) { //reminder
        ReminderType rtype = getReminderType(p[1]);
        if(rtype == ReminderNone){
            delete ln;
            return;
        }
        //时间或周期
        lpFestival r = new Festival;
        r->type = type;
        r->info1.type = rtype;
        if(rtype == ReminderWeekly){ //周
            swscanf(p[0],L"%d",&r->info0.idx);
            if(r->info0.idx > 7 || r->info0.idx == 0){
                delete ln;
                delete r;
                return;
            }
        }else if(rtype == ReminderMonthly){   //月
            swscanf(p[0],L"%d",&r->info0.idx);
            if(r->info0.idx > 31 || r->info0.idx == 0){
                delete ln;
                delete r;
                return;
            }
        }else{
            swscanf(p[0],L"%04d%02d%02d",&r->info0.year,&r->month,&r->day);
        }
        C::newstrcpy(&r->detail,p[2]);
        festivals[type-1].push_back(r);
    }else{
        lpFestival f = new Festival;
        f->type = type;
        swscanf(p[0],L"%04d%02d%02d",&f->info0.year,&f->month,&f->day);
        C::newstrcpy(&f->info1.name,p[1]);
        C::newstrcpy(&f->detail,p[2]);
        festivals[type-1].push_back(f);
    }
    delete ln;
}

void FestivalIO::parseline(LPCTSTR linetext){
    if(linetext == 0 || lstrlen(linetext) == 0) return;

    static FestivalType tagid = FestivalNone;
    LPTSTR tagstr = 0;
    int lntype = identLineType(linetext);
    switch(lntype){
        case LnTag:
            {
                if(getTag(linetext,tagstr)){
                    tagid = getTagType(tagstr);
                }else{
                    tagid = FestivalNone;
                }
                delete []tagstr;
                break;
            }
        case LnData:
            parsedata(linetext,tagid);
            break;
        default:
            break;
    }
}

bool FestivalIO::parsefile(TCHAR* filename){

    if(!File::FileExists(filename)){
        if(!CreateDefaultFestivalTxt(filename)){
            return false;
        }
    }

    TEXTENCODE_t enc = File::getTextCode(filename);

    int nitems = 0;

    if(enc == ttcAnsi){
        ifstream file;
        file.open(filename,  ios::in | ios::binary);
        if (file.is_open())
        {
            file.seekg(0, ios::end);
            unsigned int nLen = file.tellg();
            char *sbuf = new char[nLen+1];
            file.seekg(0, ios::beg);
            while(!file.eof()){
                if(file.getline(sbuf,nLen).good()){
                    wchar_t *wss = 0;
                    File::chr2wch(sbuf,&wss);
                    parseline(wss);
                    delete [] wss;
                }else{
                    //错误发生
                }
            }
            delete[] sbuf;
        }
        file.close();
    }else if(enc == ttcUnicode ||
        enc == ttcUnicodeBigEndian){
            wifstream ofile;
            ofile.open(filename, ios::in | ios ::binary);
            if (ofile.is_open())
            {
                ofile.seekg(0, ios::end);
                unsigned int nLen = ofile.tellg();
                wchar_t *sbuf = new wchar_t[nLen+1];
                ofile.seekg(2, ios::beg);
                while(!ofile.eof()){
                    if(ofile.getline(sbuf,nLen).good()){
                        parseline(sbuf);
                    }else{
                        //错误发生
                    }
                }
                delete [] sbuf;
            }
            ofile.close();
    }else{
        return false;
    }

    //getEntryData(0,true);   //最后一条记录

    return true;;
}

bool FestivalIO::savefile(TCHAR* filename){
    wofstream file;
    file.open(filename,  ios::out);
    if (file.is_open())
    {
        struct tagFestTag{
            FestivalType id;
            LPCTSTR name;
            LPCTSTR comment;
        }festag[] = {
            {FestivalLunarBirth,L"LunarBirth",L"农历生日"},
            {FestivalSolarBirth,L"SolarBirth",L"公历生日"},
            {FestivalLunar,L"Lunar",L"农历节日"},
            {FestivalSolar,L"Solar",L"公历节日"},
            {FestivalLunarHoliday,L"LunarHoliday",L"农历假日"},
            {FestivalSolarHoliday,L"SolarHoliday",L"公历假日"},
            {FestivalSolarRemind,L"SolarRemind",L"公历提醒"},
        };
        LPCTSTR rnames[] = {
            L"每周",L"每月",L"倒计",L"顺计"
        };
        //LunarBirth
        for(int fsc = 0; fsc < sizeof(festag)/sizeof(festag[0]); fsc++){
            file << L"#" << festag[fsc].comment << endl;
            file << L"[" << festag[fsc].name << L"]" << endl;
            int idx = festag[fsc].id - 1;
            for(int i = 0; i < festivals[idx].size(); i++){
                lpFestival f = festivals[idx].at(i);
                if(f->bdeleted) continue;
                wchar_t d[20];
                if(idx == FestivalSolarRemind - 1){
                    if(f->info1.type == ReminderCountdown ||
                        f->info1.type == ReminderCountup){
                        wsprintf(d,L"%04d%02d%02d",f->info0.year,f->month,f->day);
                    }else{
                        wsprintf(d,L"%d",f->info0.idx);
                    }
                    file << d << L"|" << rnames[f->info1.type - 1] << L"|" << f->detail << endl;
                }else{
                    wsprintf(d,L"%04d%02d%02d",f->info0.year,f->month,f->day);
                    file << d << L"|" << f->info1.name << L"|" << f->detail << endl;
                }
            }
        }
    }
    file.close();
    return true;
}