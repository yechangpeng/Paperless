// httpͨѶ
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
 * ����:ͨ��curl��post���ͱ���
 * ����:pStrUrl�����͵�url��ַ
 *		pStrData�����͵�����
 *		nDataSize���������ݳ���
 *		nSendRet:ʧ��ԭ���ַ����ɴ�Ŵ�С
 *		pSendRet��ʧ��ԭ��
 * ����ֵ��0-�ɹ�������ʧ��
 **/ 
int SendData(const char *pStrUrl, int nDataSize, const char *pStrData, int nSendRet, char *pSendRet);

/**
 * ���ܣ�ͨ��curl��post���ͱ��ĵĻص����������շ���˵ķ��أ��浽��ʱ�ļ�g_sTmpFilePath��
 **/ 
size_t RecvData(void *pBuff, size_t nSize, size_t nMemb, void *pUserp);

/**
 * ���ܣ����ļ���ȡ���ر��Ĳ�����
 **/ 
int AnalyzeData();

// ����Ӧ�ó�����µķ���
int DealUpdateRet(Json::Value &sDataValue);

// ��������
int StartPaperless();