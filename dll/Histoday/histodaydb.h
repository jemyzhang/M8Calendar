#pragma once
#include <cMzCommonDll.h>
#include <windows.h>

#include <sqlite3x.hpp>
using namespace sqlite3x;

#include <vector>
using std::vector;

#ifdef _DEBUG
#define DEFAULT_DB		L"Program Files\\M8Calendar\\calendar.db"
#else
#define DEFAULT_DB		L"\\Disk\\Programs\\M8Calendar\\calendar.db"
//#define DEFAULT_DB		"Program Files\\M8Calendar\\calendar.db"
#endif

#define TABLE_HISTORY	L"HISTORY_v1"

typedef struct tagHistoryToday {
    DWORD year;
    DWORD month_day;
    wchar_t* title;
    wchar_t* content;
    tagHistoryToday(){
        year = 0;
        month_day = 0;
        title = NULL;
        content = NULL;
    }
    ~tagHistoryToday(){
        if(title){
            delete [] title;
            title = NULL;
        }
        if(content){
            delete [] content;
            content = NULL;
        }
    }
} HistoryToday, *LPHistoryToday;

class COMMON_API HistodayDB {
public:
    HistodayDB( );
    ~HistodayDB( );
public:
	bool connect(wchar_t *path);
	bool disconnect();
	void recover();
	bool decrypt(wchar_t* pwd,int len);
	bool encrypt(wchar_t* pwd,int len);
	void clean();	//clean rabish
private:
    vector<LPHistoryToday> vhistory;
    //sqlite operations
private:
	sqlite3_connection sqlconn;
protected:
    void connectDatabase(const wchar_t*);
    void disconnectDatabase();
    void createDefaultDatabase();
	bool decrytpDatabase(const char* pwd,int len);	//true: successful
	bool setDatabasePassword(const char* pwd,int len);	//true: successful
private:
	void clear();
protected:
	bool search(sqlite3_command& cmd);
public:
	bool exists(LPHistoryToday his);
	bool append(LPHistoryToday his);
    int query_size();
	LPHistoryToday query_at(int index);
	bool queryDate(int month_day);
	bool queryDetail(LPHistoryToday his);
public:
	bool beginTrans();
	bool commitTrans();
	bool indexDatabase();
	bool reorgDatebase();
	bool checkDatabaseVersion();
};

