// http通讯
#include "../stdafx.h"
#pragma once

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

/**
 * 功能：通过curl，post发送报文
 * 入参：pStrUrl：发送的url地址
 *		pStrData：发送的数据
 *		nDataSize：发送数据长度
 * 返回值：0-成功，其他失败
 **/ 
int SendHttp1(char *pStrUrl, const char *pStrData, int nDataSize);


/**
 * 功能：通过curl，post发送报文的回调函数，接收服务端的返回
 **/ 
size_t write_data1(void *pBuff, size_t nSize, size_t nMemb, void *pUserp);