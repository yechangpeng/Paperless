#include "CCentOneReader.h"
#include "MyTTrace.h"
#include "utils.h"


// ��д����ȡ���֤оƬ��Ϣ
int CCentOneReader::MyReadIDCardInfo(const char *pSaveHeadPicFilenm, MYPERSONINFO *pPersonInfo)
{
	GtWriteTrace(30, "%s:%d: �����ȡ���֤оƬ��Ϣ����!", __FUNCTION__, __LINE__);
	if (pSaveHeadPicFilenm == NULL)
	{
		return -1;
	}
	// �������֤ʶ����Ϣ�ṹ��
	PERSONINFO pPerson;
	memset( &pPerson, 0, sizeof(PERSONINFO) );
	int nRet = 0;
	// ��ȡ�����ļ��е�����
	char sDiv[32] = {0};
	char sBp[32] = {0};
	char sBaud[32] = {0};
	char sTimeOut[32] = {0};
	// ��ǰ��������·��
	CString sIniFilePath;
	sIniFilePath = GetAppPath() + "\\win.ini";
	// ��ʱ�ַ���
	char sTmpString[32] = {0};
	// �豸���Ӷ˿�
	GetPrivateProfileString("CentermOneMachine", "Device", "HID", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sDiv, (const char*)sTmpString, sizeof(sDiv)-1);
	// BP��
	GetPrivateProfileString("CentermOneMachine", "BpNo", "9", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sBp, (const char*)sTmpString, sizeof(sBp)-1);
	// ���ڲ�����
	GetPrivateProfileString("CentermOneMachine", "Baud", "115200", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sBaud, (const char*)sTmpString, sizeof(sBaud)-1);
	// Ѱ����ʱʱ��
	GetPrivateProfileString("CentermOneMachine", "TimeOut", "5", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sTimeOut, (const char*)sTmpString, sizeof(sTimeOut)-1);

	GtWriteTrace(30, "%s:%d: ����: sDiv=[%s], sBp=[%s], sBaud=[%s], sTimeOut=[%s], pSaveHeadPicFilenm=[%s]",
		__FUNCTION__, __LINE__, sDiv, sBp, sBaud, sTimeOut, pSaveHeadPicFilenm);
	// �������ǽӿڻ�ȡ���֤��Ϣ
	nRet = CT_ReadIDCard( sDiv, sBp, atoi(sBaud), atoi(sTimeOut), (char *)pSaveHeadPicFilenm, &pPerson );
	GtWriteTrace(30, "%s:%d: �����֤ʶ���ӿڷ���ֵ nRet = [%d]!",  __FUNCTION__, __LINE__, nRet);
	switch (nRet)
	{
	case 0:
		// ��ȡ�ɹ�
		nRet = 0;
		// ������Ϣ�ṹ�帳ֵ
		memcpy(pPersonInfo, &pPerson, sizeof(PERSONINFO));
		break;
	case -34:
		// δ��⵽���֤
		nRet = 204;
		break;
	case -36:
		// δ��⵽�豸
		nRet = 203;
		break;
	default:
		break;
	}

	return nRet;
}