#include "../stdafx.h"
#include "SocketComm.h"
#include "../MyTTrace.h"
#include "../CBase64.h"
#include "../utils.h"
#include "../PaperlessDlg.h"

HANDLE g_hIoRes = NULL;
CString msgStr = "";
CString sendMsg="";
CString wname;
int long_time_interval, short_time_interval;
HWND hWnd;
HWND hCurFocus;


int InitIocpService(LPVOID lpVoid)
{
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
		lpVoid
		);
	if (hObject)
	{
		StartListenerAccept(g_hIoRes, hObject, 5);
	}
	return 0;
}

void __stdcall AcceptEvt(LPVOID lpParam, HANDLE hAcceptor, PCHAR buf, DWORD len)
{
	SetLimitReadLen(g_hIoRes, hAcceptor, FALSE);
	GtWriteTrace(EM_TraceDebug, "%s:%d: iocp回调函数：AcceptEvt()!", __FUNCTION__, __LINE__);
}

void __stdcall WriteEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	GtWriteTrace(EM_TraceDebug, "%s:%d: iocp回调函数：WriteEvt()!", __FUNCTION__, __LINE__);
}

void __stdcall ExceptEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	GtWriteTrace(EM_TraceDebug, "%s:%d: iocp回调函数：ExceptEvt()!", __FUNCTION__, __LINE__);
}

void __stdcall CloseEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	GtWriteTrace(EM_TraceDebug, "%s:%d: iocp回调函数：CloseEvt()!", __FUNCTION__, __LINE__);
}

/**************************************************************************
*函数名：void __stdcall ReadEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
*函数说明：socket的读数据回调函数
*参数说明：
*buf:接收到的数据
*len：接收的数据长度
*函数返回值：DWORD
***************************************************************************/
void __stdcall ReadEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len)
{
	GtWriteTrace(EM_TraceDebug, "%s:%d: --->收到新请求...", __FUNCTION__, __LINE__);
	PIO_OP_KEY pOpKey = (PIO_OP_KEY)hObject;
	buf[len] = '\0';
	GtWriteTrace(EM_TraceDebug, "%s:%d: 收到报文[%s]", __FUNCTION__, __LINE__, buf);

	// base64解码
	ZBase64 zBase;
	int msg_base64_after_len = 0;
	string strValue = zBase.Decode((const char*)buf, len, msg_base64_after_len);
	GtWriteTrace(EM_TraceDebug, "%s:%d: base64解码后[%s]", __FUNCTION__, __LINE__, strValue.c_str());

	Json::Reader reader;//json解析
	Json::Value value;//表示一个json格式的对象
	std::string msgStr_rtn;
	std::string out;
	std::string tran_type;

	CString reStr="";
	Json::Reader ret_reader;//json解析
	Json::Value msgStr_json_rtn;//表示一个json格式的对象 
	std::string ret_out;
	if(reader.parse(strValue, value))//解析出json放到value中区
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
			::MessageBoxA(NULL, sendMsg, "预填单报文", MB_OK);

			SendToWindows();

			sockaddr_in addr;
			addr.sin_addr.s_addr = pOpKey->remote_addr;
			int resCount = WriteDataEx(g_hIoRes,pOpKey,NULL,0,reStr.GetBuffer(),reStr.GetLength());
			//写日志
			return;
		}
		else
		{
			CPaperlessDlg* pPaperlessDlg = (CPaperlessDlg*)AfxGetApp()->m_pMainWnd;
			if (0 == tran_type.compare("1"))
			{
				// 获取身份证芯片信息
				int nRet = 0;
				MYPERSONINFO pMyPerson;
				memset(&pMyPerson, 0, sizeof(MYPERSONINFO));
				CString str = GetAppPath();
				str.Append("\\IDPicture\\HeadPictureTmp.jpg");
				nRet = pPaperlessDlg->pBaseReadIDCardInfo->MyReadIDCardInfo(str.GetBuffer(), &pMyPerson);
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
				nRet = pPaperlessDlg->pBaseSaveCameraPic->MySaveDeskIDPic(str.GetBuffer());
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
				nRet = pPaperlessDlg->pBaseSaveCameraPic->MySaveDeskIDPic(str.GetBuffer());
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
				nRet = pPaperlessDlg->pBaseSaveCameraPic->MySaveEnvPic(str.GetBuffer());
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
	GtWriteTrace(EM_TraceDebug, "<---请求处理结束！\n");
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
	strncpy(sRetMsg, "摄像头获取照片失败，其他错误！", 128-1);
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