
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "XXbgService.h"
#include "MainFrm.h"
#include "MyTTrace.h"
#include "CBase64.h"
#include "GHook/GHook.h"
#include "Json/json.h"
#include "utils.h"
#include "iocp.h"
#include "CCentOneReader.h"
#include "CCentOneCamera.h"
#include "CCentReader.h"
#include "CGeitCamera.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib,"GHook.lib")
#pragma comment ( lib, "json_vc71_libmtd.lib" ) 
//#pragma comment(lib,"iocp.lib")

HANDLE g_hIoRes = NULL;
CString msgStr = "";
CString sendMsg="";
CString wname;
int long_time_interval,short_time_interval;
HWND hWnd;
HWND hCurFocus;

void __stdcall WriteEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len);
void __stdcall ExceptEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len);
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_TRAYNOTIFY, OnTrayNotify)
	ON_MESSAGE(WM_SCREENSHOT, OnScreenshot)
	ON_MESSAGE(WM_SCREENDLG_MSG, OnScreenDlgMessage)
	ON_MESSAGE(WM_CONTINUE_INPUT, OnContinueInput)
	// ��ȡ�����С���¼����д���
	ON_WM_SYSCOMMAND()
	// �رմ����¼�
	ON_MESSAGE_VOID(WM_CLOSE, OnMyClose)
	//ON_MESSAGE(WM_CLOSE, OnClose)
	ON_COMMAND(ID_QUIT, OnQuit)
	ON_COMMAND(ID_TEST, OnManualInput)
	ON_COMMAND(ID_SETTING, OnSettingWin)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	screenshotDlg = NULL;
	input = NULL;
	settingDlg = NULL;
	pBaseReadIDCardInfo = NULL;
	pBaseSaveDeskPic = NULL;
	pBaseSaveEnvPic = NULL;


	// ��ʼ�����֤ʶ���ǣ���������������ͷ�������ǻ�������ͷ
	InitDevice();
}

CMainFrame::~CMainFrame()
{
	m_nid.hIcon = NULL;
	Shell_NotifyIcon(NIM_DELETE,&m_nid);
	if (screenshotDlg != NULL)
	{
		delete screenshotDlg;
		screenshotDlg = NULL;
	}
	if (input != NULL)
	{
		delete input;
		input = NULL;
	}
	if (settingDlg != NULL)
	{
		delete settingDlg;
		settingDlg = NULL;
	}
	if (pBaseReadIDCardInfo != NULL)
	{
		delete pBaseReadIDCardInfo;
		pBaseReadIDCardInfo = NULL;
	}
	if (pBaseSaveDeskPic != NULL)
	{
		delete pBaseSaveDeskPic;
		pBaseSaveDeskPic = NULL;
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// ���Ͻ�x����
	int nWinXPos = 0;
	// ���Ͻ�y����
	int nWinYPos = 0;
	// ���ڿ�
	int nWinWidth = 0;
	// ���ڸ�
	int nWinHigh = 0;
	// ��ʱ�ַ���
	CString tmpString;
	CString iniDir = GetAppPath()+"\\win.ini";

	// ��ȡ�����ļ����ϴ��˳��Ĵ����Ƿ������״̬������ָ�
	GetPrivateProfileString("Information", "IsZoomed", "0", tmpString.GetBuffer(31), 31, iniDir);
	tmpString.ReleaseBuffer();
	if (0 == tmpString.Compare("1"))
	{
		// �ϴ��˳�Ϊ���״̬�������´���ʾΪ���
		cs.style |= WS_MAXIMIZE;
	}
	else
	{
		// �ϴ��˳�Ϊ�����״̬�������´���ʾ��λ��
		// ��ȡ�����ļ��еĴ�����ʾ��λ��
		GetPrivateProfileString("Information", "WinXPos", "-1", tmpString.GetBuffer(31), 31, iniDir);
		tmpString.ReleaseBuffer();
		nWinXPos = _ttoi(tmpString);

		GetPrivateProfileString("Information", "WinYPos", "-1", tmpString.GetBuffer(31), 31, iniDir);
		tmpString.ReleaseBuffer();
		nWinYPos = _ttoi(tmpString);

		GetPrivateProfileString("Information", "WinWidth", "-1", tmpString.GetBuffer(31), 31, iniDir);
		tmpString.ReleaseBuffer();
		nWinWidth = _ttoi(tmpString);

		GetPrivateProfileString("Information", "WinHigh", "-1", tmpString.GetBuffer(31), 31, iniDir);
		tmpString.ReleaseBuffer();
		nWinHigh = _ttoi(tmpString);

		if (nWinXPos < 0 || nWinYPos < 0 || nWinWidth <= 0 || nWinHigh <= 0)
		{
			// �����ļ������ݣ�Ĭ��λ�ã���Ļ���·��ķ�֮һ��λ��
			// ��ȡ������λ�ã�ֻ�����������ڵ׶˵����
			CRect rect;
			::GetWindowRect(::FindWindow(_T("Shell_TrayWNd"), NULL), &rect);
			// ������Ļ���·��ķ�֮һ��λ��
			cs.x = rect.right / 2;
			cs.y = rect.top / 2;
			cs.cx = rect.right / 2;
			cs.cy = rect.top / 2;
		}
		else
		{
			// �������ļ�λ����ʾ
			cs.x = nWinXPos;
			cs.y = nWinYPos;
			cs.cx = nWinWidth;
			cs.cy = nWinHigh;
		}
	}
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.hMenu = NULL;
	return TRUE;
}


void CMainFrame::InitDevice()
{
	// �����ļ���ȡ�豸������Ϣ
	// ���֤ʶ�����豸
	char sReadIDDevice[32] = {0};
	// ��������ͷ�豸
	char sDeskCameraDevice[32] = {0};
	// ��ǰ��������·��
	CString sIniFilePath;
	sIniFilePath = GetAppPath() + "\\win.ini";
	// ��ʱ�ַ���
	char sTmpString[32] = {0};
	// ���֤ʶ�����豸
	GetPrivateProfileString("Device", "ReadIDDevice", "-1", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sReadIDDevice, (const char*)sTmpString, sizeof(sReadIDDevice)-1);
	// ��������ͷ�豸
	GetPrivateProfileString("Device", "DesktopCameraDevice", "-1", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sDeskCameraDevice, (const char*)sTmpString, sizeof(sDeskCameraDevice)-1);

	GtWriteTrace(30, "%s:%d: �豸���ò�����ReadIDDevice=[%s], sDeskCameraDevice=[%s]!", __FUNCTION__
		, __LINE__, sReadIDDevice, sDeskCameraDevice);
	// �ж� ���֤ʶ�����豸���ͣ����ض�Ӧ��

	CStatic *pStatic = new CStatic();
	switch (atoi(sReadIDDevice))
	{
	case -1:
		::MessageBox(NULL, "���֤ʶ�����豸δ���ã����飡", "����", MB_OK);
		break;
	case 1:
		pBaseReadIDCardInfo = new CCentOneReader();
		break;
	case 2:
		pBaseReadIDCardInfo = new CCentReader();
		break;
	default:
		::MessageBox(NULL, "���õ����֤ʶ���ǲ����ڣ����飡", "����", MB_OK);
		break;
	}
	// �ж� ����������ͷ�豸���ͣ����ض�Ӧ��
	switch (atoi(sDeskCameraDevice))
	{
	case -1:
		::MessageBox(NULL, "����������ͷ�豸δ���ã����飡", "����", MB_OK);
		break;
	case 1:
		pBaseSaveDeskPic = new CCentOneCamera();
		break;
	case 2:
		pBaseSaveDeskPic = new CGeitCamera();
		break;
	default:
		::MessageBox(NULL, "���õĸ���������ͷ�豸�����ڣ����飡", "����", MB_OK);
		break;
	}
}


/////////////////iocp�ص�����//////////////////////////
void __stdcall AcceptEvt(
	LPVOID lpParam, HANDLE hAcceptor, PCHAR buf, DWORD len)
{
	SetLimitReadLen(g_hIoRes, hAcceptor, FALSE);
	printf("\nAccpet Event\n");
}
/*
void __stdcall ConnectEvt(
	LPVOID lpParam, HANDLE hConnector, PCHAR buf, DWORD len)
{
	printf("\nConnect Event\n");
}
*/
void __stdcall CloseEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	printf("\nClose Event\n");
/*	
	CString reStr = "OK";
	PIO_OP_KEY pOpKey = (PIO_OP_KEY)hObject;
	sockaddr_in addr;
	addr.sin_addr.s_addr = pOpKey->remote_addr;
	CString remoteAddr = inet_ntoa(addr.sin_addr);
	int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
	*/
}

// ���֤ʶ���ǻ�ȡ���֤��Ϣ��Ӧ����ת��
void ReadIDCardCodeTrans(int nRet, char *sRetCode, char *sRetMsg)
{
	char sCodeTmp[4+1] = {0};
	sprintf(sCodeTmp, "%03d", nRet);
	for (int i = 0; i < N_ID_CARD_CODE_COUNT; i++)
	{
		if (0 == strcmp(sCodeTmp, S_ID_CARD_CODE[i]))
		{
			strncpy(sRetCode, sCodeTmp, 5-1);
			strncpy(sRetMsg, S_ID_CARD_MSG[i], 128-1);
			return;
		}
	}
	strncpy(sRetCode, "299", 5-1);
	strncpy(sRetMsg, "���֤ʶ���ǻ�ȡ���֤��Ϣʧ�ܣ���������", 128-1);
}
// ����ͷ��ȡ��Ƭ��Ӧ����ת��
void ReadPicCodeTrans(int nRet, char *sRetCode, char *sRetMsg)
{
	char sCodeTmp[4+1] = {0};
	sprintf(sCodeTmp, "%03d", nRet);
	for (int i = 0; i < N_READ_PIC_CODE_COUNT; i++)
	{
		if (0 == strcmp(sCodeTmp, S_READ_PIC_CODE[i]))
		{
			strncpy(sRetCode, sCodeTmp, 5-1);
			strncpy(sRetMsg, S_READ_PIC_MSG[i], 128-1);
			return ;
		}
	}
	strncpy(sRetCode, "199", 5-1);
	strncpy(sRetMsg, "����ͷ��ȡ��Ƭ�ǰɣ���������", 128-1);
}

// ͨ�� PERSONINFO �ṹ����json����
void getIDCardInfoJson(Json::Value &jsonBuff, CString strDir, MYPERSONINFO *pPerson, int nRet)
{
	char pinyin[512] = {0};
	// ��Ч����
	char effDate[128] = {0};
	char strFlag[8] = {0};
	// ������Ϣ
	char sRetCode[4+1] = {0};
	char sRetMsg[128+1] = {0};

	if (nRet == 0)
	{
		FILE * pFile= NULL;
		char *fileBuffer = NULL;
		long lSize = 0;
		ZBase64 zBase;
		string encodeBase64_pic;
		size_t result = 0;
		char PIC_FLAG[16] = {0};
		// ��ȡͷ����
		do 
		{
			pFile = fopen (strDir.GetBuffer(), "rb");
			strDir.ReleaseBuffer();
			if (pFile == NULL)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDCardInfoJson() open [%s] failed! err=[%d][%s]", strDir.GetBuffer(), errno, strerror(errno));
				nRet = 200;
				break;
			}
			fseek (pFile, 0, SEEK_END);
			lSize = ftell(pFile);
			rewind (pFile);
			// �����ڴ�洢�����ļ�
			fileBuffer = (char*) malloc(sizeof(char) * lSize);
			if (fileBuffer == NULL)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDCardInfoJson() malloc failed! err=[%d][%s]", errno, strerror(errno));
				nRet = 202;
				// �ر��ļ�
				fclose(pFile);
				pFile = NULL;
				break;
			}
			// ���ļ�������fileBuffer��
			result = fread(fileBuffer, 1, lSize, pFile);
			if (result != lSize)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDCardInfoJson() read [%s] failed! err=[%d][%s]", strDir.GetBuffer(), errno, strerror(errno));
				nRet = 201;
				// �ر��ļ����ͷ��ڴ�
				fclose(pFile);
				pFile = NULL;
				free(fileBuffer);
				fileBuffer = NULL;
				break;
			}
			// ��ȡ���֤�������ļ��ɹ�������base64����
			encodeBase64_pic = zBase.Encode((const unsigned char*)fileBuffer, (int)lSize);
			// �ͷ��ڴ�
			free(fileBuffer);
			fileBuffer = NULL;
			fclose(pFile);
			pFile = NULL;
			nRet = 0;
		}while (0);

		// ������ת��
		ReadIDCardCodeTrans(nRet, sRetCode, sRetMsg);
		// ��ȡ����ƴ��
		MyGetPinYin(pPerson->name, pinyin);
		// ƴ���֤��Ч����
		sprintf(effDate, "%4.4s.%2.2s.%2.2s-%4.4s.%2.2s.%2.2s", pPerson->validStart, pPerson->validStart+4, pPerson->validStart+4+2,
			pPerson->validEnd, pPerson->validEnd+4, pPerson->validEnd+4+2);
		sprintf(strFlag, "%4d", pPerson->iFlag);
		jsonBuff["XYM"] = sRetCode;
		jsonBuff["XYSM"] = sRetMsg;
		jsonBuff["NAME"] = pPerson->name;
		jsonBuff["NAME_PINYIN"] = pinyin;
		jsonBuff["VERSION"] = pPerson->version;
		jsonBuff["GOV_CODE"] = pPerson->govCode;
		jsonBuff["CARD_TYPE"] = pPerson->cardType;
		jsonBuff["SEX"] = pPerson->sex;
		jsonBuff["NATION"] = pPerson->nation;
		jsonBuff["BIRTH_DAY"] = pPerson->birthday;
		jsonBuff["PAPER_ADDR"] = pPerson->address;
		jsonBuff["ID_NO"] = pPerson->cardId;
		jsonBuff["DFFECTIVE_DATE"] = effDate;
		jsonBuff["ISSUING_AUTHORITY"] = pPerson->police;
		jsonBuff["APPEND_MSG"] = pPerson->appendMsg;
		jsonBuff["FLAG"] = strFlag;
		jsonBuff["HRAD_PIC"] = encodeBase64_pic.c_str();
		jsonBuff["OTH_MSG1"] = "";
	}
	else
	{
		// ������ת��
		ReadIDCardCodeTrans(nRet, sRetCode, sRetMsg);
		jsonBuff["XYM"] = sRetCode;
		jsonBuff["XYSM"] = sRetMsg;
		jsonBuff["NAME"] = "";
		jsonBuff["NAME_PINYIN"] = "";
		jsonBuff["ENGLISH_NAME"] = "";
		jsonBuff["VERSION"] = "";
		jsonBuff["GOV_CODE"] = "";
		jsonBuff["CARD_TYPE"] = "";
		jsonBuff["SEX"] = "";
		jsonBuff["NATION"] = "";
		jsonBuff["BIRTH_DAY"] = "";
		jsonBuff["PAPER_ADDR"] = "";
		jsonBuff["ID_NO"] = "";
		jsonBuff["DFFECTIVE_DATE"] = "";
		jsonBuff["ISSUING_AUTHORITY"] = "";
		jsonBuff["APPEND_MSG"] = "";
		jsonBuff["FLAG"] = "";
		jsonBuff["HRAD_PIC"] = "";
		jsonBuff["OTH_MSG1"] = "";
	}
	return ;
}

/* ���ܣ�ͨ�����֤���������鷵�ر���
 * ��Σ�flag 0-���� 1-����
 *		strDir ��Ƭ����·��
 *		nRet ��������֮ǰ���������ͼƬ�Ƿ�ɹ�
 * ���Σ�jsonBuff�����ر��� 
*/ 
void getIDPicJson(Json::Value &jsonBuff, int flag, CString strDir, int nRet)
{
	// ��ȡ���֤������
	FILE * pFile= NULL;
	char *fileBuffer = NULL;
	long lSize = 0;
	ZBase64 zBase;
	string encodeBase64_pic;
	size_t result = 0;
	char sRetCode[4+1] = {0};
	char sRetMsg[128+1] = {0};
	char PIC_FLAG[16] = {0};
	switch (flag) 
	{
	case 0:
		strncpy(PIC_FLAG, "FRONT_PIC", sizeof(PIC_FLAG)-1);
		break;
	case 1:
		strncpy(PIC_FLAG, "BACK_PIC", sizeof(PIC_FLAG)-1);
		break;
	case 2:
		strncpy(PIC_FLAG, "LIVE_PIC", sizeof(PIC_FLAG)-1);
		break;
	}
	// �ж� �����ǻ�ȡͼƬ�Ƿ�ɹ������ɹ�����ʧ��
	if (nRet == 0)
	{
		do 
		{
			pFile = fopen (strDir.GetBuffer(strDir.GetLength()), "rb");
			strDir.ReleaseBuffer();
			if (pFile == NULL)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDPicJson() open [%s] failed! err=[%d][%s]", strDir.GetBuffer(), errno, strerror(errno));
				nRet = 101;
				jsonBuff[PIC_FLAG] = "";
				break;
			}
			fseek (pFile, 0, SEEK_END);
			lSize = ftell(pFile);
			rewind (pFile);
			// �����ڴ�洢�����ļ�
			fileBuffer = (char*) malloc(sizeof(char) * lSize);
			if (fileBuffer == NULL)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDPicJson() malloc failed! err=[%d][%s]", errno, strerror(errno));
				nRet = 103;
				jsonBuff[PIC_FLAG] = "";
				// �ر��ļ�
				fclose(pFile);
				pFile = NULL;
				break;
			}
			// ���ļ�������fileBuffer��
			result = fread(fileBuffer, 1, lSize, pFile);
			if (result != lSize)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDPicJson() read [%s] failed! err=[%d][%s]", strDir.GetBuffer(), errno, strerror(errno));
				nRet = 102;
				jsonBuff[PIC_FLAG] = "";
				// �ر��ļ����ͷ��ڴ�
				fclose(pFile);
				pFile = NULL;
				free(fileBuffer);
				fileBuffer = NULL;
				break;
			}
			// ��ȡ���֤�������ļ��ɹ�������base64����
			encodeBase64_pic = zBase.Encode((const unsigned char*)fileBuffer, (int)lSize);
			// �ͷ��ڴ�
			free(fileBuffer);
			fileBuffer = NULL;
			fclose(pFile);
			pFile = NULL;
			jsonBuff[PIC_FLAG] = encodeBase64_pic.c_str();
			//GtWriteTrace(EM_TraceDebug, "[MainFrm]file buff=[%s]", encodeBase64_pic.c_str());
		} while (0);
		// ������ת��
		ReadPicCodeTrans(nRet, sRetCode, sRetMsg);
	}
	else
	{
		// ������ת��
		ReadPicCodeTrans(nRet, sRetCode, sRetMsg);
		jsonBuff[PIC_FLAG] = "";
	}
	jsonBuff["XYM"] = sRetCode;
	jsonBuff["XYSM"] = sRetMsg;
	jsonBuff["OTH_MSG1"] = "";
	return ;
}

// ����json���ݱ���
void SendJsonMsg(Json::Value &jsonBuff, PIO_OP_KEY pOpKey)
{
	sockaddr_in addr;
	addr.sin_addr.s_addr = pOpKey->remote_addr;

	// json����ת��string��ʽ
	string msgStr_rtn_gbk = jsonBuff.toStyledString();

	// ת�� GBKToUtf8
	GtWriteTrace(EM_TraceDebug, "%s:%d: ת��ǰ����(GBK)=[%d]!", __FUNCTION__, __LINE__, msgStr_rtn_gbk.length());
	//GtWriteTrace(EM_TraceDebug, "%s:%d: ת��ǰ����(GBK)=[%s]!", __FUNCTION__, __LINE__, msgStr_rtn_gbk.c_str());
	string msgStr_rtn = MyGBKToUtf8(msgStr_rtn_gbk);
	//string msgStr_rtn = msgStr_rtn_gbk;
	GtWriteTrace(EM_TraceDebug, "%s:%d: ת��󳤶�(UTF-8)=[%d]!", __FUNCTION__, __LINE__, msgStr_rtn.length());

	// ���ͱ��ģ���������=10�ֽڱ��ĳ���+�����壻����������ƣ��ʳ�������Ҫѭ�����÷��ͺ���
	// ����ͷ���ȣ�10�ֽ�
	char sPreBuff[10+1] = {0};
	// �������ֽڳ���
	long countLen = msgStr_rtn.length();

	// ���η���������ֽڳ��ȣ�64 * 1024 socket�����(iocp.h)�����Ϊ64 * 1024�ֽ�
	const int SEND_MAX_LEN = 32 * 1024;
	// ��Ҫ���͵Ĵ���
	int count = (countLen - 1) / SEND_MAX_LEN + 1;
	// ���η��͵��ֽ���
	int sendLen = 0;
	// �����͵ı���ָ��
	char *sendBuff = (char *)msgStr_rtn.c_str();
	// ���ͺ�������ֵ
	int resCount = 0;

	sprintf(sPreBuff, "%010ld", countLen);
	// ���ͱ���ͷ
	GtWriteTrace(EM_TraceDebug, "%s:%d: ���ͱ���ͷ=[%s]!", __FUNCTION__, __LINE__, sPreBuff);
	WriteDataEx(g_hIoRes, pOpKey, NULL, 0, sPreBuff, sizeof(sPreBuff)-1);
	GtWriteTrace(EM_TraceDebug, "%s:%d: �����ͱ��ĳ���=[%d], ��%d�η���!", __FUNCTION__, __LINE__, countLen, count);

	// ѭ�����ͱ��ģ�ÿ�η���SEND_MAX_LEN�ֽ�
	for (int i = 0; i < count; i++)
	{
		// ��ȡ���η����ֽ��������һ�η��� countLen % SEND_MAX_LEN �ֽ�
		sendLen = (i == count - 1) ? countLen % SEND_MAX_LEN : SEND_MAX_LEN;
		resCount = WriteDataEx(g_hIoRes, pOpKey, NULL, 0, sendBuff + i * SEND_MAX_LEN, sendLen);
		if (resCount < 0)
		{
			GtWriteTrace(EM_TraceDebug, "%s:%d: ��%d�η��ͱ���ʧ�ܣ�����WriteDataEx()����ֵ[%d] < 0��ֹͣ���ͱ��ģ�", __FUNCTION__, __LINE__, i + 1, resCount);
			break;
		}
		if (resCount == 0)
		{
			Sleep(10);
		}
		GtWriteTrace(EM_TraceDebug, "%s:%d: ��%d�η��ͱ�����ɣ����η���[%d]�ֽ�!", __FUNCTION__, __LINE__, i + 1, sendLen);
		if (i == count - 1)
		{
			GtWriteTrace(EM_TraceDebug, "%s:%d: ����ȫ��������ɣ���[%d]�ֽ�!", __FUNCTION__, __LINE__, countLen);
		}
	}
	return ;
}

CString RetMsg(string xym,string xynr)
{
	CString ret_str="";
	Json::Value msgStr_json_rtn;//��ʾһ��json��ʽ�Ķ��� 
	std::string msgStr_rtn;
	msgStr_json_rtn["XYM"]=xym.c_str();
	msgStr_json_rtn["XYSM"]=xynr.c_str();
	msgStr_rtn=msgStr_json_rtn.toStyledString();
	GtWriteTrace(EM_TraceDebug,"%s",msgStr_rtn.c_str());
	ret_str = msgStr_rtn.c_str();
	return ret_str;
}

CString Json_010101_SendMsg(Json::Value &value)
{
	Json::Reader reader;//json����
	std::string out="";
	sendMsg+=out.c_str();
	CString reStr="";

	//GtWriteTrace(EM_TraceDebug,"%s",sendMsg.GetBuffer());
	out=value["HM"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//����Ϊ��
		reStr=RetMsg("02","�����Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+='\n';
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["ZJLX"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//֤������Ϊ��
		reStr=RetMsg("03","֤�������Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["ZJHM"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//֤������Ϊ��
		reStr=RetMsg("04","֤�������Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+="\n";
		sendMsg+="\n";
		sendMsg+=out.c_str();
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["XM"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//����Ϊ��
		reStr=RetMsg("05","�����Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["ZJSFCQYX"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//֤���Ƿ�����ЧΪ��
		reStr=RetMsg("06","֤���Ƿ�����Ч�Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		if(out=="2")
		{
			out=value["ZJDQR"].asString();
			if(out.empty()!=0) //true 1 false 0
			{
				//֤��������Ϊ��
				reStr=RetMsg("14","֤���������Ǳ��������Ϊ��");
				//д��־
				return reStr;
			}
			else
			{
				sendMsg+=out.c_str();
			}
		}
	}
			
	out=value["FZJGDQDM"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//��֤���ص�������Ϊ��
		reStr=RetMsg("07","��֤���ص��������Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["XB"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�Ա�Ϊ��
		reStr=RetMsg("08","�Ա��Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["GJ"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//����Ϊ��
		reStr=RetMsg("09","�����Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["GDDH"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		out=value["YDDH"].asString();
		if(out.empty()!=0) //true 1 false 0
		{
			//�ƶ��绰Ϊ��
			reStr=RetMsg("10","�̶��绰���ƶ��绰��������һ��");
			//д��־
			return reStr;
		}
		else
		{
			sendMsg+="\n";
			sendMsg+=out.c_str();
			sendMsg+="\n";
			//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		}
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["YDDH"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�̶��绰��Ϊ�գ��ƶ��绰Ϊ��
		sendMsg+="\n";
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["TXDZ"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//ͨѶ��ַΪ��
		reStr=RetMsg("11","ͨѶ��ַ�Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["YZBM"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�������벻�Ǳ�����
		sendMsg+="\n";
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["ZY"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//ְҵΪ��
		reStr=RetMsg("12","ְҵ�Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["JJLXR"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//������
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		/*out=value["JJLXDH"].asString();
		if(out.empty()!=0) //true 1 false 0
		{
			//������������ϵ���У�������ϵ�绰һ�����У�������ϵ��û�У�������ϵ�绰�Ͳ�������
			//֤���Ƿ�����ЧΪ��
			reStr=RetMsg("13","���ڽ�����ϵ�ˣ�������ϵ�绰���Ǳ��������Ϊ��");
			//д��־
			return reStr;
		}
		else
		{
			sendMsg+=out.c_str();
			//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		}*/
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		out=value["JJLXDH"].asString();
		if(out.empty()!=0) //true 1 false 0
		{
			//����������ϵ���У�������ϵ�绰һ������
			//֤���Ƿ�����ЧΪ��
			reStr=RetMsg("13","���ڽ�����ϵ�ˣ�������ϵ�绰���Ǳ��������Ϊ��");
			//д��־
			return reStr;
		}
		else
		{
			sendMsg+=out.c_str();
			//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		}
	}
	reStr=RetMsg("000","���ͳɹ�");
	return reStr;
}
CString Json_101004_SendMsg(Json::Value &value)
{
	Json::Reader reader;//json����
	std::string out="";
	sendMsg+=out.c_str();
	CString reStr="";	

	out=value["ZFZFF"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�ʷ�֧����Ϊ��
		reStr=RetMsg("014","�ʷ�֧�����Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["SKRXM"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�տ�������Ϊ��
		reStr=RetMsg("015","�տ��������Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+='\n';
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["SKRZKH"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�տ����˺�/����Ϊ��
		reStr=RetMsg("016","�տ����˺�/�����Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+='\n';
		sendMsg+=out.c_str();
		sendMsg+='\n';
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["HKJE"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�����Ϊ��
		reStr=RetMsg("017","������Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+='\n';
		sendMsg+=out.c_str();
		sendMsg+='\n';
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["ZZLX"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//ת������Ϊ��
		reStr=RetMsg("018","ת�������Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+='\n';
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["HKRZKHBZ"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//������˺�/���ű�־Ϊ��
		reStr=RetMsg("020","������˺�/���ű�־�Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		if(0==strcmp(out.c_str(),"C_BZ"))
		{
			//���͵�һ������
			GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		}
		
	}
	out=value["ZHMMBZ"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�˻������־Ϊ��
		reStr=RetMsg("021","�˻������־�Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		if(0==strcmp(out.c_str(),"MM_BZ"))
		{
			//
			sendMsg+='&';
			GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		}
		
	}
	out=value["HKRZJLX"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�����֤������Ϊ��
		reStr=RetMsg("022","�����֤�������Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+='\n';
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	out=value["HKRZJHM"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//�����֤������Ϊ��
		reStr=RetMsg("023","�����֤�������Ǳ��������Ϊ��");
		//д��־
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+='\n';
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	reStr=RetMsg("000","���ͳɹ�");
	return reStr;
}

int SendToWindows()
{
	// �Զ�����׽��д���
		std::string msgStr_rtn;

		GtWriteTrace(EM_TraceDebug,"sendMsg:[%s]",sendMsg);
		BOOL bRes = FALSE;

		int iPos = sendMsg.Find('&');
		CString strTmp="";
		if(iPos!=-1)
		{
			strTmp = sendMsg.Left(iPos);
			GtWriteTrace(EM_TraceDebug,"strTmp:[%s]",strTmp);
			GtWriteTrace(EM_TraceDebug,"sendMsg:[%s]",sendMsg);
			sendMsg = sendMsg.Mid(iPos + 1);
		}
		else
			strTmp=sendMsg;
		int strlen = strTmp.GetLength();
		CString tmp;
		tmp.Format("wname.GetBuffer()=[%s], sendMsg=[%s]", wname.GetBuffer(), sendMsg.GetBuffer());
		//::MessageBoxA(NULL, tmp, "test", MB_OK);
		//��ȡ���ھ��
		if(wname.GetBuffer() == "")
		{
			::MessageBox(NULL,"Ŀ�괰��δ����","��ʾ",MB_OK);
			return -1;
		}

		hWnd = ::FindWindow(NULL,wname.GetBuffer());
		if(NULL==hWnd){
			::MessageBox(NULL,"û���ҵ�����","��ʾ",MB_OK);
			return -1;
		}

		//Ŀ�괰���߳��뵱ǰ�̹߳���
		DWORD curtid = GetCurrentThreadId();
		DWORD tid = GetWindowThreadProcessId(hWnd, NULL);
		DWORD error = 0;
	/**���������̣߳�ʹĿ���߳��뵱ǰ�̹߳�����Ϣ���У��ⲽ�ܹؼ���
		�ɹ��������Ŀ���̴߳���Ϊ����ڣ�����ȡ����λ�õľ��.*/
		hCurFocus = NULL;
		//Ŀ�괰���ö�
		::SetWindowPos(hWnd,HWND_TOPMOST,20,20,820,420,SWP_SHOWWINDOW);
		::ShowWindow(hWnd,SW_SHOWMAXIMIZED);
		if(AttachThreadInput(tid,curtid , true))
		{
		//	HWND tmpHWnd = ::SetActiveWindow(hWnd);	
			bRes = ::SetForegroundWindow(hWnd);
			hCurFocus = ::GetFocus();
		}
		else 
		{
			error = GetLastError();
			CString errStr = "�����߳�ʧ��" + error;
			::MessageBox(NULL,errStr,"��ʾ",MB_OK);
			return -1;
		}

	
		//Ŀ�괰���ö�
		//::SetWindowPos(hWnd,HWND_TOPMOST,20,20,820,420,SWP_SHOWWINDOW);
		//���Ϳ��߳���Ϣ
		byte temp[2] = {0};
		LRESULT res = 0;
		if(hCurFocus)
		{
			for(int i=0; i<strlen; )
			{
				temp[0] = strTmp.GetBuffer()[i];
				if(temp[0] < 128)
				{
					res = ::PostMessage(hCurFocus, WM_CHAR, temp[0], 0);
					if(temp[0]==10)
					{
						Sleep(long_time_interval);
					}
					else
					{
						Sleep(short_time_interval);
					}
				}
				else
				{
					temp[1] = strTmp.GetBuffer()[i+1];
					::PostMessage(hCurFocus, WM_CHAR, temp[0], 0);
					::PostMessage(hCurFocus, WM_CHAR, temp[1], 0);
					i++;
				}
				i++;
			}
		}

		//�����̹߳���
		if(!AttachThreadInput(tid, curtid, false))
		{
			::MessageBox(NULL,"ȡ���̹߳���ʧ�ܣ�","��ʾ",MB_OK);
			return -1;
		}
		//ȡ���ö�
		::SetWindowPos(hWnd,HWND_NOTOPMOST,100,100,900,500,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
	//	::SetWindowPos(hWnd,HWND_NOTOPMOST,100,100,900,500,SWP_SHOWWINDOW);
		return 0;
}

/*
	JSONת���ɷ��͸�������ϵͳ���ַ�������
	CString JsonToSendMsg(string str,CString &sendMsg)
	���������
		str:�յ��ı���
		sendMsg:���ص��ַ�������
	���������
		���ص�CString���͵�jsonӦ����
*/
CString JsonToSendMsg(string str)
{
	Json::Reader reader;//json����
	Json::Value jValue;//��ʾһ��json��ʽ�Ķ���
	std::string out="";
	CString reStr="";
	if(reader.parse(str,jValue))//������json�ŵ�json����
	{
		out=jValue["JYDM"].asString();
		GtWriteTrace(EM_TraceDebug,"%s",out.c_str());
		//���ڿ���
		if(0==strcmp(out.c_str(),"010101"))
		{
			sendMsg="";
			sendMsg+=out.c_str();
			reStr=Json_010101_SendMsg(jValue);
			return reStr;
		}
		//�����˻����˻����
		if(0==strcmp(out.c_str(),"101004"))
		{
			sendMsg="";
			sendMsg+=out.c_str();
			reStr=Json_101004_SendMsg(jValue);
			
			return reStr;
		}
		else
		{
			//δ֪������
			reStr=RetMsg("01","δ֪������");
			return reStr;
		}		
	}
	else
	{
		GtWriteTrace(EM_TraceDebug,"error = [%s]", (reader.getFormatedErrorMessages()).c_str());
		//����ʧ��
		reStr=RetMsg("99","����ʧ��");
		return reStr;
	}
}
/**************************************************************************
*��������void __stdcall ReadEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
*����˵����socket�Ķ����ݻص�����
*����˵����
*buf:���յ�������
*len�����յ����ݳ���
*��������ֵ��DWORD
***************************************************************************/
void __stdcall ReadEvt(
	LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	GtWriteTrace(EM_TraceDebug, "--->�յ�������...");
	PIO_OP_KEY pOpKey = (PIO_OP_KEY)hObject;
	buf[len] = '\0';
	CString msgStr = buf;
	//////
	ZBase64 zBase;
	int msg_base64_org_len = strlen(msgStr.GetBuffer());
	int msg_base64_after_len = 0;
	string msg_base64_after = zBase.Decode(msgStr.GetBuffer(),msg_base64_org_len,msg_base64_after_len);
	msgStr = msg_base64_after.c_str();
	ConvertUtf8ToGBK(msgStr);
	GtWriteTrace(EM_TraceDebug,"�����%s",msgStr.GetBuffer());

	/////
	//CMainFrame * p_CMainFrame = dynamic_cast<CMainFrame *>((CMainFrame *) lpParam);
	//	::MessageBox(NULL,msgStr,"��ʾ",MB_OK);
	//	std::string strValue="{\"KEY\":\"value666\",\"ARRAY\":[{\"KEY2\":\"value2\"},{\"KEY2\":\"value3\"},{\"KEY2\":\"value4\"}]}"; 
	std::string strValue=msgStr.GetBuffer();
	Json::Reader reader;//json����
	Json::Value msgStr_json_rtn;//��ʾһ��json��ʽ�Ķ��� 
	Json::Value value;//��ʾһ��json��ʽ�Ķ���
	std::string msgStr_rtn;
	std::string out;
	std::string tran_type;
	CString reStr="";
    Json::Reader ret_reader;//json����
	std::string ret_out;
	GtWriteTrace(EM_TraceDebug,"Get message! strValue:%s",strValue.c_str());
	if(reader.parse(strValue,value))//������json�ŵ�json����
	{
		tran_type = value["BWKZLX"].asString();
		if (0 == tran_type.compare("0"))
		{
			//��ȡ�����ļ�
			GetPrivateProfileString("Information","Wname","test.txt - ���±�",wname.GetBuffer(100),100,GetAppPath()+"\\win.ini");
			long_time_interval=GetPrivateProfileInt("Information","DefaultInterval",10,GetAppPath()+"\\win.ini");
			short_time_interval=GetPrivateProfileInt("Information","EnterInputInterval",10,GetAppPath()+"\\win.ini");
			// �Զ������
			reStr=JsonToSendMsg(strValue);
			if(ret_reader.parse(reStr.GetBuffer(),msgStr_json_rtn))//������json�ŵ�json����
			{
				ret_out=msgStr_json_rtn["XYM"].asString();
				GtWriteTrace(EM_TraceDebug,"%s",ret_out.c_str());
				//���ڿ���
				if(0!=strcmp(ret_out.c_str(),"000"))
				{
					GtWriteTrace(EM_TraceDebug,"XYM[%s]",ret_out.c_str());
					sockaddr_in addr;
					addr.sin_addr.s_addr = pOpKey->remote_addr;
					int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
					//д��־
					return;
				}
			}

			SendToWindows();

			sockaddr_in addr;
			addr.sin_addr.s_addr = pOpKey->remote_addr;
			int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
			//д��־
			return;
		}
		else if (0 == tran_type.compare("1"))
		{
			// ��ȡ���֤оƬ��Ϣ
			int nRet = 0;
			MYPERSONINFO pMyPerson;
			memset(&pMyPerson, 0, sizeof(PERSONINFO));
			CString str = GetAppPath();
			str.Append("\\IDPicture\\HeadPictureTmp.jpg");
			nRet = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pBaseReadIDCardInfo->MyReadIDCardInfo(str.GetBuffer(), &pMyPerson);
// 			nRet = 0;
// 			memcpy(pMyPerson.address, "����ʡ�������Ͼ���������13-1��", sizeof(pMyPerson.address));
// 			memcpy(pMyPerson.appendMsg, "", sizeof(pMyPerson.appendMsg));
// 			memcpy(pMyPerson.birthday, "19941022", sizeof(pMyPerson.birthday));
// 			memcpy(pMyPerson.cardId, "350425199410220517", sizeof(pMyPerson.cardId));
// 			memcpy(pMyPerson.cardType, "", sizeof(pMyPerson.cardType));
// 			memcpy(pMyPerson.EngName, "", sizeof(pMyPerson.EngName));
// 			memcpy(pMyPerson.govCode, "", sizeof(pMyPerson.govCode));
// 			pMyPerson.iFlag = 0;
// 			memcpy(pMyPerson.name, "Ҷ����", sizeof(pMyPerson.name));
// 			memcpy(pMyPerson.nation, "��", sizeof(pMyPerson.nation));
// 			memcpy(pMyPerson.nationCode, "", sizeof(pMyPerson.nationCode));
// 			memcpy(pMyPerson.otherData, "", sizeof(pMyPerson.otherData));
// 			memcpy(pMyPerson.police, "�����ع�����", sizeof(pMyPerson.police));
// 			memcpy(pMyPerson.sex, "��", sizeof(pMyPerson.sex));
// 			memcpy(pMyPerson.sexCode, "", sizeof(pMyPerson.sexCode));
// 			memcpy(pMyPerson.validEnd, "20201221", sizeof(pMyPerson.validEnd));
// 			memcpy(pMyPerson.validStart, "20101221", sizeof(pMyPerson.validStart));
// 			memcpy(pMyPerson.version, "", sizeof(pMyPerson.version));
			// ͨ��������Ϣͷ��·���ͷ���ֵƴjson����
			getIDCardInfoJson(msgStr_json_rtn, str, &pMyPerson, nRet);
			// ��json���ķ���
			SendJsonMsg(msgStr_json_rtn, pOpKey);
		}
		else if (0 == tran_type.compare("2"))
		{
			// ��ȡ���֤������Ϣ
			int nRet = 0;
			CString str = GetAppPath();
			str.Append("\\IDPicture\\FrontPictureTmp.jpg");
			nRet = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pBaseSaveDeskPic->MySaveDeskIDPic(str.GetBuffer());
			//nRet = 0;
			GtWriteTrace(EM_TraceDebug, "[MainFrm]Save file [%s] return = [%d]", str.GetBuffer(), nRet);
			// ͨ�����֤������Ϣ����ֵƴjson����
			getIDPicJson(msgStr_json_rtn, 0, str, nRet);
			// ��json���ķ���
			SendJsonMsg(msgStr_json_rtn, pOpKey);
		}
		else if (0 == tran_type.compare("3"))
		{
			// ��ȡ���֤������Ϣ
			int nRet = 0;
			CString str = GetAppPath();
			str.Append("\\IDPicture\\BackPictureTmp.jpg");
			nRet = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pBaseSaveDeskPic->MySaveDeskIDPic(str.GetBuffer());
			//nRet = 0;
			GtWriteTrace(EM_TraceDebug, "[MainFrm]Save file [%s] return = [%d]", str.GetBuffer(), nRet);
			// ͨ�����֤������Ϣ����ֵƴjson����
			getIDPicJson(msgStr_json_rtn, 1, str, nRet);
			// ��json���ķ���
			SendJsonMsg(msgStr_json_rtn, pOpKey);
		}
		else if (0 == tran_type.compare("4"))
		{
			// ��ȡ ��������ͷ������
			int nRet = 0;
			CString str = GetAppPath();
			str.Append("\\IDPicture\\EnvPictureTmp.jpg");
			nRet = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pBaseSaveDeskPic->MySaveEnvPic(str.GetBuffer());
			//nRet = 0;
			GtWriteTrace(EM_TraceDebug, "[MainFrm]Save file [%s] return = [%d]", str.GetBuffer(), nRet);
			// ͨ�� ��������ͷ��Ϣ����ֵƴjson����
			getIDPicJson(msgStr_json_rtn, 2, str, nRet);
			// ��json���ķ���
			SendJsonMsg(msgStr_json_rtn, pOpKey);
		}
		else
		{
			//δ֪��������
			GtWriteTrace(EM_TraceDebug, "[MainFrm]Unknown tran type! tran_type = [%s]", tran_type.c_str());
			msgStr_json_rtn["XYM"]="998";
			msgStr_json_rtn["XYSM"]="δ֪�ı��Ŀ�������";
			msgStr_rtn=msgStr_json_rtn.toStyledString();
			reStr = msgStr_rtn.c_str();
			sockaddr_in addr;
			addr.sin_addr.s_addr = pOpKey->remote_addr;
			//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
			int resCount = WriteDataEx(g_hIoRes, pOpKey,NULL, 0, reStr.GetBuffer(), reStr.GetLength());
			//д��־
			GtWriteTrace(EM_TraceDebug, "%s", msgStr_rtn.c_str());
			return;
		}
	}
	else
	{
		GtWriteTrace(EM_TraceDebug,"Read json failed! error = [%s]", (reader.getFormatedErrorMessages()).c_str());
		//����ʧ��
		msgStr_json_rtn["XYM"]="999";
		msgStr_json_rtn["XYSM"]="json���Ľ���ʧ��";
		msgStr_rtn=msgStr_json_rtn.toStyledString();
		GtWriteTrace(EM_TraceDebug,"%s",msgStr_rtn.c_str());
		CString reStr = msgStr_rtn.c_str();
		sockaddr_in addr;
		addr.sin_addr.s_addr = pOpKey->remote_addr;
		int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
		//д��־
		return;
	}
#if 0
	::MessageBox(NULL,msgStr,"��ʾ",MB_OK);
	char msgStr_all[4096] = {0};
	char msg_type[2+1] = {0};
	char msg_controltype[2+1] = {0};
	char msg_id[8+1] = {0};
	char msg_body[4096] = {0};
	char msgStr_rtn[1024] = {0};			//�ظ���Ϣ
	strncpy(msgStr_all,msgStr.GetBuffer(),msgStr.GetLength());
	//д��־
	GtWriteTrace(EM_TraceDebug,"�����%s",msgStr_all);
	//GtWriteTrace(EM_TraceDebug,msgStr_all);

	if( msgStr.GetLength()<=12 )
	{
		sprintf(msgStr_rtn,"02000000000099");
		CString reStr = msgStr_rtn;
		sockaddr_in addr;
		addr.sin_addr.s_addr = pOpKey->remote_addr;
		//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
		int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
		//д��־
		return;
	}
	strncpy(msg_type,msgStr_all,2);//��������
	strncpy(msg_controltype,msgStr_all+2,2);//��������
	strncpy(msg_id,msgStr_all+4,8);//����id
	strncpy(msg_body,msgStr_all+12,strlen(msgStr_all)-12);//��������
	if(strncmp(msg_type,"01",2) == 0)
	{
		//������
		if(strncmp(msg_controltype,"01",2) == 0)//��ʾ�û���Ϣ
		{
			//д��־

			//
			memset(msgStr.GetBuffer(),0x00,msgStr.GetLength()+1);
			strncpy(msgStr.GetBuffer(),msg_body,strlen(msgStr_all)-12);		
		}
		else if(strncmp(msg_controltype,"02",2) == 0)//��ʾ��ҳ����
		{
			//��ʾ��ҳ(����Ϣ��ָ������)
			::SendMessageA(((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView()->GetSafeHwnd(),WM_HTML_SHOW,(WPARAM)msg_body,NULL);
			//д��־
			//��Ӧ���
			sprintf(msgStr_rtn,"02%s%s00",msg_controltype,msg_id);
			CString reStr = msgStr_rtn;
			sockaddr_in addr;
			addr.sin_addr.s_addr = pOpKey->remote_addr;
			//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
			int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
			return;
		}
		else
		{
			//����Ŀ�������
			sprintf(msgStr_rtn,"02%s%s99",msg_controltype,msg_id);
			CString reStr = msgStr_rtn;
			sockaddr_in addr;
			addr.sin_addr.s_addr = pOpKey->remote_addr;
			//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
			int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
			//д��־
			return;
		}
	}
	else
	{
		//����ı�������
		sprintf(msgStr_rtn,"02%s%s99",msg_controltype,msg_id);
		CString reStr = msgStr_rtn;
		sockaddr_in addr;
		addr.sin_addr.s_addr = pOpKey->remote_addr;
		//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
		int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
		//д��־
		return;
	}
#endif
#if 0
	if (tran_type.compare("0") == 0)
	{
		// �Զ�����׽��д���
		//::MessageBox(NULL,msgStr,"��ʾ",MB_OK);
		Json::Value msgStr_json_rtn;//��ʾһ��json��ʽ�Ķ��� 

		std::string msgStr_rtn;
		Json::Reader ret_reader;//json����
		Json::Value ret_value;//��ʾһ��json��ʽ�Ķ���
		std::string ret_out;
		if(ret_reader.parse(reStr.GetBuffer(),ret_value))//������json�ŵ�json����
		{
			ret_out=ret_value["XYM"].asString();
			GtWriteTrace(EM_TraceDebug,"%s",ret_out.c_str());
			//���ڿ���
			if(0!=strcmp(ret_out.c_str(),"000"))
			{
				GtWriteTrace(EM_TraceDebug,"XYM[%s]",ret_out.c_str());
				sockaddr_in addr;
				addr.sin_addr.s_addr = pOpKey->remote_addr;
				int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
				//д��־
				return;
			}
		}
		GtWriteTrace(EM_TraceDebug,"sendMsg:[%s]",sendMsg);
		//::MessageBox(NULL,sendMsg,"��ʾ",MB_OK);
		////////////////////
		BOOL bRes = FALSE;
		int strlen = sendMsg.GetLength();
		//��ȡ�����ļ�
		GetPrivateProfileString("Information","Wname","test.txt - ���±�",wname.GetBuffer(100),100,GetAppPath()+"\\win.ini");
		int long_time_interval,short_time_interval;
		long_time_interval=GetPrivateProfileInt("Information","DefaultInterval",10,GetAppPath()+"\\win.ini");
		short_time_interval=GetPrivateProfileInt("Information","EnterInputInterval",10,GetAppPath()+"\\win.ini");
		//��ȡ���ھ��
		HWND hWnd = ::FindWindow(NULL,wname.GetBuffer());
		if(NULL==hWnd){
			::MessageBox(NULL,"û���ҵ�����","��ʾ",MB_OK);
		}

		//Ŀ�괰���߳��뵱ǰ�̹߳���
		DWORD curtid = GetCurrentThreadId();
		DWORD tid = GetWindowThreadProcessId(hWnd, NULL);
		DWORD error = 0;
	/**���������̣߳�ʹĿ���߳��뵱ǰ�̹߳�����Ϣ���У��ⲽ�ܹؼ���
		�ɹ��������Ŀ���̴߳���Ϊ����ڣ�����ȡ����λ�õľ��.*/
		HWND hCurFocus = NULL;
		//Ŀ�괰���ö�
		::SetWindowPos(hWnd,HWND_TOPMOST,20,20,820,420,SWP_SHOWWINDOW);
		::ShowWindow(hWnd,SW_SHOWMAXIMIZED);
		if(AttachThreadInput(tid,curtid , true))
		{
		//	HWND tmpHWnd = ::SetActiveWindow(hWnd);	
			bRes = ::SetForegroundWindow(hWnd);
			hCurFocus = ::GetFocus();
		}
		else 
		{
			error = GetLastError();
			CString errStr = "�����߳�ʧ��" + error;
			::MessageBox(NULL,errStr,"��ʾ",MB_OK);
		}

	
		//Ŀ�괰���ö�
		//::SetWindowPos(hWnd,HWND_TOPMOST,20,20,820,420,SWP_SHOWWINDOW);
		//���Ϳ��߳���Ϣ
		byte temp[2] = {0};
		LRESULT res = 0;
		if(hCurFocus)
		{
			for(int i=0; i<strlen; )
			{
				temp[0] = sendMsg.GetBuffer()[i];
				if(temp[0] < 128)
				{
					res = ::PostMessage(hCurFocus, WM_CHAR, temp[0], 0);
					if(temp[0]==10)
					{
						Sleep(long_time_interval);
					}
					else
					{
						Sleep(short_time_interval);
					}
				}
				else
				{
					temp[1] = sendMsg.GetBuffer()[i+1];
					::PostMessage(hCurFocus, WM_CHAR, temp[0], 0);
					::PostMessage(hCurFocus, WM_CHAR, temp[1], 0);
					i++;
				}
				i++;
			}
		}

		//�����̹߳���
		if(!AttachThreadInput(tid, curtid, false))
		{
			::MessageBox(NULL,"ȡ���̹߳���ʧ�ܣ�","��ʾ",MB_OK);
		}
		//ȡ���ö�
		::SetWindowPos(hWnd,HWND_NOTOPMOST,100,100,900,500,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
	//	::SetWindowPos(hWnd,HWND_NOTOPMOST,100,100,900,500,SWP_SHOWWINDOW);
		//sprintf(msgStr_rtn,"02%s%s00",msg_controltype,msg_id);
		//strncpy(msgStr_rtn,"00",2);
		//δ֪������
		/*msgStr_json_rtn["XYM"]="00";
		msgStr_rtn=msgStr_json_rtn.toStyledString();
		GtWriteTrace(EM_TraceDebug,"%s",msgStr_rtn.c_str());
		CString reStr = msgStr_rtn.c_str();*/
		sockaddr_in addr;
		addr.sin_addr.s_addr = pOpKey->remote_addr;
		//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
		int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
	//	int resCount = send(pOpKey->s,reStr.GetBuffer(),2,0);
	}
#endif
	GtWriteTrace(EM_TraceDebug, "<---�����������\n");
}

void __stdcall WriteEvt(
	LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	printf("\nWrite Event\n");
}

void __stdcall ExceptEvt(
	LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	printf("\nExcept Event\n");
}


/////////////////iocp�ص�����/////////////////////////

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//���������ص����½�
	memset(&m_nid,0,sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.hWnd = this->m_hWnd;
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	m_nid.uCallbackMessage = WM_TRAYNOTIFY;
	CString strToolTip = "��̨����";
	strcpy(m_nid.szTip,strToolTip.GetBuffer());
	m_nid.uID = IDR_MAINFRAME;
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nid.hIcon = hIcon;
	Shell_NotifyIcon(NIM_ADD,&m_nid);
	if(hIcon)
		::DestroyIcon(hIcon);
	BOOL flag = m_menu.LoadMenu(IDR_POPMENU); 
	//��ʼ����������
//	m_ScreenshotDlg = new CScreenshotDlg;
//	m_ScreenshotDlg->Create(IDD_SCREENSHOT);
//	m_ScreenshotDlg->ShowWindow(SW_HIDE);

	//����������־����ʼΪFALSEδ����״̬
	m_hookFlag = FALSE;
	// ��������
	StartKeyBoardHook();

	//��ʼ����������
	g_hIoRes = CreateIoResource(0, 100, 80, 0);
	SetGIoRes(g_hIoRes);
	HANDLE hObject = CreateTcpListener(
			g_hIoRes,
			NULL,
			9190,
			5,
			AcceptEvt,
			CloseEvt,
			ReadEvt,
			WriteEvt,
			ExceptEvt,
			this
			);
	if (hObject)
	{
		StartListenerAccept(g_hIoRes, hObject, 5);
	}

	return 0;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

// CMainFrame ��Ϣ�������
LRESULT CMainFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case WM_RBUTTONDOWN:              // �Ҽ�����ʱ�����˵�  
		{  
			CMenu *pMenu = m_menu.GetSubMenu(0);
			if(pMenu){
				LPPOINT lpoint = new tagPOINT;  
				::GetCursorPos(lpoint);                    // �õ����λ��  	
				SetForegroundWindow();
				pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, lpoint->x ,lpoint->y, AfxGetMainWnd());   
				delete lpoint;
			}	  
			break;  
		}  
	case WM_LBUTTONDBLCLK:                            // ˫������Ĵ���      
		if (AfxGetApp()->m_pMainWnd->IsZoomed())
		{
			// ��󻯣��ָ����
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		}
		else if (AfxGetApp()->m_pMainWnd->IsIconic())
		{
			// ��С�����ָ�ԭ��״̬
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNOACTIVATE);
		}
		else
		{
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNORMAL);
		}
		break;  
	}
	return 0;
}

void CMainFrame::ActivateFrame(int nCmdShow)
{
	// TODO: �ڴ����ר�ô����/����û���
	nCmdShow = SW_HIDE;
	CFrameWnd::ActivateFrame(nCmdShow);
}


void CMainFrame::OnQuit()
{
	// �˳����򣬱��洰��λ�õ������ļ���
	SaveFrmPosToFile();
	Shell_NotifyIcon(NIM_DELETE,&m_nid);
	if (screenshotDlg != NULL)
	{
		delete screenshotDlg;
		screenshotDlg = NULL;
	}
	if (input != NULL)
	{
		delete input;
		input = NULL;
	}
	if (settingDlg != NULL)
	{
		delete settingDlg;
		settingDlg = NULL;
	}
	DestroyWindow();
}

// ���ù���
void CMainFrame::StartKeyBoardHook()
{
	if(!m_hookFlag)
	{
		// �����ļ���ȡ��ǰ�ȼ�
		char sHotKeyValue[32] = {0};
		// �ȼ�ֵ
		GetPrivateProfileString("Information", "HotKeyValue", "3|162|164|83|0|", sHotKeyValue, sizeof(sHotKeyValue)-1, GetAppPath()+"\\win.ini");

		char tmpKey[32] = {0};
		//���ÿ�ݼ�
		COMBKEY combKey[4];
		HWND myHwnd[4];
		UINT myMsgType[4];
		memset(combKey, 0, sizeof(COMBKEY) * 4);
		memset(myHwnd, 0, sizeof(HWND) * 4);
		memset(myMsgType, 0, sizeof(UINT) * 4);
		/*
		combKey.count = atoi(tmpKey);
		combKey.keys[0] = VK_LCONTROL;
		combKey.keys[1] = VK_LMENU;
		combKey.keys[2] = 83;  //S��
		*/
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 0);
		combKey[0].count = atoi(tmpKey);
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 1);
		combKey[0].keys[0] = atoi(tmpKey);
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 2);
		combKey[0].keys[1] = atoi(tmpKey);
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 3);
		combKey[0].keys[2] = atoi(tmpKey);
		memset(tmpKey, 0, sizeof(tmpKey));
		splitString(tmpKey, sHotKeyValue, 4);
		combKey[0].keys[3] = atoi(tmpKey);
		myHwnd[0] = m_hWnd;
		myMsgType[0] = WM_SCREENSHOT;

		combKey[1].count = 2;
		combKey[1].keys[0] = VK_LCONTROL;
		combKey[1].keys[1] = 80;
		combKey[1].keys[2] = 0;
		combKey[1].keys[3] = 0;
		myHwnd[1] = m_hWnd;
		myMsgType[1] = WM_CONTINUE_INPUT;


		//����֪ͨ���ھ����֪ͨ��Ϣ����
		SetHWndAndMsgType(myHwnd, myMsgType);
		// ���ÿ�ݼ�
		SetCombKey(combKey);
		//����ȫ�ּ��̹���
		SetGHook_KEYBOARD();
	}
	else
	{
		FreeGHook_KEYBOARD();
	}
}


void CMainFrame::OnSettingWin()
{
	if (settingDlg == NULL)
	{
		settingDlg = new CSetting();
		settingDlg->Create(IDD_SETTING, this);
		settingDlg->ShowWindow(SW_SHOWNA);
	}
	else
	{
		settingDlg->MyInit();
		settingDlg->ShowWindow(SW_SHOWNA);
	}
}

void CMainFrame::OnMyClose()
{
	//MessageBox("OnMyClose!");
	SaveFrmPosToFile();
	this->ShowWindow(SW_HIDE);
}


// ����������λ�õ������ļ���
BOOL CMainFrame::SaveFrmPosToFile()
{
	// �����ļ�·��
	CString iniDir = GetAppPath()+"\\win.ini";
	if (IsZoomed())
	{
		// ��󻯣�����״̬�������ļ���
		// ���״̬��1-��
		WritePrivateProfileString("Information", "IsZoomed", "1", iniDir);
	}
	else if (IsIconic())
	{
		// ��С������������Ϊ�ڵ����С��֮ǰ�Ѿ��������
	}
	else
	{
		// ����󻯣����洰��λ�õ������ļ���
		RECT rect = {0};
		CString tmp;
		// ��ȡ��ǰ����λ��
		::GetWindowRect(this->m_hWnd, &rect);
		tmp.Format("%d", rect.left);
		WritePrivateProfileString("Information", "WinXPos", tmp, iniDir);
		tmp.Format("%d", rect.top);
		WritePrivateProfileString("Information", "WinYPos", tmp, iniDir);
		tmp.Format("%d", rect.right - rect.left);
		WritePrivateProfileString("Information", "WinWidth", tmp, iniDir);
		tmp.Format("%d", rect.bottom - rect.top);
		WritePrivateProfileString("Information", "WinHigh", tmp, iniDir);
		// ���״̬��0-��
		WritePrivateProfileString("Information", "IsZoomed", "0", iniDir);
	}
	return TRUE;
}

// ������ͼ
LRESULT CMainFrame::OnScreenshot(WPARAM wParam, LPARAM lParam)
{
//	MessageBox("�յ�������Ϣ!");
//	m_ScreenshotDlg->ShowWindow(SW_SHOW);
	
	if (screenshotDlg == NULL)
	{
		screenshotDlg = new CScreenshotDlg();
		screenshotDlg->DoModal();
		delete screenshotDlg;
		screenshotDlg = NULL;
	}
	else
	{
		screenshotDlg->ShowWindow(SW_SHOWNORMAL);
	}
	
	return 0;
}


// ��ʾ��ά����ˮ�ŶԻ���
void CMainFrame::ShowManualInput(char *tip)
{
	if (input == NULL)
	{
		input = new CManualInput(tip, this);
		input->Create(IDD_MANUALINPUT, this);
		input->ShowWindow(SW_SHOWNA);
	}
	else
	{
		input->SetTip(tip);
		input->ResetEdit();
		input->ShowWindow(SW_SHOWNA);
	}
}


// ���̲˵�->�ֶ�����
void CMainFrame::OnManualInput()
{
	//int j = 0;
	//volatile int i = 10 / j;
	ShowManualInput("");
}


// �յ���ͼ�Ի������Ϣ
LRESULT CMainFrame::OnScreenDlgMessage(WPARAM wParam, LPARAM iParam)
{
	char *str = (char *)wParam;
	GtWriteTrace(30, "[MainFrm]Receive screenshotDlg message = [%s]", str);

	// �ж��յ�����Ϣ
	if (0 == strcmp(str, RECOGNIZE_PICTURE_FAILED))
	{
		// ��ά��ʶ��ʧ�ܣ������ֶ������ά����
		ShowManualInput("��ά��ʶ��ʧ�ܣ����ֶ�������ˮ�Ż����½�ȡ��ά�룡");
	}
	else if (0 == strcmp(str, QR_CODE_NOT_EXIST))
	{
		// ��ˮ�Ų����ڣ���ʾ�Ƿ����������ά����
		ShowManualInput("��ˮ�Ų����ڣ���ȷ�Ϻ��������룡");
	}
	return 0;
}


LRESULT CMainFrame::OnContinueInput(WPARAM wParam, LPARAM iParam)
{
	SendToWindows();
	//MessageBox("tsadfd");
	return 0;
}


void CMainFrame::OnSysCommand( UINT nID, LPARAM lParam)
{
	switch (nID)
	{
	case SC_MAXIMIZE:
		// ��󻯰�ť����¼�
		break;
	case SC_MINIMIZE:
		// ��С����ť����¼�
		// ���洰��λ����Ϣ���Ƿ������Ϣ
		SaveFrmPosToFile();
		break;
	default:
		break;
	}
	CFrameWnd::OnSysCommand(nID, lParam);
}