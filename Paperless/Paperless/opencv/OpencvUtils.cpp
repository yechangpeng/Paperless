#include "OpencvUtils.h"
#include "../MyTTrace.h"
#include <math.h>


/** 
* ���ܣ��Զ���ѡ��ԭͼ�ڵ����֤
* ������pImgSrc������ȡ��ԭͼ��
*		pImgDest����ȡ�������֤ͼ�񣬱����ֶ��ͷ�
* ����ֵ��0-�ɹ�������-ʧ��
**/
int MyAutoCrop(IplImage *pImgSrc, IplImage **pImgDest)
{
#define SHOW_IMAGE 0
	GtWriteTrace(30, "%s:%d: ���뵽���֤��ѡ������", __FUNCTION__, __LINE__);
	if (pImgSrc == NULL)
	{
		GtWriteTrace(30, "%s:%d: �ֶ��ñߺ���ʧ�ܣ�ԭͼ��ΪNULL", __FUNCTION__, __LINE__);
		return -1;
	}
	// ��⵽�������е���������С
	double lMaxArea = 0;
	int nMaxAreaIndex = 0;
	// ԭͼ��Mat��ʹ��Mat��Ϊ�����ͼ����Ч�ʣ������ֶ��ͷ�
	cv::Mat srcImg;
	// �Ҷ�ͼ��Mat���µ�ͼ��
	cv::Mat grayImg;
	// ��ֵ�˲�ͼ��ǳ�������ڻҶ�ͼ��ֱ�Ӵ���
	cv::Mat avgFilterImg;
	// ��ֵ��ͼ��
	cv::Mat binImg;
	// ������������������
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	// ��������λ��
	int index = 0;
	// ��һ�βü���ͼ�񣬼��ҵ�����������Ӿ��βü���ͼ��
	IplImage* pFirstCutImg = NULL;
	// ��ת���ͼ���ڵ�һ�βü������ϣ�������������תˮƽƫ�ƽǵõ�
	IplImage* pRemoteImg = NULL;

	// IplImage* ת cv::Mat��ֻ��ǳ��������Ϊֻ���ȡԭͼ����
	srcImg = pImgSrc;
#if SHOW_IMAGE
	cv::imshow("ԭͼ��", srcImg);
	cvWaitKey(0);
	cv::destroyWindow("ԭͼ��");
#endif
	// �Ҷȴ���
	cvtColor(srcImg, grayImg, CV_RGB2GRAY);
#if SHOW_IMAGE
	imshow("�Ҷ�ͼ", grayImg);
	cvWaitKey(0);
	cv::destroyWindow("�Ҷ�ͼ");
#endif
	// 3*3��ֵ�˲�ͼ�񣬴ӻҶ�ͼ����ǳ��������Ҷ�ͼָ��ͬһ�����ݣ�֮������ֵ�˲�����
	avgFilterImg = grayImg;
	blur(grayImg, avgFilterImg, cv::Size(3, 3));
#if SHOW_IMAGE
	imshow("��ֵ�˲�ͼ��", avgFilterImg);
	cvWaitKey(0);
	cv::destroyWindow("��ֵ�˲�ͼ��");
#endif
	// ��ֵ��ͼ�񣬴Ӿ�ֵ�˲�ͼ������ǳ���������ֵ�˲�ͼ��ָ��ͬһ�����ݣ�֮������ֵ������
	binImg = avgFilterImg;
	threshold(avgFilterImg, binImg, 0, 255, CV_THRESH_OTSU);
#if SHOW_IMAGE 
	imshow("��ֵ��ͼ��", avgFilterImg);
	cvWaitKey(0);
	cv::destroyWindow("��ֵ��ͼ��");
#endif

	// ��Ե��⴦��
	cv::findContours(binImg, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	// �ж��Ƿ��⵽����
	if (contours.size() == 0 || hierarchy.size() == 0)
	{
		GtWriteTrace(30, "%s:%d: δ��⵽����ͼ��111111��", __FUNCTION__, __LINE__);
		return -2;
	}
	// �����������ҵ�������������
	for (; index>=0; index = hierarchy[index][0])
	{
		double lArea = cv::contourArea(contours[index]);
		if (lMaxArea < lArea)
		{
			// ����������ֵ����vectot�������е�λ��
			lMaxArea = lArea;
			nMaxAreaIndex = index;
		}
	}
	// �ٴ��ж��Ƿ��⵽����
	if (index == 0 && contours.size() == 0 && hierarchy.size() == 0)
	{
		// δ��⵽����
		GtWriteTrace(30, "%s:%d: δ��⵽����ͼ��222222��", __FUNCTION__, __LINE__);
		return -1;
	}
	double lArea = cv::contourArea(contours[nMaxAreaIndex]);
	if (lMaxArea == lArea)
	{
		GtWriteTrace(30, "%s:%d: ������⵽������������=[%.6lf]��\n", __FUNCTION__, __LINE__, lMaxArea);
		// ��ȡ������Ӿ���
		cv::Rect normalRect=cv::boundingRect(contours[nMaxAreaIndex]);

#if SHOW_IMAGE 
		// ����������������Ӿ��Σ�����ʹ��
		cv::Mat dstImg = srcImg.clone();
		cv::Scalar color1(rand()&255, rand()&255, rand()&255);
		cv::Scalar color2(rand()&255, rand()&255, rand()&255);
		//cv::Mat dstImg = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC3);
		cv::drawContours(dstImg, contours, index, color2, 1, 8, hierarchy);
		// cvRectangle���������� ͼƬ�� ���Ͻǣ� ���½ǣ� ��ɫ�� ������ϸ�� �������ͣ�������    
		rectangle(dstImg, normalRect.tl(), normalRect.br(), color1, 1, 4, 0 );
		imshow("������������Ӿ���", dstImg);
		cvWaitKey(0);
		cv::destroyWindow("������������Ӿ���");
#endif

		// ��ȡ��С��Ӿ���
		cv::RotatedRect minRect  = cv::minAreaRect(contours[nMaxAreaIndex]);
		float minRectWidth = minRect.size.width;
		float minRectHeight = minRect.size.height;
		GtWriteTrace(30, "%s:%d: ��С��Ӿ��Σ�ˮƽƫ�ƽ�=[%.6lf] ��С=[%f][%f]��\n", __FUNCTION__, __LINE__, minRect.angle, minRectWidth, minRectHeight);

		// ������һ�ü���ͼ��
		pFirstCutImg = cvCreateImage(cvSize(normalRect.width, normalRect.height), pImgSrc->depth, pImgSrc->nChannels);
		// ����ԭͼ��ROI����һ�βü�������Ӿ���
		cvSetImageROI(pImgSrc, normalRect);
		// �ü�ͼ�񣨸���ͼ��
		cvCopy(pImgSrc, pFirstCutImg, NULL);
		// ԭͼ����������ROI
		cvResetImageROI(pImgSrc);
#if SHOW_IMAGE 
		cvShowImage("��һ�βü����ͼ��", pFirstCutImg);
		cvWaitKey(0);
		cvDestroyWindow("��һ�βü����ͼ��");
#endif
		// ����ת�ĽǶȣ�ȡ����ֵ
		float degree = fabs(minRect.angle);
		// �ж���ת�ķ���
		if (((90 - degree) < 0.0001) || ((degree - 0) < 0.0001))
		{
			// �Ƕ�ָ�������ߺ�ˮƽ�н�
			// �����0�ȣ��ҿ�<�ߣ���ת90�ȣ������90�ȣ��ҿ�>�ߣ���ת90��
			if ( ((minRectWidth < minRectHeight) && ((degree - 0) < 0.0001)) || 
				((minRectWidth > minRectHeight) && ((90 - degree) < 0.0001)) )
			{
				degree = -90;
				pRemoteImg = rotateImage(pFirstCutImg, degree);
			}
			else
			{
				//pRemoteImg = pFirstCutImg;
				pRemoteImg = cvCloneImage(pFirstCutImg);
			}
		}
		else
		{
			// �� 0~90 ��֮��
			// ͨ�����ο�Ⱥ͸߶��ж���ת����
			if (minRectWidth < minRectHeight)
			{
				degree = degree - 90;
				pRemoteImg = rotateImage(pFirstCutImg, degree);
			}
			else
			{
				pRemoteImg = rotateImage(pFirstCutImg, degree);
			}
		}
		// �ж�ͼ����ת�Ƿ�ʧ���Ƿ�ɹ�
		if (pRemoteImg == NULL)
		{
			GtWriteTrace(30, "%s:%d: ͼ����ת����ʧ�ܣ�\n", __FUNCTION__, __LINE__);
			goto end;
		}
		// �ͷŵ�һ�βü���ͼ��
		if (pFirstCutImg != NULL)
		{
			cvReleaseImage(&pFirstCutImg);
			pFirstCutImg = NULL;
		}
#if SHOW_IMAGE
		cvShowImage("��ת���ͼ��", pRemoteImg);
		cvWaitKey(0);
		cvDestroyWindow("��ת���ͼ��");
#endif
		// ��С��Ӿ��ε��������ߣ��� > �ߣ���Ҳ���ǵڶ��βü�ͼ��Ŀ�͸�
		float saveWidth = minRectWidth > minRectHeight ? minRectWidth : minRectHeight;
		float saveHeight = minRectWidth < minRectHeight ? minRectWidth : minRectHeight;
		// �����ڶ��βü���ͼ�񣨽��ͼ��
		*pImgDest = cvCreateImage(cvSize(saveWidth, saveHeight), pRemoteImg->depth, pRemoteImg->nChannels);
		// �ü�ͼ��Ĵ�С
		CvRect cutRect = cvRect( (pRemoteImg->width - saveWidth) / 2, (pRemoteImg->height - saveHeight) / 2,
			saveWidth, saveHeight);
		// ����ROI
		cvSetImageROI(pRemoteImg, cutRect);
		// �ڶ��βü��õ����ͼ��ROI���ƣ�
		cvCopy(pRemoteImg, *pImgDest, NULL);
		// ���ROI
		cvResetImageROI(pRemoteImg);
		// �ͷ���ת���ͼ��
		if (pRemoteImg != NULL)
		{
			cvReleaseImage(&pRemoteImg);
			pRemoteImg = NULL;
		}
		//cvSaveImage("C:\\Users\\Administrator\\Desktop\\dest1.jpg", pImgDest);
#if SHOW_IMAGE
		cvShowImage("������ͼ��", *pImgDest);
		cvWaitKey(0);
		cvDestroyWindow("������ͼ��");
#endif
	}
end:
	// �ͷ��ڴ�
	if (pFirstCutImg != NULL)
	{
		cvReleaseImage(&pFirstCutImg);
		pFirstCutImg = NULL;
	}
	if (pRemoteImg != NULL)
	{
		cvReleaseImage(&pRemoteImg);
		pRemoteImg = NULL;
	}
	GtWriteTrace(30, "%s:%d: ��ѡͼ���ڵ����֤���������\n", __FUNCTION__, __LINE__);
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


// ��������
int round_double(float number)
{
	return (number > 0.0) ? (number + 0.5) : (number - 0.5); 
}


/** 
* ���ܣ���ȡͼ���������򣬰�����Ϳ�߱�����ȡ��ͼ���ͼƬ�ļ��л�ȡ
* ������pFilePath������ת��ԭͼ���·��
*		fAreaRate����������������/С��������������1
*		fHeightWidthRate���߿�ȣ���/�����������1
* ����ֵ����ȡ���ͼ��ΪNULL��ʾʧ�ܣ���Ϊ����Ҫ�ֶ��ͷ�
**/
IplImage* GetCentreOfImage2(const char *pFilePath, float fAreaRate, float fHeightWidthRate)
{
	IplImage *pSrcImg = cvLoadImage(pFilePath, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	if (pSrcImg == NULL)
	{
		return NULL;
	}
	IplImage *pDestImg = GetCentreOfImage1(pSrcImg, fAreaRate, fHeightWidthRate);
	if (pDestImg == NULL)
	{
		cvReleaseImage(&pSrcImg);
		return NULL;
	}
	return pDestImg;
}


/** 
* ���ܣ���ȡͼ���������򣬰�����Ϳ�߱�����ȡ��ͼ���image�л�ȡ
* ������pImgSrc������ȡ��ԭͼ��
*		fAreaRate����������������/С��������������1
*		fHeightWidthRate���߿�ȣ���/�����������1
* ����ֵ����ȡ���ͼ��ΪNULL��ʾʧ�ܣ���Ϊ����Ҫ�ֶ��ͷ�
**/
IplImage* GetCentreOfImage1(IplImage* pImgSrc, float fAreaRate, float fHeightWidthRate)
{
	if (pImgSrc == NULL || fAreaRate <= 1 || fHeightWidthRate <= 1)
	{
		return NULL;
	}

	IplImage *pImgDest = NULL;
	// ��ͼ�����
	long dBigArea = 0;
	// Сͼ�����
	double dLittleArea = 0;
	// Сͼ����
	float fTimes = 0;
	int nWidth = 0;
	int nHeight = 0;
	int nX = 0;
	int nY = 0;

	// ��������
	dBigArea = pImgSrc->height * pImgSrc->width;
	// ����С���
	dLittleArea = dBigArea / fAreaRate;
	// ͨ��������߱ȼ��㱶��
	fTimes = sqrt(dLittleArea / fHeightWidthRate);
	nWidth = round_double(fTimes);
	nHeight = round_double(fTimes * fHeightWidthRate);
	GtWriteTrace(30, "%s:%d: ԭͼ���=[%d], ��=[%d]����һ�μ���Ŀ�=[%d], ��=[%d]��", __FUNCTION__, __LINE__,
		pImgSrc->width, pImgSrc->height, nWidth, nHeight);

	// �жϸߡ����Ƿ񳬹�ԭͼ��ĸߡ����������ȡ������
	if (nWidth > pImgSrc->width)
	{
		GtWriteTrace(30, "%s:%d: ����Ŀ�[%d] > ԭͼ���=[%d]��", __FUNCTION__, __LINE__, nWidth,
			pImgSrc->width);
		// ��ֵ����
		nWidth = pImgSrc->width;
		// ���¼����
		nHeight = round_double(nWidth * fHeightWidthRate);
		nX = 0;
		nY = (pImgSrc->height - nHeight) / 2;
	}
	else if (nHeight > pImgSrc->height )
	{
		GtWriteTrace(30, "%s:%d: ����ĸ�[%d] > ԭͼ���=[%d]��", __FUNCTION__, __LINE__, nHeight,
			pImgSrc->height);
		// ��ֵ����
		nHeight = pImgSrc->height;
		// ���¼����
		nWidth = round_double(nHeight / fHeightWidthRate);
		nX = (pImgSrc->width - nWidth) / 2;
		nY = 0;
	}
	else
	{
		nX = (pImgSrc->width - nWidth) / 2;
		nY = (pImgSrc->height - nHeight) / 2;
	}
	GtWriteTrace(30, "%s:%d: ����Ĵ���ȡ���������(%d, %d)����=[%d], ��=[%d]��", __FUNCTION__, __LINE__,
		nX, nY, pImgSrc->width, pImgSrc->height);

	// ��ȡͼ��
	pImgDest = cvCreateImage(cvSize(nWidth, nHeight), pImgSrc->depth, pImgSrc->nChannels);
	// ��ȡͼ�������
	CvRect cutRect = cvRect( nX, nY, nWidth, nHeight);
	// ����ROI
	cvSetImageROI(pImgSrc, cutRect);
	// �ڶ��βü��õ����ͼ��ROI���ƣ�
	cvCopy(pImgSrc, pImgDest, NULL);
	// ���ROI
	cvResetImageROI(pImgSrc);

	return pImgDest;
}