#include "stdafx.h"
#include <string>
#include <string.h>
using std::string;

#pragma once

/**    
    ���ܣ�    ������ת����ƴ��ȫƴ�����罫��������ת���ɡ�mazhaorui����������ma521����ת���ɡ�mama521����
    @param[in]    Chinese��Ҫת���ĺ����ַ�
    @param[out]    PinYin��ת�����ƴ���ַ�
    @par�޸ļ�¼��
**/
void GetPinYin(unsigned char* Chinese, string& PinYin);

void GetOtherCodePinYin(int nCode, string& strValue);

/* ����:�����������ת��дƴ�����Ҹ�������ƴ��֮����һ���ո�
 * ��Σ�name����������ģ������ַ������붼Ϊ���ģ��Ҳ����пո�����ֽ���128�ֽ�
 * ���Σ�PinYin��ת���ƴ��
 */
void MyGetPinYin(const char *name, char *PinYin);

// GBKתUTF-8
void ConvertGBKToUtf8(CString &strGBK);

// UTF-8תGBK
void ConvertUtf8ToGBK(CString &strUtf8);

// GBKתUTF-8  2
string MyGBKToUtf8(const string& strGBK);

// UTF-8תGBK  2
string MyUtf8ToGBK(const string& strUTF8);

/* ��ȡ��ǰ����ִ��·��
*/
CString GetAppPath();

/* ͨ��GetLastError()�ķ���ֵ��ȡ������Ϣ
 * ������Ϣ��sLastMsg
*/
char* MyGetLastError(char *sLastMsg, int strLen);

/* ��'|'�ָ��ַ�������ȡsrc�ַ����� index ���Ӵ�(index��0��ʼ)������des�ַ���
 * 0-�ɹ�������-ʧ��
*/
int splitString(char *des, const char *src, int index);

/** ���ܣ���ȡע�����Ϣ����ȡREG_DWORD��REG_SZ���͵�����
 * ��Σ�pRootKey����key
 *		pSubKey����key
 *		pKeyName��key����
 *		pKeyValue����ȡ����ֵ
 *		nKeyValueLen����ȡ��ֵ�ĳ���
 *		pKeyValueType��Ҫ��ȡ��ֵ������
 * ����ֵ��TRUE-��ȡ�ɹ���FALSE-��ȡʧ��
 **/
BOOL MyReadRegedit(HKEY pRootKey, char *pSubKey, char *pKeyName, void *pKeyValue, int nKeyValueLen, DWORD pKeyValueType);

/** ���ܣ�дע�����Ϣ��дREG_DWORD��REG_SZ���͵�����
 * ��Σ�pRootKey����key
 *		pSubKey����key
 *		pKeyName����д��key����
 *		pKeyValue����д��key��ֵ
 *		pKeyValueType����д��keyֵ������
 * ����ֵ��TRUE-д��ɹ���FALSE-д��ʧ��
 **/
BOOL MyWriteRegedit(HKEY pRootKey, char *pSubKey, char *pKeyName, void *pKeyValue, DWORD pKeyValueType);

/** ���ܣ�����ע����
 * ��Σ�pRootKey����key
 *		pSubKey����key
 * ����ֵ��TRUE-д��ɹ���FALSE-д��ʧ��
 **/
BOOL MyAddRegedit(HKEY pRootKey, char *pSubKey);

/** ���ܣ��жϵ�ǰ�������32λ����64λϵͳ������ʱ��ʹ�ô˺�����
 * ����ֵ��64 - ��64λϵͳ��32 - 32λϵͳ������ - ��ȡʧ��
 **/
int GetSystemBit();

/* ��ȡsFilePath���ļ���Ϣ����pStat��
*/
BOOL GetFileAttributes(struct _stati64 *pStat, CString sFilePath);