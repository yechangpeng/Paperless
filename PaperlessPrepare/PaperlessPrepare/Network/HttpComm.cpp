#include "HttpComm.h"

#include "../Curl/curl.h"
#include "../utils.h"
#include "../MyTTrace.h"
#include "../resource.h"
#include "../PaperlessPrepareDlg.h"
#include "../CBase64.h"

/**
 * 功能：通过curl，post发送报文
 * 输入：pStrUrl：发送的url地址
 *		pStrData：发送的数据
 *		nDataSize：发送数据长度
 *		nSendRet:失败原因字符串可存放大小
 * 输出：返回值：0-成功，其他失败
 *		pSendRet：失败原因
 **/ 
int SendData(const char *pStrUrl, const char *pStrData, int nDataSize, int nSendRet, char *pSendRet)
{
	if (pStrUrl == NULL || pStrData == NULL || nDataSize < 0 || pSendRet == NULL || nSendRet <= 0)
	{
		return -1000;
	}
	BOOL ret = true;
	// url 打印到日志中
	GtWriteTrace(30, "[%s][%d]: 发送地址：[%s]", __FUNCTION__, __LINE__, pStrUrl);

	// post方式发送
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *conn = curl_easy_init();
	curl_slist *plist = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");
	// 超时时间
	curl_easy_setopt(conn, CURLOPT_TIMEOUT, 5);
	// url地址
	curl_easy_setopt(conn, CURLOPT_URL, pStrUrl);
	// 服务端返回报文的回调函数
	curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &RecvData);
	// 设置http请求头
	curl_easy_setopt(conn, CURLOPT_HTTPHEADER, plist);
	// 发送的数据
	curl_easy_setopt(conn, CURLOPT_POSTFIELDS, pStrData);
	// 发送数据的长度
	curl_easy_setopt(conn, CURLOPT_POSTFIELDSIZE, nDataSize);
	// 设置post发送方式
	curl_easy_setopt(conn, CURLOPT_POST, true);

	CURLcode code = curl_easy_perform(conn);
	if(code != CURLE_OK)
	{ 
		GtWriteTrace(30, "[%s][%d]: 发送失败：code=[%d] msg=[%s]！", __FUNCTION__, __LINE__, code, curl_easy_strerror(code));
		memcpy(pSendRet, curl_easy_strerror(code), nSendRet - 1);
	}
	else
	{
		GtWriteTrace(30, "[%s][%d]: 发送成功！", __FUNCTION__, __LINE__);
	}
	// 清理资源
	curl_easy_cleanup(conn);

	return code;
}

/**
 * 功能：通过curl，post发送报文的回调函数，接收服务端的返回
 **/ 
size_t RecvData(void *pBuff, size_t nSize, size_t nmemb, void *pUserp)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)pUserp);
	if( NULL == str || NULL == pBuff )
	{
		return -1;
	}
	GtWriteTrace(30, "[%s][%d]: 接到到数据（UTF-8）size=[%d]！", __FUNCTION__, __LINE__, nSize * nmemb);
	GtWriteTrace(30, "[%s][%d]: 接到到数据（UTF-8）buff=[%s]！", __FUNCTION__, __LINE__, pBuff);

	// printf("pBuff%s\n",pBuff);
	//GtWriteTrace(30, "[ScreenDlg]Receive pBuff = [%s], nSize = [%d]", pBuff, strlen((char *)pBuff));

	// utf-8转gbk
	CString utf8String = (char *)pBuff;
	ConvertUtf8ToGBK(utf8String);
	GtWriteTrace(30, "[%s][%d]: 接到到数据（gbk）size=[%d]！", __FUNCTION__, __LINE__, utf8String.GetLength());
	GtWriteTrace(30, "[%s][%d]: 接到到数据（gbk）buff=[%s]！", __FUNCTION__, __LINE__, utf8String.GetBuffer());
	utf8String.ReleaseBuffer();
	//sprintf((char *)pBuff, "{\"type\":\"2\", \"code\":\"2\", \"msg\":\"成功\", \"filename\":\"XXbgService.exe\", \"version\":\"1.1\", \"application\":\"eyJCV0taTFgiOiI0IiwgIkpZRE0iOiIxMDEwMTAifQ==\"}");//测试字符串
	sprintf((char *)pBuff, "{\"type\":\"2\", \"code\":\"0\", \"msg\":\"成功\", \"filename\":\"XXbgService.exe\", \"version\":\"1.1\", \"application\":\"eyJCV0taTFgiOiI0IiwgIkpZRE0iOiIxMDEwMTAifQ==\"}");//测试字符串

	string recvBuff = (char *)pBuff;
	// 解析服务端返回的json类型数据，获取交易类型
	//json解析
	Json::Reader reader;
	//表示一个json格式的对象
	Json::Value value;
	// 获取返回信息
	string type;
	//解析json报文，存到value中
	if(reader.parse(recvBuff, value))
	{
		// 获取交易类型
		if (value.size() != 0)
		{
			type = value["type"].asString();
			if (type == "2")
			{
				// 应用程序更新的返回报文处理
				DealUpdateRet(value);
			}
			else
			{
				GtWriteTrace(30, "[%s][%d]服务器返回未定义的交易类型，type=[%s]!", __FUNCTION__, __LINE__, type);
				//::MessageBoxA(NULL, "服务器返回未定义的交易类型！", "提示", MB_OK);
				((CPaperlessPrepareDlg *)(AfxGetApp()->m_pMainWnd))->MyRetryWin("服务器处理错误！");
			}
		}
		else
		{
			// json解析失败
			GtWriteTrace(30, "[%s][%d]服务器返回非json格式报文,解析出的json结构体size=0!", __FUNCTION__, __LINE__);
			//::MessageBoxA(NULL, "服务器返回非json格式报文!！", "提示", MB_OK);
			((CPaperlessPrepareDlg *)(AfxGetApp()->m_pMainWnd))->MyRetryWin("服务器处理异常！");
		}
		
	}
	else
	{
		// json解析失败
		GtWriteTrace(30, "[%s][%d]服务器返回非json格式报文!", __FUNCTION__, __LINE__);
		//::MessageBoxA(NULL, "服务器返回非json格式报文!！", "提示", MB_OK);
		((CPaperlessPrepareDlg *)(AfxGetApp()->m_pMainWnd))->MyRetryWin("服务器处理异常2！");
	}

	return nmemb;
}

// 处理应用程序更新的返回
int DealUpdateRet(Json::Value &sDataValue)
{
	string code;
	string msg;
	string filename;
	string version;
	string application;

	// 获取返回码
	code = sDataValue["code"].asString();
	if ("0" == code)
	{
		// 服务器返回新版本
		GtWriteTrace(30, "[%s][%d]服务器返回更新版本!", __FUNCTION__, __LINE__);
		filename = sDataValue["filename"].asString();
		version = sDataValue["version"].asString();
		application = sDataValue["application"].asString();

		char sSrcFile[256] = {0};
		char sDestFile[256] = {0};
		sprintf_s(sSrcFile, sizeof(sSrcFile)-1, "%s/%s", GetFilePath().GetBuffer(), filename.c_str());
		sprintf_s(sDestFile, sizeof(sDestFile)-1, "%s/%s.loadtmp", GetFilePath().GetBuffer(), filename.c_str());
		GtWriteTrace(30, "[%s][%d]源文件=[%s]，目标文件=[%s]!", __FUNCTION__, __LINE__, sSrcFile, sDestFile);

		// 源文件备份，覆盖剪切
		MoveFileEx(sDestFile, sSrcFile, MOVEFILE_REPLACE_EXISTING);

		// 将文件进行base64解码，并写入到文件中
		int nRet = 0;
		ZBase64 zBase;
		int nBase64AfterLens = 0;
		string sBase64After = zBase.Decode(application.c_str(), application.length(), nBase64AfterLens);

		GtWriteTrace(30, "[%s][%d]待写入 len=[%d]!", __FUNCTION__, __LINE__, nBase64AfterLens);

		FILE *fp = NULL;
		fp = fopen(sSrcFile, "wb");
		if (fp != NULL)
		{
			GtWriteTrace(30, "[%s][%d]更新版本保存到本地!", __FUNCTION__, __LINE__);
			nRet = fwrite(sBase64After.c_str(), 1, nBase64AfterLens, fp);
			fclose(fp);
			if (nRet != nBase64AfterLens)
			{
				// 写文件失败，源文件重新拷贝回来
				MoveFileEx(sSrcFile, sDestFile, MOVEFILE_REPLACE_EXISTING);
				((CPaperlessPrepareDlg *)(AfxGetApp()->m_pMainWnd))->MyRetryWin("保存最新版本程序失败！");
				return -1;
			}
			// 应用程序更新成功，版本号写入配置文件中
			WritePrivateProfileString("Information", "Version", version.c_str(), GetFilePath()+"\\win.ini");
		}
		// 启动主程序，成功启动后将关闭本程序
		StartPaperless();
	}
	else if ("1" == code)
	{
		// 当前文件和最新版本一致
		GtWriteTrace(30, "[%s][%d]最新版本和当前版本一致!", __FUNCTION__, __LINE__);
		// 启动主程序，成功启动后将关闭本程序
		StartPaperless();
	}
	else
	{
		GtWriteTrace(30, "[%s][%d]服务器返回失败:%s", __FUNCTION__, __LINE__, msg.c_str());
		// 服务器返回失败
		msg = sDataValue["msg"].asString();
		char tmp[128] = {0};
		sprintf_s(tmp, sizeof(tmp)-1, "服务器返回失败:%s", msg.c_str());
		((CPaperlessPrepareDlg *)(AfxGetApp()->m_pMainWnd))->MyRetryWin(tmp);
	}
	return 0;
}



// 启动主程序，成功启动后将关闭本程序
int StartPaperless()
{
	char sAppDir[256] = {0};
	char sRetMsg[128] = {0};

	// 主应用程序路径
	GetPrivateProfileString("Information", "AppDir", "../Paperless.exe", sAppDir, sizeof(sAppDir)-1, GetFilePath()+"\\win.ini");
	GtWriteTrace(30, "[%s][%d]启动程序=[%s]!", __FUNCTION__, __LINE__, sAppDir);
	// 启动主程序
	LPCSTR lpcsDir = _T(sAppDir);
	ShellExecute(NULL, "open", sAppDir, NULL, NULL, SW_SHOWNORMAL);
	// 关闭本程序
	((CPaperlessPrepareDlg *)(AfxGetApp()->m_pMainWnd))->OnBnClickedCancel();
	return 0;
}