#include "stdafx.h"
#include "CBaseReadIDCard.h"
#include "Centerm/CENT_IDCard.h"

#pragma once

/************************************************************************/
/* ���ڶ����� - ���֤ʶ����                                            */
/************************************************************************/
class CCentReader : public CBaseReadIDCardInfo
{
public:
	CCentReader() {}
	virtual ~CCentReader() {}
public:
	// ��д����ȡ���֤оƬ��Ϣ
	virtual int MyReadIDCardInfo(const char *pSaveHeadPicFilenm, MYPERSONINFO *pPersonInfo);
};