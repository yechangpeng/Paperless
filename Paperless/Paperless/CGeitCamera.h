#include "CBaseReadIDCard.h"
#include <cv.h> 

#pragma once


/************************************************************************/
/* ���� - ����������ͷ                                            */
/************************************************************************/
class CGeitCamera : public CBaseSaveDeskPic
{
public:
	CGeitCamera();
	virtual ~CGeitCamera();
public:
	// ��д����������ͷ��ȡ���֤��Ƭ
	virtual int MySaveDeskIDPic(const char *pSaveDesktopIDPicFilenm);
	// ��д����������ͷ��ȡ���֤��Ƭ
	virtual int MySaveEnvPic(const char *pSaveEnvPicFilenm);

};