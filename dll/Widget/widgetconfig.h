#ifndef _UI_CALENDARDACONFIG_H
#define _UI_CALENDARDACONFIG_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include <cMzConfig.h>
// Main window derived from CMzWndEx

class ItemConfig{
public:
    void Init(LPCTSTR keyname,
        DWORD x = 0, DWORD y = 0,
        DWORD w = 0, DWORD h = 0,
        DWORD show = 0){
        if(keyname == 0) keyname = L"unknown";
        X.InitKey(keyname,L"X",x);
        Y.InitKey(keyname,L"Y",y);
        W.InitKey(keyname,L"W",w);
        H.InitKey(keyname,L"H",h);
        Show.InitKey(keyname,L"Show",show);
    }
public:
    cMzConfig X;
    cMzConfig Y;
    cMzConfig W;
    cMzConfig H;
    cMzConfig Show;
};

class IconItemConfig : public ItemConfig{
};

class StringItemConfig : public ItemConfig{
public:
    void Init(LPCTSTR keyname,
        DWORD x = 0, DWORD y = 0,
        DWORD w = 0, DWORD h = 0,
        DWORD sz = 0, DWORD color = RGB(0,0,0),
        DWORD al = DT_CENTER, DWORD weight = FW_BOLD,
        DWORD show = 0,
        bool bicolor = false, DWORD color2 = RGB(0,0,0)){
			ItemConfig::Init(keyname,x,y,w,h,show);
        if(keyname == 0) keyname = L"unknown";
        FontSize.InitKey(keyname,L"FontSize",sz);
        if(bicolor){
            Color.InitKey(keyname,L"Color1",color);
            Color2.InitKey(keyname,L"Color2",color2);
        }else{
            Color.InitKey(keyname,L"Color",color);
        }
        Align.InitKey(keyname,L"Alignment",al);
        Weight.InitKey(keyname,L"Weight",weight);
    }
public:
    cMzConfig FontSize;
    cMzConfig Color;
    cMzConfig Color2;
    cMzConfig Align;
    cMzConfig Weight;
};

extern HMODULE GetThisModule();

class WidgetConfig : public AppConfigIni{
public:
	WidgetConfig()
        : AppConfigIni() {
        wchar_t currpath[MAX_PATH];
        wchar_t ini_path[MAX_PATH];
        if(File::GetCurrentPath(currpath,GetThisModule())){
            wsprintf(ini_path,L"%s\\widgetcalendar.ini",currpath);
        }else{
            wsprintf(ini_path,L"widgetcalendar.ini");
        }
        SetupIniFullPath(ini_path);
        CreateIni();
        InitIniKey();
	}
protected:
	void InitIniKey(){
        InitIniWidgetSize();
        Zodiac.Init(L"ZodiacImage",315,10,90,90,1);
        YearMonth.Init(L"YearMonth",0,20,240,35,
            36,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        Day.Init(L"Day",0,60,240,120,
            120,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1,
            true,RGB(255,64,64));
        WeekdayCN.Init(L"WeekdayCN",0,180,240,35,
            30,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        WeekdayEn.Init(L"WeekdayEn",0,220,240,35,
            26,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        LunarYear.Init(L"LunarYear",240,105,240,22,
            20,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        LunarMonthDay.Init(L"LunarMonthDay",240,127,240,22,
            20,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        LunarHoliday.Init(L"LunarHoliday",240,149,240,22,
            20,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        Jieqi.Init(L"Jieqi",240,171,240,22,
            20,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        SolarHoliday.Init(L"SolarHoliday",240,193,240,22,
            20,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        GanzhiMonthDay.Init(L"GanzhiMonthDay",240,215,240,22,
            20,RGB(255,255,255),DT_CENTER|DT_VCENTER,FW_BOLD,1);
        //
        LunarBirth.Init(L"LunarBirthdayIcon",200,70,30,30,1);
        SolarBirth.Init(L"SolarBirthdayIcon",235,70,30,30,1);

        WeekReminder.Init(L"WeeklyReminderIcon",200,110,30,30,1);
        MonthReminder.Init(L"MonthlyReminderIcon",235,110,30,30,1);
        CountDown.Init(L"CountDownIcon",200,150,30,30,1);
        CountUp.Init(L"CountUpIcon",235,150,30,30,1);

        ClockHour.Init(L"ClockHour");
        ClockMinute.Init(L"ClockMinute");
        ClockColon.Init(L"ClockColon");

        LinkedExe.InitKey(L"Widget",L"DoubleClick",L"\\Disk\\Programs\\Tools\\M8Calendar\\M8Calendar.exe");
	}
    void InitIniWidgetSize(){
        WidgetSizeW.InitKey(L"Widget",L"Width",4);
        WidgetSizeH.InitKey(L"Widget",L"Height",2);
    }

public:
    //生肖
    IconItemConfig Zodiac;
    //年月
    StringItemConfig YearMonth;
    //日
    StringItemConfig Day;
    //中文星期
    StringItemConfig WeekdayCN;
    //英文星期
    StringItemConfig WeekdayEn;
    //农历干支年
    StringItemConfig LunarYear;
    //农历月日
    StringItemConfig LunarMonthDay;
    //干支月日
    StringItemConfig GanzhiMonthDay;
    //节气
    StringItemConfig Jieqi;
    //农历节日
    StringItemConfig LunarHoliday;
    //公历节日
    StringItemConfig SolarHoliday;

    //农历生日
    IconItemConfig LunarBirth;
    //公历生日
    IconItemConfig SolarBirth;
    //周事件
    IconItemConfig WeekReminder;
    //月事件
    IconItemConfig MonthReminder;
    //倒计时
    IconItemConfig CountDown;
    //顺计时
    IconItemConfig CountUp;

    //时钟 小时
    StringItemConfig ClockHour;
    //时钟 分
    StringItemConfig ClockMinute;
    //时钟 冒号
    StringItemConfig ClockColon;
    //widget
    cMzConfig WidgetSizeW;
    cMzConfig WidgetSizeH;

    cMzConfig LinkedExe;

};

#endif /*_UI_CALENDARDACONFIG_H*/