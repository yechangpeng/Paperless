#include "stdafx.h"
#include "CBaseReadIDCard.h"
#include "Centerm/CENT_IDCard.h"

#pragma once

/************************************************************************/
/* ����һ��� - ���֤ʶ����                                            */
/************************************************************************/
class CCentOneReader : public CBaseReadIDCardInfo
{
public:
	CCentOneReader() {}
	virtual ~CCentOneReader() {}
public:
	// ��д����ȡ���֤оƬ��Ϣ
	virtual int MyReadIDCardInfo(const char *pSaveHeadPicFilenm, MYPERSONINFO *pPersonInfo);
};