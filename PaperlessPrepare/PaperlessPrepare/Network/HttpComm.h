// http通讯
#pragma once

#include "../stdafx.h"

#define CURL_STATICLIB 
#if defined(_DEBUG) 
#pragma comment(lib, "libcurld.lib") 
#else
#pragma comment(lib, "libcurl.lib") 
#endif
#pragma comment ( lib, "winmm.lib" ) 
#pragma comment ( lib, "ws2_32.lib" ) 
#pragma comment ( lib, "wldap32.lib" ) 

#include <string>
#include <string.h>
using std::string;

#include "../Json/json.h"
extern char g_sTmpFilePath[256];

/**
 * 功能:过curl，post发送报文
 * 输入:pStrUrl：发送的url地址
 *		pStrData：发送的数据
 *		nDataSize：发送数据长度
 *		nSendRet:失败原因字符串可存放大小
 * 输出：返回值：0-成功，其他失败
 *		pSendRet：失败原因
 **/ 
int SendData(const char *pStrUrl, const char *pStrData, int nDataSize, int nSendRet, char *pSendRet);

/**
 * 功能：通过curl，post发送报文的回调函数，接收服务端的返回，存到临时文件中
 **/ 
size_t RecvData(void *pBuff, size_t nSize, size_t nMemb, void *pUserp);

/**
 * 功能：从文件读取，返回报文并解析
 **/ 
int AnalyzeData();

// 处理应用程序更新的返回
int DealUpdateRet(Json::Value &sDataValue);

// 启动服务
int StartPaperless();