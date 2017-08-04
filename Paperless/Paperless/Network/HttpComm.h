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

// http���͵����ݽṹ��
typedef struct S_SEND_DATA
{
	// ���ͱ�������
	int nFlag;
	int nSendLen;
	char *pSendBuff;
}SEND_DATA;

/**
 * ����:ͨ��curl��post���ͱ��ĵ�Ԥ������
 * ����:nFlag�����͵ı������ͣ�0-��ά���ͼ��1-��ˮ��
 *		pStrData��Я�������ݣ���ά���ͼʱЯ����ͼ����·������ˮ��ʱЯ��������ˮ��
 * ����ֵ��0 - �ɹ���-1 - ��μ��ʧ�ܣ����� - ʧ��
 **/ 
int SendDataPrepare(int nFlag, const char *pStrData);


// ����HTTP�����̺߳���
UINT ThreadSendDataFunc(LPVOID pParm);

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