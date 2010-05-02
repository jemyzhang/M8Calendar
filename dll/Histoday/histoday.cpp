#include "histoday.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include <Mzfc/MzProgressDialog.h> 
#include <fstream>

using namespace std;

static HistodayDB* g_pldb = NULL;

UINT identHistodayLine(LPTSTR linetext){
    UINT nRet = 2;
    if(linetext == NULL) nRet = 0;  //��Ч
    if(linetext[0] == '\0' || linetext[0] == '\x0d' || linetext[0] == '\x0a') nRet = 0;    //�հס����з�
    if(linetext[0] == ' ' && linetext[1] == '=') nRet = 0; //�༭��ע��
    if(linetext[0] == '#' && linetext[1] == '#') nRet = 1;  //����
    return nRet;   //����
}

bool getEntryData(LPTSTR linetext,bool bEnd = false){
    bool nRet = false;
    static HistoryToday item;
    static bool newEntryTitle = false;
    static bool newEntryContent = false;
    if(bEnd){
        if(newEntryTitle){    //����δ�������Ŀ
            //������Ŀ
            if(newEntryContent){
                newEntryContent = false;
                g_pldb->append(&item);
                delete [] item.content;
                item.content = NULL;
                nRet = true;
            }
        }
        return nRet;
    }
    switch(identHistodayLine(linetext)){
        case 1://����
            {
                if(newEntryTitle){    //����δ�������Ŀ
                    //������Ŀ
                    if(newEntryContent){
                        newEntryContent = false;
                        g_pldb->append(&item);
                        delete [] item.content;
                        item.content = NULL;
                        nRet = true;
                    }
                }
                newEntryTitle = true;
                int nlen = lstrlen(linetext);
                wchar_t * wtitle = linetext;//�Թ����ں�##�Ϳո�
                while(*wtitle != ' ' && wtitle < linetext+nlen) wtitle++;
                *wtitle = '\0'; //�ָ����������
                wtitle++;
                wtitle[lstrlen(wtitle) - 1] = '\0'; //ȥ��title��Ļس�����'\r''\x13'
                C::newstrcpy(&item.title,wtitle);
                wchar_t *wDate = linetext + 2;//�Թ�##
                wchar_t *wMonth_Day;
                wchar_t *wYear;
                nlen = lstrlen(wDate);
                if(nlen < 7){ //���ڳ��Ȳ��Ϸ�
                    newEntryTitle = false;
                    break;
                }
                wMonth_Day = wDate + nlen - 4;
                item.month_day = _wtoi(wMonth_Day);
                *wMonth_Day = '\0';
                wYear = wDate;
                item.year = _wtoi(wYear);
                break;
            }
        case 2://����
            newEntryContent = true;
            if(item.content == NULL){
                int wlen = lstrlen(linetext) + 2;
                item.content = new wchar_t[wlen];
                wcscpy_s(item.content,wlen,linetext);
                item.content[wlen - 2] = '\n';
                item.content[wlen - 1] = '\0';
            }else{
                wchar_t *strtmp = item.content;
                int wlen = lstrlen(linetext) + 2 + lstrlen(strtmp);
                item.content = new wchar_t[wlen];
                wcscpy_s(item.content,wlen,strtmp);
                wcscat_s(item.content,wlen,linetext);
                item.content[wlen - 2] = '\n';
                item.content[wlen - 1] = '\0';
                delete [] strtmp;
            }
            break;
        case 0://��Ч
        default:
            break;
    }
    return nRet;
}

bool ImportData(TCHAR* filename,HWND m_hWnd){
    if(g_pldb == NULL) return false;

	if(!File::FileExists(filename)){
		return false;
	}
    if(::MzMessageBoxEx(m_hWnd,L"���ֿɸ��µ�����Դ���Ƿ����ڸ��£�",L"����",MZ_YESNO) != 1){
        DateTime::waitms(0);
        return true;
    }
    DateTime::waitms(0);
	MzProgressDialog m_Progressdlg;
	m_Progressdlg.SetShowTitle(true);
	m_Progressdlg.SetShowInfo(true);
	m_Progressdlg.SetRange(0,100);
	m_Progressdlg.SetTitle(L"������ʷ�ϵĽ��������У����Ժ�...");
	m_Progressdlg.SetInfo(L"�ļ�������");
	m_Progressdlg.BeginProgress(m_hWnd);

	TEXTENCODE_t enc = File::getTextCode(filename);

	int nitems = 0;
	unsigned int nbytes = 0;
	g_pldb->beginTrans();
    int nPercent = 0;
    int _nPercent = 1;
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
                    nbytes += strlen(sbuf);
                    wchar_t *wss;
                    File::chr2wch(sbuf,&wss);
                    if(getEntryData(wss)){
                        nitems++;
                        nPercent = 100 * nbytes / nLen;
                        if(nPercent != _nPercent){
                            _nPercent = nPercent;
                            wchar_t info[32];
                            wsprintf(info,L"����%d����¼...%d%%",nitems,nPercent);
                            m_Progressdlg.SetInfo(info);
                            m_Progressdlg.SetCurValue(80 * nbytes / nLen);
                            m_Progressdlg.UpdateProgress();
                            DateTime::waitms(1);
                        }
                    }
                }else{
                    //������
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
                    nbytes += lstrlen(sbuf);
                    if(getEntryData(sbuf)){
                        nitems++;
                        wchar_t info[32];
                        wsprintf(info,L"�����¼��...%d",nitems);
                        m_Progressdlg.SetInfo(info);
                        m_Progressdlg.SetCurValue(80 * nbytes / nLen);
                        m_Progressdlg.UpdateProgress();
                        DateTime::waitms(1);
                    }
                }else{
                    //������
                }
            }
            delete [] sbuf;
		}
        ofile.close();
	}

    getEntryData(0,true);   //���һ����¼

	m_Progressdlg.SetInfo(L"д��������...");
	m_Progressdlg.SetCurValue(85);
	m_Progressdlg.UpdateProgress();
	g_pldb->commitTrans();
	m_Progressdlg.SetInfo(L"��������...");
	m_Progressdlg.SetCurValue(90);
	m_Progressdlg.UpdateProgress();
	g_pldb->indexDatabase();
	m_Progressdlg.SetInfo(L"�������");
	m_Progressdlg.SetCurValue(100);
	m_Progressdlg.EndProgress();
	File::DelFile(filename);
	return true;
}

HistodayDB *CreateHistodayDatabase(){
    if(g_pldb == NULL){
        g_pldb = new HistodayDB();
    }
    return g_pldb;
}