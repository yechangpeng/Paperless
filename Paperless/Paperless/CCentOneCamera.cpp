#include "CCentOneCamera.h"
#include "MyTTrace.h"
#include "utils.h"
#include "Paperless.h"


CCentOneCamera::CCentOneCamera(CDialogEx *pDialogCamera)
{
	GtWriteTrace(EM_TraceDebug, "%s:%d: CCentOneCamera ���캯��!", __FUNCTION__, __LINE__);
	pVideoWnd = NULL;
	mHighCamera = NULL;
	pVideoWnd = new CStatic();
	pVideoWnd->Create("���ڸ�����������ͷ��ʾ����", WS_CHILD, CRect(0, 0, 120, 120), pDialogCamera);
	pVideoWnd->ShowWindow(SW_SHOW);
	mHighCamera = new CCentermIFImp(pVideoWnd);
}


CCentOneCamera::~CCentOneCamera()
{
	if (mHighCamera)
	{
		delete mHighCamera;
		mHighCamera = NULL;
	}
	if (pVideoWnd)
	{
		delete pVideoWnd;
		pVideoWnd = NULL;
	}
}


// ��д����������ͷ��ȡ������Ƭ
int CCentOneCamera::MySaveDeskIDPic(const char *pSaveDesktopIDPicFilenm)
{
	GtWriteTrace(30, "%s:%d: ������������ͷ��ȡ���֤��Ƭ����!", __FUNCTION__, __LINE__);
	int nRet = 0;
	CImage imSrc;
	CImage imDest;
	// С�ֱ���ͼƬHDC
	HDC destDc;
	// СͼƬ����
	CRect destRect;
	// ͼƬ�ؼ�����
	CRect picRect;
	// �ֱ���
	int width = 0;
	int high = 0;
	char sDeskScanNo[32] = {0};
	char sDeskScanSize[32] = {0};
	char sIDPicWidth[32] = {0};
	char sIDPicHigh[32] = {0};
	char sAutoCropWaitTime[32] = {0};
	// ��ǰ��������·��
	CString sIniFilePath;
	sIniFilePath = GetAppPath() + "\\win.ini";
	// ��ʱ�ַ���
	char sTmpString[32] = {0};
	// ������ ��������ͷ ���
	GetPrivateProfileString("CentermOneMachine", "DeskScanNo", "0", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sDeskScanNo, (const char*)sTmpString, sizeof(sDeskScanNo)-1);
	// ������ ��������ͷ �ֱ��ʼ���
	GetPrivateProfileString("CentermOneMachine", "DeskScanSize", "5", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sDeskScanSize, (const char*)sTmpString, sizeof(sDeskScanSize)-1);
	// �����ǽ�ȡ�����֤�ֱ��ʿ�
	GetPrivateProfileString("CentermOneMachine", "IDPicWidth", "330", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sIDPicWidth, (const char*)sTmpString, sizeof(sIDPicWidth)-1);
	// �����ǽ�ȡ�����֤�ֱ��ʸ�
	GetPrivateProfileString("CentermOneMachine", "IDPicHigh", "210", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sIDPicHigh, (const char*)sTmpString, sizeof(sIDPicHigh)-1);
	// �����������Զ���ѡ����ȴ�ʱ������λ��ms�����ȴ�ֱ�ӻ�ȡͼƬ�����޷���ѡ��
	GetPrivateProfileString("CentermOneMachine", "AutoCropWaitTime", "100", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sAutoCropWaitTime, (const char*)sTmpString, sizeof(sAutoCropWaitTime)-1);

	GtWriteTrace(30, "%s:%d: ����: sDeskScanNo=[%s], sDeskScanSize=[%s], sIDPicWidth=[%s], sIDPicHigh=[%s], sAutoCropWaitTime=[%s]",
		__FUNCTION__, __LINE__, sDeskScanNo, sDeskScanSize, sIDPicWidth, sIDPicHigh, sAutoCropWaitTime);
	if (mHighCamera == NULL)
	{
		mHighCamera = new CCentermIFImp(pVideoWnd);
	}
	GtWriteTrace(30, "%s:%d: �������ͷ!", __FUNCTION__, __LINE__);
	// �������ͷ����
	nRet = mHighCamera->DetectDevice();
	if (nRet <= 0)
	{
		GtWriteTrace(30, "%s:%d: �������ͷ���� DetectDevice() nRet = %d", __FUNCTION__, __LINE__, nRet);
		return 104;
	}
	GtWriteTrace(30, "%s:%d: ������ͷ!", __FUNCTION__, __LINE__);
	// ������ͷ
	nRet = mHighCamera->OpenDevice(atoi(sDeskScanNo));
	if (nRet != 0)
	{
		GtWriteTrace(30, "%s:%d: ������ͷ OpenDevice() nRet = %d", __FUNCTION__, __LINE__, nRet);
		return 105;
	}
	GtWriteTrace(30, "%s:%d: ���÷ֱ���!", __FUNCTION__, __LINE__);
	// ���÷ֱ���
	nRet = mHighCamera->SetScanSize(atoi(sDeskScanSize), atoi(sDeskScanNo));
	if (nRet != 0)
	{
		GtWriteTrace(30, "%s:%d: ���÷ֱ��� SetScanSize() nRet = %d", __FUNCTION__, __LINE__, nRet);
		mHighCamera->CloseDevice();
		return 106;
	}
	GtWriteTrace(30, "%s:%d: �����Զ��ñ�!", __FUNCTION__, __LINE__);
	// �����Զ��ñ�
	nRet = mHighCamera->SetAutoCrop(true, atoi(sDeskScanNo));
	if (nRet != 0)
	{
		GtWriteTrace(30, "%s:%d: �����Զ��ñ� SetAutoCrop() nRet = %d", __FUNCTION__, __LINE__, nRet);
		mHighCamera->CloseDevice();
		return 107;
	}
	Sleep(atoi(sAutoCropWaitTime));
	// �������ǽӿڱ���ͼƬ
	CString tmpDir = pSaveDesktopIDPicFilenm;
	nRet = mHighCamera->ScanImage(tmpDir);
	if (nRet != 0)
	{
		GtWriteTrace(30, "%s:%d: �������ǽӿڱ���ͼƬ ScanImage() nRet = %d", __FUNCTION__, __LINE__, nRet);
		mHighCamera->CloseDevice();
		return 108;
	}
	// �ر��豸
	nRet = mHighCamera->CloseDevice();
	GtWriteTrace(30, "%s:%d: �ر��豸����ֵ CloseDevice() nRet = [%d]", __FUNCTION__, __LINE__, nRet);

	// �޸����֤�ķֱ��ʣ���->С
	width = atoi(sIDPicWidth);
	high = atoi(sIDPicHigh);
	// ����·�������ͼƬ
	imSrc.Load(pSaveDesktopIDPicFilenm);
	if (imSrc.IsNull())
	{
		GtWriteTrace(30, "%s:%d: �ֱ���ת��ʱ����ԴͼƬʧ�� Load()", __FUNCTION__, __LINE__);
		return 110;
	}
	// ����СͼƬ
	if (!imDest.Create(width, high, 24))
	{
		GtWriteTrace(30, "%s:%d: �ֱ���ת��ʱ����Ŀ��ͼƬʧ�� Create()", __FUNCTION__, __LINE__);
		return 111;
	}
	// ��ȡСͼƬHDC
	destDc = imDest.GetDC();
	destRect.SetRect(0, 0, width, high);
	// ����ͼƬ��ʧ��
	SetStretchBltMode(destDc, STRETCH_HALFTONE);
	imSrc.StretchBlt(destDc, destRect, SRCCOPY);
	imDest.ReleaseDC();
	HRESULT hResult = imDest.Save(pSaveDesktopIDPicFilenm);
	if(FAILED(hResult))
	{
		GtWriteTrace(30, "%s:%d: �ֱ���ת��ʱ����Ŀ��ͼƬʧ�� Save()", __FUNCTION__, __LINE__);
		return 112;
	}
	return 0;
}


// ��д����������ͷ��ȡ���֤��Ƭ
int CCentOneCamera::MySaveEnvPic(const char *pSaveEnvPicFilenm)
{
	GtWriteTrace(30, "%s:%d: ���뻷������ͷ��ȡ������Ƭ����!", __FUNCTION__, __LINE__);
	int nRet = 0;
	char sEnvScanNo[32] = {0};
	char sEnvScanSize[32] = {0};
	// ��ǰ��������·��
	CString sIniFilePath;
	sIniFilePath = GetAppPath() + "\\win.ini";
	// ��ʱ�ַ���
	char sTmpString[32] = {0};
	// ������ ��������ͷ ���
	GetPrivateProfileString("CentermOneMachine", "EnvScanNo", "1", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sEnvScanNo, (const char*)sTmpString, sizeof(sEnvScanNo)-1);
	// ������ ��������ͷ �ֱ��ʼ���
	GetPrivateProfileString("CentermOneMachine", "EnvScanSize", "5", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sEnvScanSize, (const char*)sTmpString, sizeof(sEnvScanSize)-1);

	GtWriteTrace(30, "%s:%d: ����: sCentEnvScanNo=[%s], sCentEnvScanSize=[%s]", __FUNCTION__, __LINE__,
		sEnvScanNo, sEnvScanSize);

	if (mHighCamera == NULL)
	{
		mHighCamera = new CCentermIFImp(pVideoWnd);
	}
	nRet = mHighCamera->DetectDevice();
	GtWriteTrace(30, "%s:%d: DetectDevice nRet = %d", __FUNCTION__, __LINE__, nRet);
	// ������ͷ
	nRet = mHighCamera->OpenDevice(atoi(sEnvScanNo));
	if (nRet != 0)
	{
		GtWriteTrace(30, "%s:%d: ������ͷ OpenDevice() nRet = %d", __FUNCTION__, __LINE__, nRet);
		return 105;
	}
	// ���÷ֱ���
	nRet = mHighCamera->SetScanSize(atoi(sEnvScanSize), atoi(sEnvScanNo));
	if (nRet != 0)
	{
		GtWriteTrace(30, "%s:%d: ���÷ֱ��� SetScanSize() nRet = %d", __FUNCTION__, __LINE__, nRet);
		mHighCamera->CloseDevice();
		return 106;
	}
	// �������ǽӿڱ���ͼƬ
	CString tmpDir = pSaveEnvPicFilenm;
	nRet = mHighCamera->ScanImage(tmpDir);
	if (nRet != 0)
	{
		GtWriteTrace(30, "%s:%d: �������ǽӿڱ���ͼƬ ScanImage() nRet = %d", __FUNCTION__, __LINE__, nRet);
		mHighCamera->CloseDevice();
		return 108;
	}
	// �ر��豸
	nRet = mHighCamera->CloseDevice();
	GtWriteTrace(30, "%s:%d: �ر��豸����ֵ CloseDevice() nRet = [%d]", __FUNCTION__, __LINE__, nRet);

	return 0;
}