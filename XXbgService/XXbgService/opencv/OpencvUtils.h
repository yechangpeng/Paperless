// opencv ���ߺ���
#include "../stdafx.h"
#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"


/** 
* ���ܣ��Զ���ѡ��ԭͼ�ڵ����֤
* ������pImgSrc������ȡ��ԭͼ��
*		pImgDest����ȡ�������֤ͼ�񣬱����ֶ��ͷ�
* ����ֵ��0-�ɹ�������-ʧ��
**/
int MyAutoCrop(IplImage *pImgSrc, IplImage **pImgDest);

/** 
* ���ܣ��Զ���ѡ��ԭͼ�ڵ����֤
* ������pImgSrc������ȡ��ԭͼ��
*		pImgDest����ȡ�������֤ͼ�񣬱����ֶ��ͷ�
* ����ֵ��0-�ɹ�������-ʧ��
**/
int MyAutoCrop1(IplImage *pImgSrc, IplImage **pImgDest);

/** 
* ���ܣ����ı��С����תͼ��
* ������pImgSrc������ת��ԭͼ��
*		fDegree������ת�ĽǶȣ������ƣ���������ʾ��ʱ�룬������ʾ˳ʱ��
* ����ֵ����ת���ͼ��ΪNULL��ʾʧ��
**/
IplImage* rotateImage(IplImage* pImgSrc, float fDegree);
