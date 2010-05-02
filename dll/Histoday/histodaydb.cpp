#include "histodaydb.h"
#include <cMzCommon.h>
using namespace cMzCommon;

using std::exception;

#ifdef _DEBUG
#define dbg_printf(s) printf("%s\n",s);
#else
#define dbg_printf(s)
#endif

#if 0
#ifdef _DEBUG
#define TRY try
#define CATCH catch
#else
#define TRY __try
#define CATCH __except
#endif
#else
#define TRY try
#define CATCH catch
#endif

HistodayDB::HistodayDB() {
}

HistodayDB::~HistodayDB() {
	disconnect();
    clean();
}

bool HistodayDB::connect(wchar_t *path){
	bool bRet = true;
	TRY{
		sqlconn.open(path);
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	return bRet;
}

bool HistodayDB::disconnect(void){
	bool bRet = true;
	TRY{
		sqlconn.close();
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	return bRet;
}

void HistodayDB::recover(){
	createDefaultDatabase();
}

bool HistodayDB::decrypt(wchar_t* pwd, int len){
	char* temp = new char[len*2+1];
	int bytecnt = 0;
	wchar_t *p = pwd;
	char* b_pwd = temp;
	for(int i = 0; i < len; i++){
		wchar_t w = *p++;
		if(w&0xff){
			*b_pwd++ = w&0xff;
			bytecnt++;
		}
		if((w>>8)&0xff){
			*b_pwd++ = (w>>8)&0xff;
			bytecnt++;
		}
	}
	*b_pwd = '\0';
    bool bRet = true;
    TRY{
	    sqlconn.decypt(temp,bytecnt);
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
    delete temp;
	return bRet;
}

bool HistodayDB::encrypt(wchar_t* pwd, int len){
	char* temp = new char[len*2+1];
	int bytecnt = 0;
	wchar_t *p = pwd;
	char* b_pwd = temp;
	for(int i = 0; i < len; i++){
		wchar_t w = *p++;
		if(w&0xff){
			*b_pwd++ = w&0xff;
			bytecnt++;
		}
		if((w>>8)&0xff){
			*b_pwd++ = (w>>8)&0xff;
			bytecnt++;
		}
	}
	*b_pwd = '\0';
    bool bRet = true;
    TRY{
	    sqlconn.encypt(temp,bytecnt);
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	delete temp;
	return bRet;
}

bool HistodayDB::beginTrans(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"begin;");
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	return bRet;
}

bool HistodayDB::commitTrans(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"commit;");
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	return bRet;
}

bool HistodayDB::indexDatabase(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(
            L"create index idx_history on '"
            TABLE_HISTORY
            L"'(month_day,title)");
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	return bRet;
}

bool HistodayDB::reorgDatebase(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"VACUUM");
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	return bRet;
}

bool HistodayDB::checkDatabaseVersion(){
	bool bRet = true;
	TRY{
        bRet = sqlconn.executeint(
            L"select count(*) from sqlite_master where name='"
            TABLE_HISTORY
            L"'") > 0;
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	return bRet;
}

void HistodayDB::clean(){
	clear();
}

bool HistodayDB::exists(LPHistoryToday his) {
	bool bRet = true;
	TRY{
		sqlite3_command cmd(this->sqlconn, 
			L"select count(*) from '"
			TABLE_HISTORY
			L"' where title=?");
		//bind title
		cmd.bind(1,his->title,lstrlen(his->title) * sizeof(his->title[0]));
		//get result
		bRet = (cmd.executeint() > 0);
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}
	return bRet;
}

bool HistodayDB::append(LPHistoryToday his) {
    bool bRet = true;
	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"insert into '"
			TABLE_HISTORY
			L"' (YEAR,MONTH_DAY,TITLE,CONTENT) values(?,?,?,?)");

		cmd.bind(1,(int)his->year);
		cmd.bind(2,(int)his->month_day);
		cmd.bind(3,his->title,lstrlen(his->title) * sizeof(his->title[0]));
		cmd.bind(4,his->content,lstrlen(his->content) * sizeof(his->content[0]));
		cmd.executenonquery();
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}

    return bRet;
}

void HistodayDB::createDefaultDatabase() {
	TRY{
		int count = sqlconn.executeint(
			L"select count(*) from sqlite_master where name='"
			TABLE_HISTORY
			L"';");
		if(count == 0){
			sqlconn.executenonquery(
				L"create table '"
				TABLE_HISTORY
				L"' (YEAR numeric not null, \
				 MONTH_DAY numeric not null, \
				 TITLE text NOT NULL primary key, \
				 CONTENT text NOT NULL);");
		}
	}CATCH(exception &ex){
		dbg_printf(ex.what());
	}

    return;
}

int HistodayDB::query_size() {
    return vhistory.size();
}

LPHistoryToday HistodayDB::query_at(int index) {
    if(index >= vhistory.size()) return NULL;
    return vhistory.at(index);
}

void HistodayDB::clear(){
	if(vhistory.size()){
		for(int i = 0;i < vhistory.size(); i++){
			LPHistoryToday his = vhistory.at(i);
			delete his;
		}
		vhistory.clear();
	}
}

bool HistodayDB::search(sqlite3_command& cmd){
	//do some clear work
	clear();

	bool bRet = true;
	TRY{
		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			LPHistoryToday h = new HistoryToday;
			h->year = reader.getint(0);
			h->month_day = reader.getint(1);
			C::newstrcpy(&h->title,(LPWSTR) reader.getstring16(2).c_str());
			C::newlinecpy(&h->content,(LPWSTR) reader.getstring16(3).c_str(),35);
			vhistory.push_back(h);
		}
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}

	if(vhistory.empty()) bRet = false;
    return bRet;
}

bool HistodayDB::queryDate(int month_day){
    bool bRet = true;
	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"select * from '"
			TABLE_HISTORY
			L"' where MONTH_DAY=? order by YEAR DESC");

		cmd.bind(1,month_day);
		bRet = search(cmd);
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}

    return bRet;
}

bool HistodayDB::queryDetail(LPHistoryToday his){
    bool bRet = true;
	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"select CONTENT from '"
			TABLE_HISTORY
			L"' where TITLE=?");

		cmd.bind(1,his->title,lstrlen(his->title) * sizeof(his->title[0]));

		C::newstrcpy(&his->content,cmd.executestring16().c_str());
	}CATCH(exception &ex){
		dbg_printf(ex.what());
		bRet = false;
	}

    return bRet;
}

