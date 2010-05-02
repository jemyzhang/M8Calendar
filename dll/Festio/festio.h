#pragma once

#include <cMzCommonDll.h>
#include <vector>

using std::vector;

typedef enum tagFestivalType{
    FestivalNone    =   0,
    FestivalLunarBirth  =   1,
    FestivalSolarBirth  =   2,
    FestivalLunar   =   3,
    FestivalSolar   =   4,
    FestivalLunarHoliday    =   5,
    FestivalSolarHoliday    =   6,
    FestivalSolarRemind =   7,
    FestivalMaxTypeSize,
}FestivalType;

typedef enum tagReminderType{
    ReminderNone    =   0,
    ReminderWeekly  =   1,
    ReminderMonthly  =   2,
    ReminderCountdown   =   3,
    ReminderCountup   =   4,
    ReminderMaxTypeSize,
}ReminderType;

typedef struct tagFestival{
    FestivalType type;
    union {
        UINT year;
        UINT idx;
    }info0;
	DWORD month;
	DWORD day;
    union{
	    wchar_t* name;
        ReminderType type;
    }info1;
    wchar_t* detail;
    bool bselected;
    bool bdeleted;
    tagFestival(){
        type = FestivalNone;
        info0.year = 0; month = 0; day = 0;
        info1.name = 0; detail = 0;
        bselected = false; bdeleted = false;
    }
    ~tagFestival(){
        if(type != FestivalSolarRemind && info1.name) delete [] info1.name;
        if(detail) delete [] detail;
    }
}Festival,*lpFestival;

typedef struct tagReminder{
    DWORD year_idx; //�����ÿ��/ÿ��
	DWORD month;
	DWORD day;
	DWORD type;     //ÿ��/ÿ��/����/˳��
    wchar_t* detail;
    tagReminder(){
        year_idx = 0; month = 0; day = 0;
        type = 0;
        detail = 0;
    }
    ~tagReminder(){
        if(detail) delete [] detail;
    }
}Reminder,*lpReminder;

class COMMON_API FestivalIO{
    enum tagLineType{
        LnComment   =  0,
        LnTag       =  1,
        LnData      =  2,
        LnInvalid   =  3,
    };
public:
    FestivalIO(LPCTSTR szpath);
    ~FestivalIO();
public:
    bool load();
    bool save();
    bool reload();
    void unload();
public:
    void query_all(FestivalType t, ReminderType rt = ReminderNone);
    bool query(DWORD month, DWORD day, FestivalType t, ReminderType rt = ReminderNone);
    int query_size();
    lpFestival query_at(int idx);
    bool append(lpFestival pf);
public:
	//���ع�������
	bool SolarHoliday(DWORD month, DWORD day, wchar_t* s, int szmax = 3);
	//����ũ������
	bool LunarHoliday(DWORD month, DWORD day, wchar_t* s, int szmax = 3);

    //�������ո���
	bool queryBirthday(DWORD month, DWORD day, bool blunar);
    int BirthdaySize();
    lpFestival Birthday(int idx);
    //��������
	bool queryReminder(DWORD weekday, DWORD day, ReminderType type);
    int ReminderSize();
    lpFestival Reminder(int idx);
private:
    vector<lpFestival> fquery;
private:
    int identLineType(LPCTSTR);
    //return 0 if no tag found
    bool getTag(LPCTSTR, LPTSTR &);
    FestivalType getTagType(LPTSTR);
    ReminderType getReminderType(LPTSTR);
private:
    void parsedata(LPCTSTR szdata, FestivalType type);
    void parseline(LPCTSTR linetext);
    bool parsefile(TCHAR* filename);
    bool savefile(TCHAR* filename);
private:
    vector<lpFestival> festivals[FestivalMaxTypeSize-1];
    LPWSTR pfestpath;
};
