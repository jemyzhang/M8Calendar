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
	DWORD imonth;	//ũ�����������
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
	//����ũ��ʱ�䣬ͬʱ��ũ��ת��Ϊ����
	void setLunarDate(DWORD year, DWORD month, DWORD day) {
		lunar.year = year;
		lunar.month = month;
		lunar.day = day;
		LunarToSolar();
	};
	//��ȡ��������
	LSDate getSolarDate() { return solar; }
	LSDate getLunarDate() { return lunar; }
	//�Ƿ��Խ���Ϊ�¸�֧��ʼ
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
	// �Ƿ�����
	bool IsSolarLeapYear(DWORD);
	//�Ƿ�ũ������
	bool isLunarLeapMonth();
	// ����һ���µ�����
	unsigned char SolarDaysInMonth(DWORD year, DWORD month);
	// �������������֮��Ĳ�ֵ
	int SolarDaysFromBaseDate();
	// ����ת��Ϊũ��
	void SolarToLunar();
	// ũ��ת��Ϊ����
	void LunarToSolar();
	// ����ũ������
	//Returns 0 if no lunar leap month
	unsigned char LunarLeapMonth();
	// ����ũ��ÿ���µ�����
	// Stores data in a global list lunarDaysInMonth
	void CalcLunarDaysInMonth();
	//ũ��������
	CMzString LunarMonth();
	//ũ��������
	CMzString LunarDay();
	//�����������ũ��������
	CMzString OriginalLunarDay();
	//���ظ�֧��
	CMzString GanZhiYear(unsigned char *gan = 0, unsigned char *zhi = 0);
	//���ظ�֧��
	CMzString GanZhiMonth(unsigned char *gan = 0, unsigned char *zhi = 0);
	//���ظ�֧��
	CMzString GanZhiDay(unsigned char *gan = 0, unsigned char *zhi = 0);
	//������Ф
	CMzString Zodiac(unsigned char *zodiac = 0);
	// ����ũ���������
	unsigned int LunarYearDays();
	//��������
	// 0 is Monday, 1 is TuesDay, ...
	unsigned char Weekday();
	//���ع�������
	bool SolarHoliday(wchar_t* s);
	//����ũ������
	bool LunarHoliday(wchar_t* s);
	// Returns 24 solar terms in the solar year
	LS24TERM_ptr SolarTerm(void);
	//����ÿ���½����ĵ�һ��
	DWORD GetFirstJieqiDay();
	//==============����==============
	char CalConv2(int yy,int mm,int dd,int y,int d,int m,int dt,int nm,int nd);
	//�ˣ���
	//�����������true, ���ݴ����yi��
	bool HuangliYiJi(CMzString &yi, CMzString &ji);
public:
	unsigned int julianDayFromDate(int year, int month, int day);
	void getDateFromJulianDay(unsigned int julianDay, DWORD *year, DWORD *month, DWORD *day);
private:
	LSDate baseDate; // 1900��1��31�������������������³�һ�����գ�
	LSDate solar;	//��������
	LSDate lunar;	//ũ������
	LS24TERM_t terms[2];	//24����������
	unsigned int lunarYearDays;
	unsigned char lunarDaysInMonth[13];
	bool _monthGanzhiByJieqi;
};
