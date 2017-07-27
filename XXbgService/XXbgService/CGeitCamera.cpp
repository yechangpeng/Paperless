#include "stdafx.h"
#include <highgui.h>
#include <opencv2/legacy/legacy.hpp>
#include <math.h>
#include "cv.h"
#include "cxcore.h"

#include "CGeitCamera.h"
#include "MyTTrace.h"
#include "utils.h"
#include "opencv/OpencvUtils.h"


CGeitCamera::CGeitCamera()
{

}


CGeitCamera::~CGeitCamera()
{

}


// ��д����������ͷ��ȡ���֤��Ƭ
int CGeitCamera::MySaveDeskIDPic(const char *pSaveDesktopIDPicFilenm)
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
	char sIDPicWidth[32] = {0};
	char sIDPicHigh[32] = {0};
	char sAutoCropWaitTime[32] = {0};
	// ��ǰ��������·��
	CString sIniFilePath;
	sIniFilePath = GetFilePath() + "\\win.ini";
	// ��ʱ�ַ���
	char sTmpString[32] = {0};
	// ������ ��������ͷ ���
	GetPrivateProfileString("GeitCamera", "DeskScanNo", "0", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sDeskScanNo, (const char*)sTmpString, sizeof(sDeskScanNo)-1);
	// �����ǽ�ȡ�����֤�ֱ��ʿ�
	GetPrivateProfileString("GeitCamera", "IDPicWidth", "330", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sIDPicWidth, (const char*)sTmpString, sizeof(sIDPicWidth)-1);
	// �����ǽ�ȡ�����֤�ֱ��ʸ�
	GetPrivateProfileString("GeitCamera", "IDPicHigh", "210", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sIDPicHigh, (const char*)sTmpString, sizeof(sIDPicHigh)-1);
	// �����������Զ���ѡ����ȴ�ʱ������λ��ms�����ȴ�ֱ�ӻ�ȡͼƬ�����޷���ѡ��
	GetPrivateProfileString("GeitCamera", "AutoCropWaitTime", "100", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sAutoCropWaitTime, (const char*)sTmpString, sizeof(sAutoCropWaitTime)-1);

	GtWriteTrace(30, "%s:%d: ����: sDeskScanNo=[%s], sIDPicWidth=[%s], sIDPicHigh=[%s], sAutoCropWaitTime=[%s]",
		__FUNCTION__, __LINE__, sDeskScanNo, sIDPicWidth, sIDPicHigh, sAutoCropWaitTime);

	//����IplImageָ��
	IplImage *pFrame = NULL;
	IplImage *pImgSrc = NULL;
	IplImage *pDest = NULL;
	int nReadErrCount = 0;
	// ��ȡ����ͷ
	CvCapture* pCapture = cvCreateCameraCapture(atoi(sDeskScanNo));
	if (pCapture == NULL)
	{
		return 105;
	}
	// ��������ͷ�ֱ���
	cvSetCaptureProperty(pCapture , CV_CAP_PROP_FRAME_WIDTH , 1280);
	cvSetCaptureProperty(pCapture , CV_CAP_PROP_FRAME_HEIGHT, 960);

	// �ȴ�ʱ��
	Sleep(atoi(sAutoCropWaitTime));
	int nReadNullCount = 0;
	pFrame = cvQueryFrame( pCapture );
	while (NULL == pFrame)
	{
		nReadNullCount++;
		if (nReadNullCount > 6)
		{
			break;
		}
		pFrame=cvQueryFrame( pCapture );
	}
	if (NULL == pFrame)
	{
		GtWriteTrace(30, "%s:%d: ��ȡ����ͷ��Ƭʧ�� cvQueryFrame()", __FUNCTION__, __LINE__);
		cvReleaseCapture(&pCapture);
		return 108;
	}
	pImgSrc = cvCreateImage(cvSize(pFrame->width, pFrame->height), pFrame->depth, pFrame->nChannels);
	//����ͼ��֡
	cvCopy(pFrame, pImgSrc, NULL); 
	// �ͷ�����ͷ
	cvReleaseCapture(&pCapture);

//	cvNamedWindow("pImgSrc", 1);
// 	cvShowImage("pImgSrc", pImgSrc);
// 	cvWaitKey(0);
// 	cvDestroyWindow("pImgSrc");


	// ��ѡͼ���ڵ����֤�����浽ͼ��pDest��
	nRet = MyAutoCrop(pImgSrc, &pDest);
	char pSaveDesktopIDPicFilenm_1[256] = {0};
	sprintf_s(pSaveDesktopIDPicFilenm_1, sizeof(pSaveDesktopIDPicFilenm_1)-1, "%s\\IDPicture\\pic.jpg", GetAppPath().GetBuffer());
	cvSaveImage(pSaveDesktopIDPicFilenm_1, pImgSrc);
	//nRet = 1;
	if (nRet == 0 && pDest != NULL)
	{
		cvSaveImage(pSaveDesktopIDPicFilenm, pDest);
		// �ͷ�Ŀ��ͼ��
		cvReleaseImage(&pDest);
	}
	else
	{
		// �޷��ҵ����֤������ԭʼͼ
		cvSaveImage(pSaveDesktopIDPicFilenm, pImgSrc);
	}

	GtWriteTrace(30, "%s:%d: Դͼ���ȡ��ɣ�׼��ת����С�ֱ���ͼƬ...", __FUNCTION__, __LINE__);
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
	//HRESULT hResult = 0;
	if(FAILED(hResult))
	{
		GtWriteTrace(30, "%s:%d: �ֱ���ת��ʱ����Ŀ��ͼƬʧ�� Save()", __FUNCTION__, __LINE__);
		return 112;
	}
	GtWriteTrace(30, "%s:%d: ת��С�ֱ���ͼƬ���.", __FUNCTION__, __LINE__);
	return 0;
}

// ��д����������ͷ��ȡ������Ƭ
int CGeitCamera::MySaveEnvPic(const char *pSaveEnvPicFilenm)
{
	GtWriteTrace(30, "%s:%d: ���뻷������ͷ��ȡ������Ƭ����!", __FUNCTION__, __LINE__);
	int nRet = 0;
	char sEnvScanNo[32] = {0};
	char sAutoCropWaitTime[32] = {0};
	// ��ǰ��������·��
	CString sIniFilePath;
	sIniFilePath = GetFilePath() + "\\win.ini";
	// ��ʱ�ַ���
	char sTmpString[32] = {0};
	// ������ ��������ͷ ���
	GetPrivateProfileString("GeitCamera", "EnvScanNo", "0", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sEnvScanNo, (const char*)sTmpString, sizeof(sEnvScanNo)-1);
	// �����������Զ���ѡ����ȴ�ʱ������λ��ms�����ȴ�ֱ�ӻ�ȡͼƬ�����޷���ѡ��
	GetPrivateProfileString("GeitCamera", "AutoCropWaitTime", "5", sTmpString, sizeof(sTmpString)-1, sIniFilePath);
	memcpy(sAutoCropWaitTime, (const char*)sTmpString, sizeof(sAutoCropWaitTime)-1);

	GtWriteTrace(30, "%s:%d: ����: sEnvScanNo=[%s], sAutoCropWaitTime=[%s]", __FUNCTION__, __LINE__, sEnvScanNo, sAutoCropWaitTime);


	//����IplImageָ��
	IplImage* pFrame = NULL;
	// ��ȡ����ͷ
	CvCapture* pCapture = cvCreateCameraCapture(atoi(sEnvScanNo));
	if (pCapture == NULL)
	{
		return 105;
	}
	// ��������ͷ�ֱ���
	cvSetCaptureProperty(pCapture , CV_CAP_PROP_FRAME_WIDTH , 1600);
	cvSetCaptureProperty(pCapture , CV_CAP_PROP_FRAME_HEIGHT, 1200);

	// �ȴ�ʱ��
	Sleep(atoi(sAutoCropWaitTime));
	int nReadNullCount = 0;
	pFrame = cvQueryFrame( pCapture );
	while (NULL == pFrame)
	{
		nReadNullCount++;
		if (nReadNullCount > 6)
		{
			break;
		}
		pFrame=cvQueryFrame( pCapture );
	}
	if (NULL == pFrame)
	{
		GtWriteTrace(30, "%s:%d: ��ȡ����ͷ��Ƭʧ�� cvQueryFrame()", __FUNCTION__, __LINE__);
		cvReleaseCapture(&pCapture);
		return 108;
	}
	// ����ԭʼͼ
	cvSaveImage(pSaveEnvPicFilenm, pFrame);
	// �ͷ�����ͷ
	cvReleaseCapture(&pCapture);
	return 0;
}