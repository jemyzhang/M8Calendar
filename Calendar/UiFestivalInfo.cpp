#include "UiFestivalInfo.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include <lcal.h>
#include <festio.h>
extern FestivalIO *pfestival;

const COLORREF festival_colors[8] = {
    RGB(235,51,153), //�ۺ�ɫ
    RGB(153,0,204),//��ɫ
    RGB(192,216,177),//��ɫ
    RGB(142,192,192),//����ɫ
    RGB(0,225,225),//����ɫ, ��ɫ
    RGB(189,167,225),//����ɫ��
    RGB(0,182,0),//������ɫ
    RGB(0,192,192),//����ɫ
};

int UiFestivalInfo::OnLButtonDblClk(UINT fwKeys, int xPos, int yPos){
    PROCESS_INFORMATION pi;
    wchar_t exepath[MAX_PATH];
    if(File::GetCurrentPath(exepath)){
        wcscat(exepath,L"FestivalEditor.exe");
    }else{
        wsprintf(exepath,L"FestivalEditor.exe");
    }
    ::CreateProcess(exepath, 
        NULL , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
    return 0;
}

void UiFestivalInfo::appendInfo(wstring info, UCHAR t){
    appendInfo(info.c_str(),t);
}

void UiFestivalInfo::appendInfo(LPCTSTR info, UCHAR t){
    if(t < 8){
        if(info){
            LPDrawFestInfo pi = new DrawFestInfo;
            pi->info = info;
            pi->t = t;
            infos.push_back(pi);
        }
    }
}

void UiFestivalInfo::clearInfo(){
    for(int j = 0; j < infos.size(); j++){
        infos.at(j)->info.clear();
        delete infos.at(j);
    }
    infos.clear();
}

void UiFestivalInfo::apply(){
    wstring s;
    for(int i = 0; i < infos.size(); i++){
        LPDrawFestInfo p = infos.at(i);
        s += p->info;
        s += L"\n";
    }
    this->SetText(s.c_str());
    for(int i = 0; i < infos.size(); i++){
        LPDrawFestInfo p = infos.at(i);
        UpdateFontColor(festival_colors[p->t],i,0,i,100);
    }
}

void UiFestivalInfo::SetDate(DWORD y, DWORD m, DWORD d){
    LCAL _lstm(y,m,d);
    _lstm.SolarToLunar();

    wstring sfestinfo;
    clearInfo();

    if(pfestival){
        if(!_lstm.isLunarLeapMonth()){
            LSDate l = _lstm.getLunarDate();

            //ũ������
            pfestival->queryBirthday(_lstm.getLunarDate().month,_lstm.getLunarDate().day,true);
            int bsz = pfestival->BirthdaySize();
            for(int i = 0; i < bsz; i++){
                lpFestival pf = pfestival->Birthday(i);
                if(pf->detail && pf->info0.year <= y){
                    wchar_t msg[200] = {0};
                    wsprintf(msg,pf->detail,y - pf->info0.year + 1);
                    sfestinfo = L"[";
                    sfestinfo += pf->info1.name;
                    sfestinfo += L"] : ";
                    sfestinfo += msg;
                    appendInfo(sfestinfo,0);
                }
            }

            pfestival->query(l.month,l.day,FestivalLunar);
            for(int i = 0; i < pfestival->query_size(); i++){
                lpFestival pf = pfestival->query_at(i);
                sfestinfo = L"[";
                sfestinfo += pf->info1.name;
                sfestinfo += L"] : ";
                sfestinfo += pf->detail;
                appendInfo(sfestinfo,2);
            }

            pfestival->query(l.month,l.day,FestivalLunarHoliday);
            for(int i = 0; i < pfestival->query_size(); i++){
                lpFestival pf = pfestival->query_at(i);
                sfestinfo = L"[";
                sfestinfo += pf->info1.name;
                sfestinfo += L"] : ";
                sfestinfo += pf->detail;
                appendInfo(sfestinfo,2);
            }
        }
        LSDate l = _lstm.getSolarDate();

        pfestival->queryBirthday(_lstm.getSolarDate().month,_lstm.getSolarDate().day,false);
        int bsz = pfestival->BirthdaySize();
        for(int i = 0; i < bsz; i++){
            lpFestival pf = pfestival->Birthday(i);
            if(pf->detail && pf->info0.year <= y){
                wchar_t msg[200] = {0};
                wsprintf(msg,pf->detail,y - pf->info0.year + 1);
                sfestinfo = L"[";
                sfestinfo += pf->info1.name;
                sfestinfo += L"] : ";
                sfestinfo += msg;
                appendInfo(sfestinfo,1);
            }
        }

        pfestival->query(l.month,l.day,FestivalSolar);
        for(int i = 0; i < pfestival->query_size(); i++){
            lpFestival pf = pfestival->query_at(i);
            sfestinfo = L"[";
            sfestinfo += pf->info1.name;
            sfestinfo += L"] : ";
            sfestinfo += pf->detail;
            appendInfo(sfestinfo,3);
        }
        pfestival->query(l.month,l.day,FestivalSolarHoliday);
        for(int i = 0; i < pfestival->query_size(); i++){
            lpFestival pf = pfestival->query_at(i);
            sfestinfo = L"[";
            sfestinfo += pf->info1.name;
            sfestinfo += L"] : ";
            sfestinfo += pf->detail;
            appendInfo(sfestinfo,3);
        }
    }
    if(sfestinfo.length()){
        SetText(L"\0");
        apply();
    }else{
        SetText(L"�޽�����Ϣ");
    }
    Invalidate();
}

