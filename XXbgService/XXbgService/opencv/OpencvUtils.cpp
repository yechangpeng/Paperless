#include "OpencvUtils.h"
#include "../MyTTrace.h"

/** 
* 功能：自动框选出原图内的身份证
* 参数：pImgSrc：待提取的原图像
*		pImgDest：提取出的身份证图像，必须手动释放
* 返回值：0-成功，其他-失败
**/
int MyAutoCrop(IplImage *pImgSrc, IplImage **pImgDest)
{
	if (pImgSrc == NULL)
	{
		GtWriteTrace(30, "%s:%d: 手动裁边函数，原图像为NULL");
		return NULL;
	}
	// 存储轮廓的容器
	CvMemStorage* storage = cvCreateMemStorage(0);
	// 原图像灰度图，及之后二值化的图像
	IplImage* pImgGray = NULL;
	// 指向轮廓的指针
	CvSeq* contour = NULL;
	CvSeq* contourTmp = NULL;
	// 提取的所有轮廓中面积最大的轮廓
	double lMaxArea = 0;
	// 过滤提取的轮廓，小于此面积区域不处理
	double lMinArea = 1000.0;

// 	cvNamedWindow("灰度图二值化后的图像", 1);
// 	cvNamedWindow("裁剪后的图像", 1);
// 	cvNamedWindow("旋转后的图像", 1);
// 	cvNamedWindow("保存后的图像", 1);

	// 原图转换为灰度图像
	pImgGray = cvCreateImage(cvGetSize(pImgSrc), 8, 1);
	cvZero(pImgGray);
	cvCvtColor(pImgSrc, pImgGray, CV_BGR2GRAY);

	// 原图像灰度图进行二值化（其中阀值可能需要修改）
	cvThreshold(pImgGray, pImgGray, 130, 255, CV_THRESH_BINARY);

	// 显示二值化后的原图像
// 	cvShowImage("灰度图二值化后的图像", pImgGray);
// 	cvWaitKey(0);


	// 灰度图进行二值化后，提取轮廓
	int contour_num = cvFindContours(pImgGray, storage, &contour, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	contourTmp = contour;
	// 先遍历一次，获取最外层同级最大面积的那一块区域面积，保存面积大小
	for( ; contourTmp != 0; contourTmp = contourTmp->h_next )
	{
		double lTmpArea = fabs(cvContourArea(contourTmp));
		if (lTmpArea < lMinArea)
		{
			// 去除小面积区域
			cvSeqRemove(contourTmp, 0);
			continue;
		}
		if(lTmpArea > lMaxArea)
		{
			lMaxArea = lTmpArea;
		}
	}
	// 指针指回原处，遍历，处理最大面积的轮廓
	contourTmp = contour;
	for(; contourTmp != 0; contourTmp = contourTmp->h_next)
	{
		double lTmpArea = fabs(cvContourArea(contourTmp));
		if (lTmpArea == lMaxArea)
		{
			// 最大面积的轮廓
			// 获取正常外接矩形
			CvRect rect = cvBoundingRect(contourTmp,1);
			CvRect rectTmp = cvRect(rect.x, rect.y, rect.width, rect.height);
			// 创建第一裁剪的图像
			IplImage* pDestCut = cvCreateImage(cvSize(rectTmp.width, rectTmp.height), pImgSrc->depth, pImgSrc->nChannels);
			// 设置源图像ROI，第一次裁剪正常外接矩形，之后再旋转最小外接矩形
			cvSetImageROI(pImgSrc, rectTmp);
			//复制图像
			cvCopy(pImgSrc, pDestCut, NULL); 
			//源图像用完后，清空ROI
 			cvResetImageROI(pImgSrc);
// 			cvShowImage("裁剪后的图像", pDestCut);
// 			cvWaitKey(0);

			// 旋转后的图像
			IplImage* pDestRemote = NULL;
			// 获得最小外接矩形
			CvBox2D box2D  = cvMinAreaRect2(contourTmp);
			// 待旋转的角度，取绝对值
			float degree = fabs(box2D.angle);
			GtWriteTrace(30, "%s:%d: 参数: degree = [%.3f] size[%.3f][%.3f]", __FUNCTION__, __LINE__, degree, box2D.size.width,
				box2D.size.height);
			if (((90 - degree) < 0.0001) || ((degree - 0) < 0.0001))
			{
				// 角度指宽那条边和水平夹角
				// 如果是0度，且宽<高，旋转90度；如果是90度，且宽>高，旋转90度
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
				// 在 0~90 度之间
				// 通过矩形宽度和高度判断旋转方向
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
				GtWriteTrace(30, "%s:%d: 获取旋转后的图像失败！", __FUNCTION__, __LINE__);
				return NULL;
			}
			// 释放第一次裁剪的图像
			if (pDestRemote != pDestCut)
			{
				cvReleaseImage(&pDestCut);
			}
// 			cvShowImage("旋转后的图像", pDestRemote);
// 			cvWaitKey(0);
			// 最小外接矩形的真正宽、高（宽 > 高）
			float saveWidth = box2D.size.width > box2D.size.height ? box2D.size.width : box2D.size.height;
			float saveHeight = box2D.size.width < box2D.size.height ? box2D.size.width : box2D.size.height;
			// 创建第二次裁剪的图像
			*pImgDest = cvCreateImage(cvSize(saveWidth, saveHeight), pDestRemote->depth, pDestRemote->nChannels);
			// 裁剪图像的大小，各边多1像素
			CvRect mSaveRect = cvRect( (pDestRemote->width - saveWidth) / 2, (pDestRemote->height - saveHeight) / 2,
				saveWidth, saveHeight);
			// 设置旋转后图像的roi
			cvSetImageROI(pDestRemote, mSaveRect);
			// 拷贝到目标图像中
			cvCopy(pDestRemote, *pImgDest, NULL);
			// 释放roi
			cvReleaseImage(&pDestRemote);
// 			cvShowImage("保存后的图像", *pImgDest);
// 			cvWaitKey(0);
		}
	}

	//cvWaitKey(0);
// 	cvDestroyWindow("灰度图二值化后的图像");
// 	cvDestroyWindow("裁剪后的图像");
// 	cvDestroyWindow("旋转后的图像");
// 	cvDestroyWindow("保存后的图像");
	cvReleaseImage(&pImgGray);
	//释放内存
	cvReleaseMemStorage(&storage);
	return 0;
}

/** 
* 功能：不改变大小，旋转图像
* 参数：pImgSrc：待旋转的原图像
*		degree：待旋转的角度（弧度制），正数表示逆时针，负数表示顺时针
* 返回值：旋转后的图像（使用后需要手动释放），为NULL表示失败
**/
IplImage* rotateImage(IplImage* pImgSrc, float lDegree)
{
	// 弧度制角转换成角度制
	double lAngle = lDegree  * CV_PI / 180.;
	// 角度的sin和cos值
	double lSinAngle = sin(lAngle);
	double lCosAngle = cos(lAngle);
	// 原图的宽和高
	int nWidth = pImgSrc->width;
	int nHeight = pImgSrc->height;
	//旋转后的新图尺寸
	int nWidthRotate= int(nHeight * fabs(lSinAngle) + nWidth * fabs(lCosAngle));
	int nHeightRotate=int(nWidth * fabs(lSinAngle) + nHeight * fabs(lCosAngle));
	// 建立旋转后的图像
	IplImage* pImgRotate = cvCreateImage(cvSize(nWidthRotate, nHeightRotate), pImgSrc->depth, pImgSrc->nChannels);
	cvZero(pImgRotate);
	// 保证原图可以任意角度旋转的最小尺寸
	int tempLength = sqrt((double)nWidth * nWidth + (double)nHeight * nHeight) + 10;
	int tempX = (tempLength + 1) / 2 - nWidth / 2;
	int tempY = (tempLength + 1) / 2 - nHeight / 2;
	// 建立临时图像
	IplImage* pImgTemp = cvCreateImage(cvSize(tempLength, tempLength), pImgSrc->depth, pImgSrc->nChannels);
	cvZero(pImgTemp);

	// 将原图复制到临时图像tmp中心
	cvSetImageROI(pImgTemp, cvRect(tempX, tempY, nWidth, nHeight));
	cvCopy(pImgSrc, pImgTemp, NULL);
	cvResetImageROI(pImgTemp);

	//旋转数组map
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]
	float m[6];
	int w = pImgTemp->width;
	int h = pImgTemp->height;
	m[0] = lCosAngle;
	m[1] = lSinAngle;
	m[3] = -m[1];
	m[4] = m[0];
	// 将旋转中心移至图像中间
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;
	CvMat M = cvMat(2, 3, CV_32F, m);
	// 
	cvGetQuadrangleSubPix(pImgTemp, pImgRotate, &M);
	cvReleaseImage(&pImgTemp);

	return pImgRotate;
}