#include "stdafx.h"
#include "CBaseReadIDCard.h"
#include "CentermIFImp.h"

#pragma once

/************************************************************************/
/* ����һ��� - ����������ͷ                                            */
/************************************************************************/
class CCentOneCamera : public CBaseSaveDeskPic
{
public:
	CCentOneCamera();
	virtual ~CCentOneCamera();
public:
	// ��д����������ͷ��ȡ���֤��Ƭ
	virtual int MySaveDeskIDPic(const char *pSaveDesktopIDPicFilenm);
	// ��д����������ͷ��ȡ������Ƭ
	virtual int MySaveEnvPic(const char *pSaveEnvPicFilenm);

private:
	CStatic *pVideoWnd;
	CCentermIFImp *mHighCamera;
};