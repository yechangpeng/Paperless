
// MainFrm.cpp : CMainFrame 类的实现
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
	// 截取最大化最小化事件进行处理
	ON_WM_SYSCOMMAND()
	// 关闭窗口事件
	ON_MESSAGE_VOID(WM_CLOSE, OnMyClose)
	//ON_MESSAGE(WM_CLOSE, OnClose)
	ON_COMMAND(ID_QUIT, OnQuit)
	ON_COMMAND(ID_TEST, OnManualInput)
	ON_COMMAND(ID_SETTING, OnSettingWin)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	screenshotDlg = NULL;
	input = NULL;
	settingDlg = NULL;
	pBaseReadIDCardInfo = NULL;
	pBaseSaveDeskPic = NULL;
	pBaseSaveEnvPic = NULL;


	// 初始化身份证识读仪，高拍仪文拍摄像头，高拍仪环境摄像头
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
	// 左上角x坐标
	int nWinXPos = 0;
	// 左上角y坐标
	int nWinYPos = 0;
	// 窗口宽
	int nWinWidth = 0;
	// 窗口高
	int nWinHigh = 0;
	// 临时字符串
	CString tmpString;
	CString iniDir = GetAppPath()+"\\win.ini";

	// 获取配置文件中上次退出的窗口是否是最大化状态，是则恢复
	GetPrivateProfileString("Information", "IsZoomed", "0", tmpString.GetBuffer(31), 31, iniDir);
	tmpString.ReleaseBuffer();
	if (0 == tmpString.Compare("1"))
	{
		// 上次退出为最大化状态，设置下次显示为最大化
		cs.style |= WS_MAXIMIZE;
	}
	else
	{
		// 上次退出为非最大化状态，设置下次显示的位置
		// 获取配置文件中的窗口显示的位置
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
			// 配置文件无数据，默认位置，屏幕右下方四分之一的位置
			// 获取任务栏位置，只考虑任务栏在底端的情况
			CRect rect;
			::GetWindowRect(::FindWindow(_T("Shell_TrayWNd"), NULL), &rect);
			// 设置屏幕右下方四分之一的位置
			cs.x = rect.right / 2;
			cs.y = rect.top / 2;
			cs.cx = rect.right / 2;
			cs.cy = rect.top / 2;
		}
		else
		{
			// 按配置文件位置显示
			cs.x = nWinXPos;
			cs.y = nWinYPos;
			cs.cx = nWinWidth;
			cs.cy = nWinHigh;
		}
	}
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.hMenu = NULL;
	return TRUE;
}


void CMainFrame::InitDevice()
{
	// 配置文件获取设备配置信息
	// 身份证识读仪设备
	char sReadIDDevice[32] = {0};
	// 文拍摄像头设备
	char sDeskCameraDevice[32] = {0};
	// 当前程序运行路径
	CString sIniFilePath;
	sIniFilePath = GetAppPath() + "\\win.ini";
	// 临时字符串
	char sTmpString[32] = {0};
	// 身份证识读仪设备
	GetPrivateProfileString("Device", "ReadIDDevice", "-1", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sReadIDDevice, (const char*)sTmpString, sizeof(sReadIDDevice)-1);
	// 文拍摄像头设备
	GetPrivateProfileString("Device", "DesktopCameraDevice", "-1", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sDeskCameraDevice, (const char*)sTmpString, sizeof(sDeskCameraDevice)-1);

	GtWriteTrace(30, "%s:%d: 设备配置参数，ReadIDDevice=[%s], sDeskCameraDevice=[%s]!", __FUNCTION__
		, __LINE__, sReadIDDevice, sDeskCameraDevice);
	// 判断 身份证识读仪设备类型，加载对应类

	CStatic *pStatic = new CStatic();
	switch (atoi(sReadIDDevice))
	{
	case -1:
		::MessageBox(NULL, "身份证识读仪设备未配置！请检查！", "警告", MB_OK);
		break;
	case 1:
		pBaseReadIDCardInfo = new CCentOneReader();
		break;
	case 2:
		pBaseReadIDCardInfo = new CCentReader();
		break;
	default:
		::MessageBox(NULL, "配置的身份证识读仪不存在！请检查！", "警告", MB_OK);
		break;
	}
	// 判断 高拍仪摄像头设备类型，加载对应类
	switch (atoi(sDeskCameraDevice))
	{
	case -1:
		::MessageBox(NULL, "高拍仪摄像头设备未配置！请检查！", "警告", MB_OK);
		break;
	case 1:
		pBaseSaveDeskPic = new CCentOneCamera();
		break;
	case 2:
		pBaseSaveDeskPic = new CGeitCamera();
		break;
	default:
		::MessageBox(NULL, "配置的高拍仪摄像头设备不存在！请检查！", "警告", MB_OK);
		break;
	}
}


/////////////////iocp回调函数//////////////////////////
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

// 身份证识读仪获取身份证信息响应码内转外
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
	strncpy(sRetMsg, "身份证识读仪获取身份证信息失败，其他错误！", 128-1);
}
// 摄像头获取照片响应码内转外
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
	strncpy(sRetMsg, "摄像头获取照片是吧，其他错误！", 128-1);
}

// 通过 PERSONINFO 结构体组json报文
void getIDCardInfoJson(Json::Value &jsonBuff, CString strDir, MYPERSONINFO *pPerson, int nRet)
{
	char pinyin[512] = {0};
	// 有效期限
	char effDate[128] = {0};
	char strFlag[8] = {0};
	// 返回信息
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
		// 获取头像照
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
			// 分配内存存储整个文件
			fileBuffer = (char*) malloc(sizeof(char) * lSize);
			if (fileBuffer == NULL)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDCardInfoJson() malloc failed! err=[%d][%s]", errno, strerror(errno));
				nRet = 202;
				// 关闭文件
				fclose(pFile);
				pFile = NULL;
				break;
			}
			// 将文件拷贝到fileBuffer中
			result = fread(fileBuffer, 1, lSize, pFile);
			if (result != lSize)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDCardInfoJson() read [%s] failed! err=[%d][%s]", strDir.GetBuffer(), errno, strerror(errno));
				nRet = 201;
				// 关闭文件，释放内存
				fclose(pFile);
				pFile = NULL;
				free(fileBuffer);
				fileBuffer = NULL;
				break;
			}
			// 读取身份证正面照文件成功，进行base64编码
			encodeBase64_pic = zBase.Encode((const unsigned char*)fileBuffer, (int)lSize);
			// 释放内存
			free(fileBuffer);
			fileBuffer = NULL;
			fclose(pFile);
			pFile = NULL;
			nRet = 0;
		}while (0);

		// 返回码转换
		ReadIDCardCodeTrans(nRet, sRetCode, sRetMsg);
		// 获取姓名拼音
		MyGetPinYin(pPerson->name, pinyin);
		// 拼身份证有效期限
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
		// 返回码转换
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

/* 功能：通过身份证正反面照组返回报文
 * 入参：flag 0-正面 1-反面
 *		strDir 照片本地路径
 *		nRet 调本函数之前，保存身份图片是否成功
 * 出参：jsonBuff：返回报文 
*/ 
void getIDPicJson(Json::Value &jsonBuff, int flag, CString strDir, int nRet)
{
	// 获取身份证正面照
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
	// 判断 高拍仪获取图片是否成功，不成功返回失败
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
			// 分配内存存储整个文件
			fileBuffer = (char*) malloc(sizeof(char) * lSize);
			if (fileBuffer == NULL)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDPicJson() malloc failed! err=[%d][%s]", errno, strerror(errno));
				nRet = 103;
				jsonBuff[PIC_FLAG] = "";
				// 关闭文件
				fclose(pFile);
				pFile = NULL;
				break;
			}
			// 将文件拷贝到fileBuffer中
			result = fread(fileBuffer, 1, lSize, pFile);
			if (result != lSize)
			{
				GtWriteTrace(EM_TraceDebug, "[MainFrm]getIDPicJson() read [%s] failed! err=[%d][%s]", strDir.GetBuffer(), errno, strerror(errno));
				nRet = 102;
				jsonBuff[PIC_FLAG] = "";
				// 关闭文件，释放内存
				fclose(pFile);
				pFile = NULL;
				free(fileBuffer);
				fileBuffer = NULL;
				break;
			}
			// 读取身份证正面照文件成功，进行base64编码
			encodeBase64_pic = zBase.Encode((const unsigned char*)fileBuffer, (int)lSize);
			// 释放内存
			free(fileBuffer);
			fileBuffer = NULL;
			fclose(pFile);
			pFile = NULL;
			jsonBuff[PIC_FLAG] = encodeBase64_pic.c_str();
			//GtWriteTrace(EM_TraceDebug, "[MainFrm]file buff=[%s]", encodeBase64_pic.c_str());
		} while (0);
		// 返回码转换
		ReadPicCodeTrans(nRet, sRetCode, sRetMsg);
	}
	else
	{
		// 返回码转换
		ReadPicCodeTrans(nRet, sRetCode, sRetMsg);
		jsonBuff[PIC_FLAG] = "";
	}
	jsonBuff["XYM"] = sRetCode;
	jsonBuff["XYSM"] = sRetMsg;
	jsonBuff["OTH_MSG1"] = "";
	return ;
}

// 返回json数据报文
void SendJsonMsg(Json::Value &jsonBuff, PIO_OP_KEY pOpKey)
{
	sockaddr_in addr;
	addr.sin_addr.s_addr = pOpKey->remote_addr;

	// json数据转成string格式
	string msgStr_rtn_gbk = jsonBuff.toStyledString();

	// 转码 GBKToUtf8
	GtWriteTrace(EM_TraceDebug, "%s:%d: 转码前长度(GBK)=[%d]!", __FUNCTION__, __LINE__, msgStr_rtn_gbk.length());
	//GtWriteTrace(EM_TraceDebug, "%s:%d: 转码前数据(GBK)=[%s]!", __FUNCTION__, __LINE__, msgStr_rtn_gbk.c_str());
	string msgStr_rtn = MyGBKToUtf8(msgStr_rtn_gbk);
	//string msgStr_rtn = msgStr_rtn_gbk;
	GtWriteTrace(EM_TraceDebug, "%s:%d: 转码后长度(UTF-8)=[%d]!", __FUNCTION__, __LINE__, msgStr_rtn.length());

	// 发送报文，完整报文=10字节报文长度+报文体；因网络库限制，故长报文需要循环调用发送函数
	// 报文头长度，10字节
	char sPreBuff[10+1] = {0};
	// 报文总字节长度
	long countLen = msgStr_rtn.length();

	// 单次发送最大报文字节长度，64 * 1024 socket网络库(iocp.h)中最大为64 * 1024字节
	const int SEND_MAX_LEN = 32 * 1024;
	// 需要发送的次数
	int count = (countLen - 1) / SEND_MAX_LEN + 1;
	// 本次发送的字节数
	int sendLen = 0;
	// 待发送的报文指针
	char *sendBuff = (char *)msgStr_rtn.c_str();
	// 发送函数返回值
	int resCount = 0;

	sprintf(sPreBuff, "%010ld", countLen);
	// 发送报文头
	GtWriteTrace(EM_TraceDebug, "%s:%d: 发送报文头=[%s]!", __FUNCTION__, __LINE__, sPreBuff);
	WriteDataEx(g_hIoRes, pOpKey, NULL, 0, sPreBuff, sizeof(sPreBuff)-1);
	GtWriteTrace(EM_TraceDebug, "%s:%d: 待发送报文长度=[%d], 分%d次发送!", __FUNCTION__, __LINE__, countLen, count);

	// 循环发送报文，每次发送SEND_MAX_LEN字节
	for (int i = 0; i < count; i++)
	{
		// 获取本次发送字节数，最后一次发送 countLen % SEND_MAX_LEN 字节
		sendLen = (i == count - 1) ? countLen % SEND_MAX_LEN : SEND_MAX_LEN;
		resCount = WriteDataEx(g_hIoRes, pOpKey, NULL, 0, sendBuff + i * SEND_MAX_LEN, sendLen);
		if (resCount < 0)
		{
			GtWriteTrace(EM_TraceDebug, "%s:%d: 第%d次发送报文失败，函数WriteDataEx()返回值[%d] < 0，停止发送报文！", __FUNCTION__, __LINE__, i + 1, resCount);
			break;
		}
		if (resCount == 0)
		{
			Sleep(10);
		}
		GtWriteTrace(EM_TraceDebug, "%s:%d: 第%d次发送报文完成，本次发送[%d]字节!", __FUNCTION__, __LINE__, i + 1, sendLen);
		if (i == count - 1)
		{
			GtWriteTrace(EM_TraceDebug, "%s:%d: 报文全部发送完成，共[%d]字节!", __FUNCTION__, __LINE__, countLen);
		}
	}
	return ;
}

CString RetMsg(string xym,string xynr)
{
	CString ret_str="";
	Json::Value msgStr_json_rtn;//表示一个json格式的对象 
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
	Json::Reader reader;//json解析
	std::string out="";
	sendMsg+=out.c_str();
	CString reStr="";

	//GtWriteTrace(EM_TraceDebug,"%s",sendMsg.GetBuffer());
	out=value["HM"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//户名为空
		reStr=RetMsg("02","户名是必输项，不可为空");
		//写日志
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
		//证件类型为空
		reStr=RetMsg("03","证件类型是必输项，不可为空");
		//写日志
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
		//证件号码为空
		reStr=RetMsg("04","证件号码是必输项，不可为空");
		//写日志
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
		//姓名为空
		reStr=RetMsg("05","姓名是必输项，不可为空");
		//写日志
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
		//证件是否长期有效为空
		reStr=RetMsg("06","证件是否长期有效是必输项，不可为空");
		//写日志
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
				//证件到期日为空
				reStr=RetMsg("14","证件到期日是必输项，不可为空");
				//写日志
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
		//发证机关地区代码为空
		reStr=RetMsg("07","发证机关地区代码是必输项，不可为空");
		//写日志
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
		//性别为空
		reStr=RetMsg("08","性别是必输项，不可为空");
		//写日志
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
		//国籍为空
		reStr=RetMsg("09","国籍是必输项，不可为空");
		//写日志
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
			//移动电话为空
			reStr=RetMsg("10","固定电话跟移动电话必输其中一项");
			//写日志
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
		//固定电话不为空，移动电话为空
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
		//通讯地址为空
		reStr=RetMsg("11","通讯地址是必输项，不可为空");
		//写日志
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
		//邮政编码不是必输项
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
		//职业为空
		reStr=RetMsg("12","职业是必输项，不可为空");
		//写日志
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
		//不报错
		sendMsg+="\n";
		//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		/*out=value["JJLXDH"].asString();
		if(out.empty()!=0) //true 1 false 0
		{
			//不报错，紧急联系人有，紧急联系电话一定得有，紧急联系人没有，紧急联系电话就不用输入
			//证件是否长期有效为空
			reStr=RetMsg("13","存在紧急联系人，紧急联系电话则是必输项，不可为空");
			//写日志
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
			//报错，紧急联系人有，紧急联系电话一定得有
			//证件是否长期有效为空
			reStr=RetMsg("13","存在紧急联系人，紧急联系电话则是必输项，不可为空");
			//写日志
			return reStr;
		}
		else
		{
			sendMsg+=out.c_str();
			//GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		}
	}
	reStr=RetMsg("000","发送成功");
	return reStr;
}
CString Json_101004_SendMsg(Json::Value &value)
{
	Json::Reader reader;//json解析
	std::string out="";
	sendMsg+=out.c_str();
	CString reStr="";	

	out=value["ZFZFF"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//资费支付方为空
		reStr=RetMsg("014","资费支付方是必输项，不可为空");
		//写日志
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
		//收款人姓名为空
		reStr=RetMsg("015","收款人姓名是必输项，不可为空");
		//写日志
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
		//收款人账号/卡号为空
		reStr=RetMsg("016","收款人账号/卡号是必输项，不可为空");
		//写日志
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
		//汇款金额为空
		reStr=RetMsg("017","汇款金额是必输项，不可为空");
		//写日志
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
		//转账类型为空
		reStr=RetMsg("018","转账类型是必输项，不可为空");
		//写日志
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
		//汇款人账号/卡号标志为空
		reStr=RetMsg("020","汇款人账号/卡号标志是必输项，不可为空");
		//写日志
		return reStr;
	}
	else
	{
		if(0==strcmp(out.c_str(),"C_BZ"))
		{
			//发送第一段内容
			GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
		}
		
	}
	out=value["ZHMMBZ"].asString();
	if(out.empty()!=0) //true 1 false 0
	{
		//账户密码标志为空
		reStr=RetMsg("021","账户密码标志是必输项，不可为空");
		//写日志
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
		//汇款人证件类型为空
		reStr=RetMsg("022","汇款人证件类型是必输项，不可为空");
		//写日志
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
		//汇款人证件号码为空
		reStr=RetMsg("023","汇款人证件号码是必输项，不可为空");
		//写日志
		return reStr;
	}
	else
	{
		sendMsg+=out.c_str();
		sendMsg+='\n';
		GtWriteTrace(EM_TraceDebug,"%s",sendMsg);
	}
	reStr=RetMsg("000","发送成功");
	return reStr;
}

int SendToWindows()
{
	// 自动填单交易进行处理
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
		//获取窗口句柄
		if(wname.GetBuffer() == "")
		{
			::MessageBox(NULL,"目标窗口未配置","提示",MB_OK);
			return -1;
		}

		hWnd = ::FindWindow(NULL,wname.GetBuffer());
		if(NULL==hWnd){
			::MessageBox(NULL,"没有找到窗口","提示",MB_OK);
			return -1;
		}

		//目标窗口线程与当前线程关联
		DWORD curtid = GetCurrentThreadId();
		DWORD tid = GetWindowThreadProcessId(hWnd, NULL);
		DWORD error = 0;
	/**关联两个线程，使目标线程与当前线程共用消息队列，这步很关键，
		成功后可设置目标线程窗口为活动窗口，并获取焦点位置的句柄.*/
		hCurFocus = NULL;
		//目标窗口置顶
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
			CString errStr = "关联线程失败" + error;
			::MessageBox(NULL,errStr,"提示",MB_OK);
			return -1;
		}

	
		//目标窗口置顶
		//::SetWindowPos(hWnd,HWND_TOPMOST,20,20,820,420,SWP_SHOWWINDOW);
		//发送跨线程消息
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

		//结束线程关联
		if(!AttachThreadInput(tid, curtid, false))
		{
			::MessageBox(NULL,"取消线程关联失败！","提示",MB_OK);
			return -1;
		}
		//取消置顶
		::SetWindowPos(hWnd,HWND_NOTOPMOST,100,100,900,500,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
	//	::SetWindowPos(hWnd,HWND_NOTOPMOST,100,100,900,500,SWP_SHOWWINDOW);
		return 0;
}

/*
	JSON转换成发送给第三方系统的字符串报文
	CString JsonToSendMsg(string str,CString &sendMsg)
	输入参数：
		str:收到的报文
		sendMsg:返回的字符串报文
	输出参数：
		返回的CString类型的json应答报文
*/
CString JsonToSendMsg(string str)
{
	Json::Reader reader;//json解析
	Json::Value jValue;//表示一个json格式的对象
	std::string out="";
	CString reStr="";
	if(reader.parse(str,jValue))//解析出json放到json中区
	{
		out=jValue["JYDM"].asString();
		GtWriteTrace(EM_TraceDebug,"%s",out.c_str());
		//活期开户
		if(0==strcmp(out.c_str(),"010101"))
		{
			sendMsg="";
			sendMsg+=out.c_str();
			reStr=Json_010101_SendMsg(jValue);
			return reStr;
		}
		//个人账户到账户汇款
		if(0==strcmp(out.c_str(),"101004"))
		{
			sendMsg="";
			sendMsg+=out.c_str();
			reStr=Json_101004_SendMsg(jValue);
			
			return reStr;
		}
		else
		{
			//未知交易码
			reStr=RetMsg("01","未知交易码");
			return reStr;
		}		
	}
	else
	{
		GtWriteTrace(EM_TraceDebug,"error = [%s]", (reader.getFormatedErrorMessages()).c_str());
		//解析失败
		reStr=RetMsg("99","解析失败");
		return reStr;
	}
}
/**************************************************************************
*函数名：void __stdcall ReadEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
*函数说明：socket的读数据回调函数
*参数说明：
*buf:接收到的数据
*len：接收的数据长度
*函数返回值：DWORD
***************************************************************************/
void __stdcall ReadEvt(
	LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	GtWriteTrace(EM_TraceDebug, "--->收到新请求...");
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
	GtWriteTrace(EM_TraceDebug,"解码后%s",msgStr.GetBuffer());

	/////
	//CMainFrame * p_CMainFrame = dynamic_cast<CMainFrame *>((CMainFrame *) lpParam);
	//	::MessageBox(NULL,msgStr,"提示",MB_OK);
	//	std::string strValue="{\"KEY\":\"value666\",\"ARRAY\":[{\"KEY2\":\"value2\"},{\"KEY2\":\"value3\"},{\"KEY2\":\"value4\"}]}"; 
	std::string strValue=msgStr.GetBuffer();
	Json::Reader reader;//json解析
	Json::Value msgStr_json_rtn;//表示一个json格式的对象 
	Json::Value value;//表示一个json格式的对象
	std::string msgStr_rtn;
	std::string out;
	std::string tran_type;
	CString reStr="";
    Json::Reader ret_reader;//json解析
	std::string ret_out;
	GtWriteTrace(EM_TraceDebug,"Get message! strValue:%s",strValue.c_str());
	if(reader.parse(strValue,value))//解析出json放到json中区
	{
		tran_type = value["BWKZLX"].asString();
		if (0 == tran_type.compare("0"))
		{
			//读取配置文件
			GetPrivateProfileString("Information","Wname","test.txt - 记事本",wname.GetBuffer(100),100,GetAppPath()+"\\win.ini");
			long_time_interval=GetPrivateProfileInt("Information","DefaultInterval",10,GetAppPath()+"\\win.ini");
			short_time_interval=GetPrivateProfileInt("Information","EnterInputInterval",10,GetAppPath()+"\\win.ini");
			// 自动填单交易
			reStr=JsonToSendMsg(strValue);
			if(ret_reader.parse(reStr.GetBuffer(),msgStr_json_rtn))//解析出json放到json中区
			{
				ret_out=msgStr_json_rtn["XYM"].asString();
				GtWriteTrace(EM_TraceDebug,"%s",ret_out.c_str());
				//活期开户
				if(0!=strcmp(ret_out.c_str(),"000"))
				{
					GtWriteTrace(EM_TraceDebug,"XYM[%s]",ret_out.c_str());
					sockaddr_in addr;
					addr.sin_addr.s_addr = pOpKey->remote_addr;
					int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
					//写日志
					return;
				}
			}

			SendToWindows();

			sockaddr_in addr;
			addr.sin_addr.s_addr = pOpKey->remote_addr;
			int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
			//写日志
			return;
		}
		else if (0 == tran_type.compare("1"))
		{
			// 获取身份证芯片信息
			int nRet = 0;
			MYPERSONINFO pMyPerson;
			memset(&pMyPerson, 0, sizeof(PERSONINFO));
			CString str = GetAppPath();
			str.Append("\\IDPicture\\HeadPictureTmp.jpg");
			nRet = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pBaseReadIDCardInfo->MyReadIDCardInfo(str.GetBuffer(), &pMyPerson);
// 			nRet = 0;
// 			memcpy(pMyPerson.address, "福建省大田县上京镇三阳村13-1号", sizeof(pMyPerson.address));
// 			memcpy(pMyPerson.appendMsg, "", sizeof(pMyPerson.appendMsg));
// 			memcpy(pMyPerson.birthday, "19941022", sizeof(pMyPerson.birthday));
// 			memcpy(pMyPerson.cardId, "350425199410220517", sizeof(pMyPerson.cardId));
// 			memcpy(pMyPerson.cardType, "", sizeof(pMyPerson.cardType));
// 			memcpy(pMyPerson.EngName, "", sizeof(pMyPerson.EngName));
// 			memcpy(pMyPerson.govCode, "", sizeof(pMyPerson.govCode));
// 			pMyPerson.iFlag = 0;
// 			memcpy(pMyPerson.name, "叶长鹏", sizeof(pMyPerson.name));
// 			memcpy(pMyPerson.nation, "汉", sizeof(pMyPerson.nation));
// 			memcpy(pMyPerson.nationCode, "", sizeof(pMyPerson.nationCode));
// 			memcpy(pMyPerson.otherData, "", sizeof(pMyPerson.otherData));
// 			memcpy(pMyPerson.police, "大田县公安局", sizeof(pMyPerson.police));
// 			memcpy(pMyPerson.sex, "男", sizeof(pMyPerson.sex));
// 			memcpy(pMyPerson.sexCode, "", sizeof(pMyPerson.sexCode));
// 			memcpy(pMyPerson.validEnd, "20201221", sizeof(pMyPerson.validEnd));
// 			memcpy(pMyPerson.validStart, "20101221", sizeof(pMyPerson.validStart));
// 			memcpy(pMyPerson.version, "", sizeof(pMyPerson.version));
			// 通过个人信息头像路径和返回值拼json报文
			getIDCardInfoJson(msgStr_json_rtn, str, &pMyPerson, nRet);
			// 将json报文发送
			SendJsonMsg(msgStr_json_rtn, pOpKey);
		}
		else if (0 == tran_type.compare("2"))
		{
			// 获取身份证正面信息
			int nRet = 0;
			CString str = GetAppPath();
			str.Append("\\IDPicture\\FrontPictureTmp.jpg");
			nRet = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pBaseSaveDeskPic->MySaveDeskIDPic(str.GetBuffer());
			//nRet = 0;
			GtWriteTrace(EM_TraceDebug, "[MainFrm]Save file [%s] return = [%d]", str.GetBuffer(), nRet);
			// 通过身份证正面信息返回值拼json报文
			getIDPicJson(msgStr_json_rtn, 0, str, nRet);
			// 将json报文发送
			SendJsonMsg(msgStr_json_rtn, pOpKey);
		}
		else if (0 == tran_type.compare("3"))
		{
			// 获取身份证反面信息
			int nRet = 0;
			CString str = GetAppPath();
			str.Append("\\IDPicture\\BackPictureTmp.jpg");
			nRet = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pBaseSaveDeskPic->MySaveDeskIDPic(str.GetBuffer());
			//nRet = 0;
			GtWriteTrace(EM_TraceDebug, "[MainFrm]Save file [%s] return = [%d]", str.GetBuffer(), nRet);
			// 通过身份证反面信息返回值拼json报文
			getIDPicJson(msgStr_json_rtn, 1, str, nRet);
			// 将json报文发送
			SendJsonMsg(msgStr_json_rtn, pOpKey);
		}
		else if (0 == tran_type.compare("4"))
		{
			// 获取 环境摄像头人像照
			int nRet = 0;
			CString str = GetAppPath();
			str.Append("\\IDPicture\\EnvPictureTmp.jpg");
			nRet = ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->pBaseSaveDeskPic->MySaveEnvPic(str.GetBuffer());
			//nRet = 0;
			GtWriteTrace(EM_TraceDebug, "[MainFrm]Save file [%s] return = [%d]", str.GetBuffer(), nRet);
			// 通过 环境摄像头信息返回值拼json报文
			getIDPicJson(msgStr_json_rtn, 2, str, nRet);
			// 将json报文发送
			SendJsonMsg(msgStr_json_rtn, pOpKey);
		}
		else
		{
			//未知交易类型
			GtWriteTrace(EM_TraceDebug, "[MainFrm]Unknown tran type! tran_type = [%s]", tran_type.c_str());
			msgStr_json_rtn["XYM"]="998";
			msgStr_json_rtn["XYSM"]="未知的报文控制类型";
			msgStr_rtn=msgStr_json_rtn.toStyledString();
			reStr = msgStr_rtn.c_str();
			sockaddr_in addr;
			addr.sin_addr.s_addr = pOpKey->remote_addr;
			//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
			int resCount = WriteDataEx(g_hIoRes, pOpKey,NULL, 0, reStr.GetBuffer(), reStr.GetLength());
			//写日志
			GtWriteTrace(EM_TraceDebug, "%s", msgStr_rtn.c_str());
			return;
		}
	}
	else
	{
		GtWriteTrace(EM_TraceDebug,"Read json failed! error = [%s]", (reader.getFormatedErrorMessages()).c_str());
		//解析失败
		msgStr_json_rtn["XYM"]="999";
		msgStr_json_rtn["XYSM"]="json报文解析失败";
		msgStr_rtn=msgStr_json_rtn.toStyledString();
		GtWriteTrace(EM_TraceDebug,"%s",msgStr_rtn.c_str());
		CString reStr = msgStr_rtn.c_str();
		sockaddr_in addr;
		addr.sin_addr.s_addr = pOpKey->remote_addr;
		int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
		//写日志
		return;
	}
#if 0
	::MessageBox(NULL,msgStr,"提示",MB_OK);
	char msgStr_all[4096] = {0};
	char msg_type[2+1] = {0};
	char msg_controltype[2+1] = {0};
	char msg_id[8+1] = {0};
	char msg_body[4096] = {0};
	char msgStr_rtn[1024] = {0};			//回复消息
	strncpy(msgStr_all,msgStr.GetBuffer(),msgStr.GetLength());
	//写日志
	GtWriteTrace(EM_TraceDebug,"解码后%s",msgStr_all);
	//GtWriteTrace(EM_TraceDebug,msgStr_all);

	if( msgStr.GetLength()<=12 )
	{
		sprintf(msgStr_rtn,"02000000000099");
		CString reStr = msgStr_rtn;
		sockaddr_in addr;
		addr.sin_addr.s_addr = pOpKey->remote_addr;
		//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
		int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
		//写日志
		return;
	}
	strncpy(msg_type,msgStr_all,2);//报文类型
	strncpy(msg_controltype,msgStr_all+2,2);//控制类型
	strncpy(msg_id,msgStr_all+4,8);//报文id
	strncpy(msg_body,msgStr_all+12,strlen(msgStr_all)-12);//报文内容
	if(strncmp(msg_type,"01",2) == 0)
	{
		//请求报文
		if(strncmp(msg_controltype,"01",2) == 0)//显示用户信息
		{
			//写日志

			//
			memset(msgStr.GetBuffer(),0x00,msgStr.GetLength()+1);
			strncpy(msgStr.GetBuffer(),msg_body,strlen(msgStr_all)-12);		
		}
		else if(strncmp(msg_controltype,"02",2) == 0)//显示网页内容
		{
			//显示网页(发消息给指定窗口)
			::SendMessageA(((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView()->GetSafeHwnd(),WM_HTML_SHOW,(WPARAM)msg_body,NULL);
			//写日志
			//回应答包
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
			//错误的控制类型
			sprintf(msgStr_rtn,"02%s%s99",msg_controltype,msg_id);
			CString reStr = msgStr_rtn;
			sockaddr_in addr;
			addr.sin_addr.s_addr = pOpKey->remote_addr;
			//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
			int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
			//写日志
			return;
		}
	}
	else
	{
		//错误的报文类型
		sprintf(msgStr_rtn,"02%s%s99",msg_controltype,msg_id);
		CString reStr = msgStr_rtn;
		sockaddr_in addr;
		addr.sin_addr.s_addr = pOpKey->remote_addr;
		//int resCount = WriteDataEx(g_hIoRes,pOpKey,inet_ntoa(addr.sin_addr),pOpKey->remote_port,reStr.GetBuffer(),reStr.GetLength());
		int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
		//写日志
		return;
	}
#endif
#if 0
	if (tran_type.compare("0") == 0)
	{
		// 自动填单交易进行处理
		//::MessageBox(NULL,msgStr,"提示",MB_OK);
		Json::Value msgStr_json_rtn;//表示一个json格式的对象 

		std::string msgStr_rtn;
		Json::Reader ret_reader;//json解析
		Json::Value ret_value;//表示一个json格式的对象
		std::string ret_out;
		if(ret_reader.parse(reStr.GetBuffer(),ret_value))//解析出json放到json中区
		{
			ret_out=ret_value["XYM"].asString();
			GtWriteTrace(EM_TraceDebug,"%s",ret_out.c_str());
			//活期开户
			if(0!=strcmp(ret_out.c_str(),"000"))
			{
				GtWriteTrace(EM_TraceDebug,"XYM[%s]",ret_out.c_str());
				sockaddr_in addr;
				addr.sin_addr.s_addr = pOpKey->remote_addr;
				int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
				//写日志
				return;
			}
		}
		GtWriteTrace(EM_TraceDebug,"sendMsg:[%s]",sendMsg);
		//::MessageBox(NULL,sendMsg,"提示",MB_OK);
		////////////////////
		BOOL bRes = FALSE;
		int strlen = sendMsg.GetLength();
		//读取配置文件
		GetPrivateProfileString("Information","Wname","test.txt - 记事本",wname.GetBuffer(100),100,GetAppPath()+"\\win.ini");
		int long_time_interval,short_time_interval;
		long_time_interval=GetPrivateProfileInt("Information","DefaultInterval",10,GetAppPath()+"\\win.ini");
		short_time_interval=GetPrivateProfileInt("Information","EnterInputInterval",10,GetAppPath()+"\\win.ini");
		//获取窗口句柄
		HWND hWnd = ::FindWindow(NULL,wname.GetBuffer());
		if(NULL==hWnd){
			::MessageBox(NULL,"没有找到窗口","提示",MB_OK);
		}

		//目标窗口线程与当前线程关联
		DWORD curtid = GetCurrentThreadId();
		DWORD tid = GetWindowThreadProcessId(hWnd, NULL);
		DWORD error = 0;
	/**关联两个线程，使目标线程与当前线程共用消息队列，这步很关键，
		成功后可设置目标线程窗口为活动窗口，并获取焦点位置的句柄.*/
		HWND hCurFocus = NULL;
		//目标窗口置顶
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
			CString errStr = "关联线程失败" + error;
			::MessageBox(NULL,errStr,"提示",MB_OK);
		}

	
		//目标窗口置顶
		//::SetWindowPos(hWnd,HWND_TOPMOST,20,20,820,420,SWP_SHOWWINDOW);
		//发送跨线程消息
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

		//结束线程关联
		if(!AttachThreadInput(tid, curtid, false))
		{
			::MessageBox(NULL,"取消线程关联失败！","提示",MB_OK);
		}
		//取消置顶
		::SetWindowPos(hWnd,HWND_NOTOPMOST,100,100,900,500,SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
	//	::SetWindowPos(hWnd,HWND_NOTOPMOST,100,100,900,500,SWP_SHOWWINDOW);
		//sprintf(msgStr_rtn,"02%s%s00",msg_controltype,msg_id);
		//strncpy(msgStr_rtn,"00",2);
		//未知交易码
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
	GtWriteTrace(EM_TraceDebug, "<---请求处理结束！\n");
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


/////////////////iocp回掉函数/////////////////////////

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//启动就隐藏到右下角
	memset(&m_nid,0,sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.hWnd = this->m_hWnd;
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	m_nid.uCallbackMessage = WM_TRAYNOTIFY;
	CString strToolTip = "后台服务";
	strcpy(m_nid.szTip,strToolTip.GetBuffer());
	m_nid.uID = IDR_MAINFRAME;
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nid.hIcon = hIcon;
	Shell_NotifyIcon(NIM_ADD,&m_nid);
	if(hIcon)
		::DestroyIcon(hIcon);
	BOOL flag = m_menu.LoadMenu(IDR_POPMENU); 
	//初始化截屏窗口
//	m_ScreenshotDlg = new CScreenshotDlg;
//	m_ScreenshotDlg->Create(IDD_SCREENSHOT);
//	m_ScreenshotDlg->ShowWindow(SW_HIDE);

	//钩子启动标志，初始为FALSE未启动状态
	m_hookFlag = FALSE;
	// 启动钩子
	StartKeyBoardHook();

	//初始化网络连接
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

// CMainFrame 诊断

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

// CMainFrame 消息处理程序
LRESULT CMainFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case WM_RBUTTONDOWN:              // 右键起来时弹出菜单  
		{  
			CMenu *pMenu = m_menu.GetSubMenu(0);
			if(pMenu){
				LPPOINT lpoint = new tagPOINT;  
				::GetCursorPos(lpoint);                    // 得到鼠标位置  	
				SetForegroundWindow();
				pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, lpoint->x ,lpoint->y, AfxGetMainWnd());   
				delete lpoint;
			}	  
			break;  
		}  
	case WM_LBUTTONDBLCLK:                            // 双击左键的处理      
		if (AfxGetApp()->m_pMainWnd->IsZoomed())
		{
			// 最大化，恢复最大化
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		}
		else if (AfxGetApp()->m_pMainWnd->IsIconic())
		{
			// 最小化，恢复原先状态
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
	// TODO: 在此添加专用代码和/或调用基类
	nCmdShow = SW_HIDE;
	CFrameWnd::ActivateFrame(nCmdShow);
}


void CMainFrame::OnQuit()
{
	// 退出程序，保存窗口位置到配置文件中
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

// 启用钩子
void CMainFrame::StartKeyBoardHook()
{
	if(!m_hookFlag)
	{
		// 配置文件获取当前热键
		char sHotKeyValue[32] = {0};
		// 热键值
		GetPrivateProfileString("Information", "HotKeyValue", "3|162|164|83|0|", sHotKeyValue, sizeof(sHotKeyValue)-1, GetAppPath()+"\\win.ini");

		char tmpKey[32] = {0};
		//设置快捷键
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
		combKey.keys[2] = 83;  //S键
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


		//设置通知窗口句柄和通知消息类型
		SetHWndAndMsgType(myHwnd, myMsgType);
		// 设置快捷键
		SetCombKey(combKey);
		//设置全局键盘钩子
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


// 保存主窗口位置到配置文件中
BOOL CMainFrame::SaveFrmPosToFile()
{
	// 配置文件路径
	CString iniDir = GetAppPath()+"\\win.ini";
	if (IsZoomed())
	{
		// 最大化，保存状态到配置文件中
		// 最大化状态：1-是
		WritePrivateProfileString("Information", "IsZoomed", "1", iniDir);
	}
	else if (IsIconic())
	{
		// 最小化，不处理，因为在点击最小化之前已经保存过了
	}
	else
	{
		// 非最大化，保存窗口位置到配置文件中
		RECT rect = {0};
		CString tmp;
		// 获取当前窗口位置
		::GetWindowRect(this->m_hWnd, &rect);
		tmp.Format("%d", rect.left);
		WritePrivateProfileString("Information", "WinXPos", tmp, iniDir);
		tmp.Format("%d", rect.top);
		WritePrivateProfileString("Information", "WinYPos", tmp, iniDir);
		tmp.Format("%d", rect.right - rect.left);
		WritePrivateProfileString("Information", "WinWidth", tmp, iniDir);
		tmp.Format("%d", rect.bottom - rect.top);
		WritePrivateProfileString("Information", "WinHigh", tmp, iniDir);
		// 最大化状态：0-否
		WritePrivateProfileString("Information", "IsZoomed", "0", iniDir);
	}
	return TRUE;
}

// 启动截图
LRESULT CMainFrame::OnScreenshot(WPARAM wParam, LPARAM lParam)
{
//	MessageBox("收到截屏消息!");
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


// 显示二维码流水号对话框
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


// 托盘菜单->手动输入
void CMainFrame::OnManualInput()
{
	//int j = 0;
	//volatile int i = 10 / j;
	ShowManualInput("");
}


// 收到截图对话框的消息
LRESULT CMainFrame::OnScreenDlgMessage(WPARAM wParam, LPARAM iParam)
{
	char *str = (char *)wParam;
	GtWriteTrace(30, "[MainFrm]Receive screenshotDlg message = [%s]", str);

	// 判断收到的消息
	if (0 == strcmp(str, RECOGNIZE_PICTURE_FAILED))
	{
		// 二维码识别失败，弹出手动输入二维码编号
		ShowManualInput("二维码识别失败，请手动输入流水号或重新截取二维码！");
	}
	else if (0 == strcmp(str, QR_CODE_NOT_EXIST))
	{
		// 流水号不存在，提示是否重新输入二维码编号
		ShowManualInput("流水号不存在，请确认后重新输入！");
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
		// 最大化按钮点击事件
		break;
	case SC_MINIMIZE:
		// 最小化按钮点击事件
		// 保存窗口位置信息，是否最大化信息
		SaveFrmPosToFile();
		break;
	default:
		break;
	}
	CFrameWnd::OnSysCommand(nID, lParam);
}