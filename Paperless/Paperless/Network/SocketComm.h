
#include "./iocp.h"
#include "../Json/json.h"
#include "../CBaseReadIDCard.h"
#include <string>
using namespace std;

#pragma once

// ��ʼ��IOCP����
int InitIocpService(LPVOID lpVoid);

void __stdcall AcceptEvt(LPVOID lpParam, HANDLE hAcceptor, PCHAR buf, DWORD len);

void __stdcall WriteEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len);

void __stdcall ExceptEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len);

void __stdcall CloseEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len);

void __stdcall ReadEvt(LPVOID lpParam, HANDLE hObject, PCHAR buf, DWORD len);

// ���֤ʶ���ǻ�ȡ���֤��Ϣ��Ӧ����ת��
void ReadIDCardCodeTrans(int nRet, char *sRetCode, char *sRetMsg);

// ����ͷ��ȡ��Ƭ��Ӧ����ת��
void ReadPicCodeTrans(int nRet, char *sRetCode, char *sRetMsg);

// ͨ�� PERSONINFO �ṹ�������֤��Ϣjson����
void getIDCardInfoJson(Json::Value &jsonBuff, CString strDir, MYPERSONINFO *pPerson, int nRet);

/* ���ܣ�ͨ�����֤����������json����
 * ��Σ�flag 0-���� 1-����
 *		strDir ��Ƭ����·��
 *		nRet ��������֮ǰ���������ͼƬ�Ƿ�ɹ�
 * ���Σ�jsonBuff�����ر��� 
*/ 
void getIDPicJson(Json::Value &jsonBuff, int flag, CString strDir, int nRet);

// ����json���ݱ���
void SendJsonMsg(Json::Value &jsonBuff, PIO_OP_KEY pOpKey);


CString RetMsg(string xym,string xynr);

CString Json_010101_SendMsg(Json::Value &value);

CString Json_101004_SendMsg(Json::Value &value);

int SendToWindows();

/*
	JSONת���ɷ��͸�������ϵͳ���ַ�������
	CString JsonToSendMsg(string str,CString &sendMsg)
	���������
		str:�յ��ı���
		sendMsg:���ص��ַ�������
	���������
		���ص�CString���͵�jsonӦ����
*/
CString JsonToSendMsg(string str);
