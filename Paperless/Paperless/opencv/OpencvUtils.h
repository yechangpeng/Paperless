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
* ���ܣ����ı��С����תͼ��
* ������pImgSrc������ת��ԭͼ��
*		fDegree������ת�ĽǶȣ������ƣ���������ʾ��ʱ�룬������ʾ˳ʱ��
* ����ֵ����ת���ͼ��ΪNULL��ʾʧ��
**/
IplImage* rotateImage(IplImage* pImgSrc, float fDegree);

/** 
* ���ܣ���ȡͼ���������򣬰�����Ϳ�߱�����ȡ��ͼ���image�л�ȡ
* ������pImgSrc������ȡ��ԭͼ��
*		fAreaRate����������������/С��������������1
*		fHeightWidthRate���߿�ȣ���/�����������1
* ����ֵ����ȡ���ͼ��ΪNULL��ʾʧ�ܣ���Ϊ����Ҫ�ֶ��ͷ�
**/
IplImage* GetCentreOfImage1(IplImage* pImgSrc, float fAreaRate, float fHeightWidthRate);

/** 
* ���ܣ���ȡͼ���������򣬰�����Ϳ�߱�����ȡ��ͼ���ͼƬ�ļ��л�ȡ
* ������pFilePath������ת��ԭͼ���·��
*		fAreaRate����������������/С��������������1
*		fHeightWidthRate���߿�ȣ���/�����������1
* ����ֵ����ȡ���ͼ��ΪNULL��ʾʧ�ܣ���Ϊ����Ҫ�ֶ��ͷ�
**/
IplImage* GetCentreOfImage2(const char *pFilePath, float fAreaRate, float fHeightWidthRate);
