#pragma once
#include <cMzCommonDll.h>

typedef struct Holiday{
	DWORD month;
	DWORD day;
	wchar_t* name;
}HOLIDAY_t;

typedef struct LSDate{
	LSDate(DWORD y = -1, DWORD m = -1, DWORD d = -1, DWORD w = -1){
		year = y;
		month = m;
		day = d;
		weekday = w;
	}
	DWORD year;
	DWORD month;
	DWORD day;
	DWORD weekday;
	DWORD imonth;	//农历，区分闰否
}LSDATE_t;

typedef struct LS24Term{
	DWORD day;
	const wchar_t* name;
}LS24TERM_t, *LS24TERM_ptr;

class COMMON_API LCAL
{
public:
	LCAL(DWORD year, DWORD month = -1, DWORD day = -1, DWORD week = -1);
    ~LCAL();
public:
	void setDay(DWORD d) { solar.day = d; SolarToLunar(); };
	//设置农历时间，同时将农历转换为公历
	void setLunarDate(DWORD year, DWORD month, DWORD day) {
		lunar.year = year;
		lunar.month = month;
		lunar.day = day;
		LunarToSolar();
	};
	//获取公历日期
	LSDate getSolarDate() { return solar; }
	LSDate getLunarDate() { return lunar; }
	//是否以节气为月干支起始
	void setLunarMonthGanZhiMode(bool byjieqi){
		_monthGanzhiByJieqi = byjieqi;
	}
public:
	// Calculate days in a year
	unsigned int daysOfYear(LSDate);
	// Calculate days diff
	int diffDate(LSDate,LSDate);
    int getWeekDay(int year,int month, int day);
public:
	// 是否闰年
	bool IsSolarLeapYear(DWORD);
	//是否农历闰月
	bool isLunarLeapMonth();
	// 计算一个月的天数
	unsigned char SolarDaysInMonth(DWORD year, DWORD month);
	// 计算与基础日期之间的差值
	int SolarDaysFromBaseDate();
	// 公历转换为农历
	void SolarToLunar();
	// 农历转换为公历
	void LunarToSolar();
	// 计算农历闰月
	//Returns 0 if no lunar leap month
	unsigned char LunarLeapMonth();
	// 计算农历每个月的天数
	// Stores data in a global list lunarDaysInMonth
	void CalcLunarDaysInMonth();
	//农历月名称
	CMzString LunarMonth();
	//农历日名称
	CMzString LunarDay();
	//不经过处理的农历日名称
	CMzString OriginalLunarDay();
	//返回干支年
	CMzString GanZhiYear(unsigned char *gan = 0, unsigned char *zhi = 0);
	//返回干支月
	CMzString GanZhiMonth(unsigned char *gan = 0, unsigned char *zhi = 0);
	//返回干支日
	CMzString GanZhiDay(unsigned char *gan = 0, unsigned char *zhi = 0);
	//返回生肖
	CMzString Zodiac(unsigned char *zodiac = 0);
	// 返回农历年的天数
	unsigned int LunarYearDays();
	//返回星期
	// 0 is Monday, 1 is TuesDay, ...
	unsigned char Weekday();
	//返回公历节日
	bool SolarHoliday(wchar_t* s);
	//返回农历节日
	bool LunarHoliday(wchar_t* s);
	// Returns 24 solar terms in the solar year
	LS24TERM_ptr SolarTerm(void);
	//返回每个月节气的第一天
	DWORD GetFirstJieqiDay();
	//==============黄历==============
	char CalConv2(int yy,int mm,int dd,int y,int d,int m,int dt,int nm,int nd);
	//宜，忌
	//特殊情况返回true, 内容存放在yi中
	bool HuangliYiJi(CMzString &yi, CMzString &ji);
public:
	unsigned int julianDayFromDate(int year, int month, int day);
	void getDateFromJulianDay(unsigned int julianDay, DWORD *year, DWORD *month, DWORD *day);
private:
	LSDate baseDate; // 1900年1月31日星期三（庚子年正月初一壬寅日）
	LSDate solar;	//公历日期
	LSDate lunar;	//农历日期
	LS24TERM_t terms[2];	//24节气发生日
	unsigned int lunarYearDays;
	unsigned char lunarDaysInMonth[13];
	bool _monthGanzhiByJieqi;
};
