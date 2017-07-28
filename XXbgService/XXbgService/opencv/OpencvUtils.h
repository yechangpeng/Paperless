// opencv 工具函数
#include "../stdafx.h"
#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"


/** 
* 功能：自动框选出原图内的身份证
* 参数：pImgSrc：待提取的原图像
*		pImgDest：提取出的身份证图像，必须手动释放
* 返回值：0-成功，其他-失败
**/
int MyAutoCrop(IplImage *pImgSrc, IplImage **pImgDest);

/** 
* 功能：自动框选出原图内的身份证
* 参数：pImgSrc：待提取的原图像
*		pImgDest：提取出的身份证图像，必须手动释放
* 返回值：0-成功，其他-失败
**/
int MyAutoCrop1(IplImage *pImgSrc, IplImage **pImgDest);

/** 
* 功能：不改变大小，旋转图像
* 参数：pImgSrc：待旋转的原图像
*		fDegree：待旋转的角度（弧度制），正数表示逆时针，负数表示顺时针
* 返回值：旋转后的图像，为NULL表示失败
**/
IplImage* rotateImage(IplImage* pImgSrc, float fDegree);
