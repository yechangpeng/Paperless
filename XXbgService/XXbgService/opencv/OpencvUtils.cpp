#include "OpencvUtils.h"
#include "../MyTTrace.h"

/** 
* ���ܣ��Զ���ѡ��ԭͼ�ڵ����֤
* ������pImgSrc������ȡ��ԭͼ��
*		pImgDest����ȡ�������֤ͼ�񣬱����ֶ��ͷ�
* ����ֵ��0-�ɹ�������-ʧ��
**/
int MyAutoCrop(IplImage *pImgSrc, IplImage **pImgDest)
{
	if (pImgSrc == NULL)
	{
		GtWriteTrace(30, "%s:%d: �ֶ��ñߺ�����ԭͼ��ΪNULL");
		return NULL;
	}
	// �洢����������
	CvMemStorage* storage = cvCreateMemStorage(0);
	// ԭͼ��Ҷ�ͼ����֮���ֵ����ͼ��
	IplImage* pImgGray = NULL;
	// ָ��������ָ��
	CvSeq* contour = NULL;
	CvSeq* contourTmp = NULL;
	// ��ȡ�����������������������
	double lMaxArea = 0;
	// ������ȡ��������С�ڴ�������򲻴���
	double lMinArea = 1000.0;

// 	cvNamedWindow("�Ҷ�ͼ��ֵ�����ͼ��", 1);
// 	cvNamedWindow("�ü����ͼ��", 1);
// 	cvNamedWindow("��ת���ͼ��", 1);
// 	cvNamedWindow("������ͼ��", 1);

	// ԭͼת��Ϊ�Ҷ�ͼ��
	pImgGray = cvCreateImage(cvGetSize(pImgSrc), 8, 1);
	cvZero(pImgGray);
	cvCvtColor(pImgSrc, pImgGray, CV_BGR2GRAY);

	// ԭͼ��Ҷ�ͼ���ж�ֵ�������з�ֵ������Ҫ�޸ģ�
	cvThreshold(pImgGray, pImgGray, 130, 255, CV_THRESH_BINARY);

	// ��ʾ��ֵ�����ԭͼ��
// 	cvShowImage("�Ҷ�ͼ��ֵ�����ͼ��", pImgGray);
// 	cvWaitKey(0);


	// �Ҷ�ͼ���ж�ֵ������ȡ����
	int contour_num = cvFindContours(pImgGray, storage, &contour, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	contourTmp = contour;
	// �ȱ���һ�Σ���ȡ�����ͬ������������һ��������������������С
	for( ; contourTmp != 0; contourTmp = contourTmp->h_next )
	{
		double lTmpArea = fabs(cvContourArea(contourTmp));
		if (lTmpArea < lMinArea)
		{
			// ȥ��С�������
			cvSeqRemove(contourTmp, 0);
			continue;
		}
		if(lTmpArea > lMaxArea)
		{
			lMaxArea = lTmpArea;
		}
	}
	// ָ��ָ��ԭ��������������������������
	contourTmp = contour;
	for(; contourTmp != 0; contourTmp = contourTmp->h_next)
	{
		double lTmpArea = fabs(cvContourArea(contourTmp));
		if (lTmpArea == lMaxArea)
		{
			// ������������
			// ��ȡ������Ӿ���
			CvRect rect = cvBoundingRect(contourTmp,1);
			CvRect rectTmp = cvRect(rect.x, rect.y, rect.width, rect.height);
			// ������һ�ü���ͼ��
			IplImage* pDestCut = cvCreateImage(cvSize(rectTmp.width, rectTmp.height), pImgSrc->depth, pImgSrc->nChannels);
			// ����Դͼ��ROI����һ�βü�������Ӿ��Σ�֮������ת��С��Ӿ���
			cvSetImageROI(pImgSrc, rectTmp);
			//����ͼ��
			cvCopy(pImgSrc, pDestCut, NULL); 
			//Դͼ����������ROI
 			cvResetImageROI(pImgSrc);
// 			cvShowImage("�ü����ͼ��", pDestCut);
// 			cvWaitKey(0);

			// ��ת���ͼ��
			IplImage* pDestRemote = NULL;
			// �����С��Ӿ���
			CvBox2D box2D  = cvMinAreaRect2(contourTmp);
			// ����ת�ĽǶȣ�ȡ����ֵ
			float degree = fabs(box2D.angle);
			GtWriteTrace(30, "%s:%d: ����: degree = [%.3f] size[%.3f][%.3f]", __FUNCTION__, __LINE__, degree, box2D.size.width,
				box2D.size.height);
			if (((90 - degree) < 0.0001) || ((degree - 0) < 0.0001))
			{
				// �Ƕ�ָ�������ߺ�ˮƽ�н�
				// �����0�ȣ��ҿ�<�ߣ���ת90�ȣ������90�ȣ��ҿ�>�ߣ���ת90��
				if ( ((box2D.size.width < box2D.size.height) && ((degree - 0) < 0.0001)) || 
					((box2D.size.width > box2D.size.height) && ((90 - degree) < 0.0001)) )
				{
					degree = -90;
					pDestRemote = rotateImage(pDestCut, degree);
				}
				else
				{
					pDestRemote = pDestCut;
				}
			}
			else
			{
				// �� 0~90 ��֮��
				// ͨ�����ο�Ⱥ͸߶��ж���ת����
				if (box2D.size.width < box2D.size.height)
				{
					degree = degree - 90;
					pDestRemote = rotateImage(pDestCut, degree);
				}
				else
				{
					pDestRemote = rotateImage(pDestCut, degree);
				}
			}

			if (pDestRemote == NULL)
			{
				GtWriteTrace(30, "%s:%d: ��ȡ��ת���ͼ��ʧ�ܣ�", __FUNCTION__, __LINE__);
				return NULL;
			}
			// �ͷŵ�һ�βü���ͼ��
			if (pDestRemote != pDestCut)
			{
				cvReleaseImage(&pDestCut);
			}
// 			cvShowImage("��ת���ͼ��", pDestRemote);
// 			cvWaitKey(0);
			// ��С��Ӿ��ε��������ߣ��� > �ߣ�
			float saveWidth = box2D.size.width > box2D.size.height ? box2D.size.width : box2D.size.height;
			float saveHeight = box2D.size.width < box2D.size.height ? box2D.size.width : box2D.size.height;
			// �����ڶ��βü���ͼ��
			*pImgDest = cvCreateImage(cvSize(saveWidth, saveHeight), pDestRemote->depth, pDestRemote->nChannels);
			// �ü�ͼ��Ĵ�С�����߶�1����
			CvRect mSaveRect = cvRect( (pDestRemote->width - saveWidth) / 2, (pDestRemote->height - saveHeight) / 2,
				saveWidth, saveHeight);
			// ������ת��ͼ���roi
			cvSetImageROI(pDestRemote, mSaveRect);
			// ������Ŀ��ͼ����
			cvCopy(pDestRemote, *pImgDest, NULL);
			// �ͷ�roi
			cvReleaseImage(&pDestRemote);
// 			cvShowImage("������ͼ��", *pImgDest);
// 			cvWaitKey(0);
		}
	}

	//cvWaitKey(0);
// 	cvDestroyWindow("�Ҷ�ͼ��ֵ�����ͼ��");
// 	cvDestroyWindow("�ü����ͼ��");
// 	cvDestroyWindow("��ת���ͼ��");
// 	cvDestroyWindow("������ͼ��");
	cvReleaseImage(&pImgGray);
	//�ͷ��ڴ�
	cvReleaseMemStorage(&storage);
	return 0;
}

/** 
* ���ܣ����ı��С����תͼ��
* ������pImgSrc������ת��ԭͼ��
*		degree������ת�ĽǶȣ������ƣ���������ʾ��ʱ�룬������ʾ˳ʱ��
* ����ֵ����ת���ͼ��ʹ�ú���Ҫ�ֶ��ͷţ���ΪNULL��ʾʧ��
**/
IplImage* rotateImage(IplImage* pImgSrc, float lDegree)
{
	// �����ƽ�ת���ɽǶ���
	double lAngle = lDegree  * CV_PI / 180.;
	// �Ƕȵ�sin��cosֵ
	double lSinAngle = sin(lAngle);
	double lCosAngle = cos(lAngle);
	// ԭͼ�Ŀ�͸�
	int nWidth = pImgSrc->width;
	int nHeight = pImgSrc->height;
	//��ת�����ͼ�ߴ�
	int nWidthRotate= int(nHeight * fabs(lSinAngle) + nWidth * fabs(lCosAngle));
	int nHeightRotate=int(nWidth * fabs(lSinAngle) + nHeight * fabs(lCosAngle));
	// ������ת���ͼ��
	IplImage* pImgRotate = cvCreateImage(cvSize(nWidthRotate, nHeightRotate), pImgSrc->depth, pImgSrc->nChannels);
	cvZero(pImgRotate);
	// ��֤ԭͼ��������Ƕ���ת����С�ߴ�
	int tempLength = sqrt((double)nWidth * nWidth + (double)nHeight * nHeight) + 10;
	int tempX = (tempLength + 1) / 2 - nWidth / 2;
	int tempY = (tempLength + 1) / 2 - nHeight / 2;
	// ������ʱͼ��
	IplImage* pImgTemp = cvCreateImage(cvSize(tempLength, tempLength), pImgSrc->depth, pImgSrc->nChannels);
	cvZero(pImgTemp);

	// ��ԭͼ���Ƶ���ʱͼ��tmp����
	cvSetImageROI(pImgTemp, cvRect(tempX, tempY, nWidth, nHeight));
	cvCopy(pImgSrc, pImgTemp, NULL);
	cvResetImageROI(pImgTemp);

	//��ת����map
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]
	float m[6];
	int w = pImgTemp->width;
	int h = pImgTemp->height;
	m[0] = lCosAngle;
	m[1] = lSinAngle;
	m[3] = -m[1];
	m[4] = m[0];
	// ����ת��������ͼ���м�
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;
	CvMat M = cvMat(2, 3, CV_32F, m);
	// 
	cvGetQuadrangleSubPix(pImgTemp, pImgRotate, &M);
	cvReleaseImage(&pImgTemp);

	return pImgRotate;
}