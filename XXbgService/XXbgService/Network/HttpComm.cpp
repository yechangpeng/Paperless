#include "HttpComm.h"

#include "../Curl/curl.h"
#include "../Json/json.h"
#include "../utils.h"
#include "../MyTTrace.h"


/**
 * 功能：通过curl，post发送报文
 * 入参：pStrUrl：发送的url地址
 *		pStrData：发送的数据
 *		nDataSize：发送数据长度
 * 返回值：0-成功，其他失败
 **/ 
int SendHttp1(char *pStrUrl, const char *pStrData, int nDataSize)
{
	if (pStrUrl == NULL || pStrData == NULL || nDataSize < 0)
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
	curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, &write_data1);
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
		GtWriteTrace(30, "[%s][%d]: 发送失败：code=[%d]！", __FUNCTION__, __LINE__, code);
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
size_t write_data1(void *pBuff, size_t nSize, size_t nmemb, void *pUserp)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)pUserp);
	if( NULL == str || NULL == pBuff )
	{
		return -1;
	}
	GtWriteTrace(30, "[%s][%d]: 接到到数据（UTF-8）size=[%d]！", __FUNCTION__, __LINE__, nSize * nmemb);
	GtWriteTrace(30, "[%s][%d]: 接到到数据（UTF-8）buff=[%s]！", __FUNCTION__, __LINE__, pBuff);

	// printf("pBuff%s\n",pBuff);
	//sprintf((char *)pBuff, "{\"code\":\"0\", \"msg\":\"成功\", \"url\":\"http://www.baidu.com\"}");//测试字符串
	//GtWriteTrace(30, "[ScreenDlg]Receive pBuff = [%s], nSize = [%d]", pBuff, strlen((char *)pBuff));

	// utf-8转gbk
	CString utf8String = (char *)pBuff;
	ConvertUtf8ToGBK(utf8String);
	GtWriteTrace(30, "[%s][%d]: 接到到数据（gbk）size=[%d]！", __FUNCTION__, __LINE__, utf8String.GetLength());
	GtWriteTrace(30, "[%s][%d]: 接到到数据（gbk）buff=[%s]！", __FUNCTION__, __LINE__, utf8String.GetBuffer());
	utf8String.ReleaseBuffer();

	string recvBuff = (char *)pBuff;
	// 解析服务端返回的json类型数据，获取交易类型
	//json解析
	Json::Reader reader;
	//表示一个json格式的对象
	Json::Value value;
	// 获取返回信息
	string code;
	string msg;
	string url;
	//解析json报文，存到value中
	if(reader.parse(recvBuff, value))
	{
		// 获取返回码
		code = value["code"].asString();
		if (code != "")
		{
			// 获取返回信息
			msg = value["msg"].asString();
			// 获取url或者失败信息
			url = value["url"].asString();
			//if (code == "0")
			if (code.compare("0") == 0)
			{
				// 解析成功，截取第一个"|"前的网址
				int ret;
				char strUrl[512] = {0};
				ret = splitString(strUrl, url.c_str(), 0);
				if (ret != 0)
				{
					// 获取url地址失败
					GtWriteTrace(30, "[ScreenDlg]splitString() failed! buff=[%s], ret=[%d]!", url.c_str(), ret);
				}
				else 
				{
					GtWriteTrace(30, "[ScreenDlg]splitString() succeed! url=[%s]!", strUrl);
				}
			}
			else if(code.compare("1") == 0)
			{
				// 二维码图片识别失败
				GtWriteTrace(30, "[ScreenDlg]Ret=[%s], msg=[%s], Server recognizing images failed! Will send message to MainFrm!", code.c_str(), msg.c_str());
			}else if(code.compare("2") == 0)
			{
				// 二维码编号不存在
				GtWriteTrace(30, "[ScreenDlg]Ret=[%s], msg=[%s], QR code number is not exist! Will send message to MainFrm!", code.c_str(), msg.c_str());
			}
			else
			{
				// 其他交易类型
				GtWriteTrace(30, "[ScreenDlg]Recvive illegal code[%d]!", code.c_str());
				//::MessageBoxA(NULL, "web端返回未定义交易类型！", "提示", MB_OK);
			}
		}

		string type = value["TYPE"].asString();
		string ret_code = value["RET_CODE"].asString();
		if (type != "")
		{
			CString msg;
			msg.Format("web端返回：%s", ret_code.c_str());
			::MessageBoxA(NULL, msg, "提示", MB_OK);
		}
	}
	else
	{
		// json解析失败
		GtWriteTrace(30, "[ScreenDlg]Read json message failed!");
		::MessageBoxA(NULL, "解析json报文失败！", "提示", MB_OK);
	}

	return nmemb;
}
