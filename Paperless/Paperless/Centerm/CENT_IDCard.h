#ifndef _CENT_IDCARD_H_
#define _CENT_IDCARD_H_

#include "GlobalDef.h"
#include "IDCardDef.h"

#ifndef _STDOUT
#define _STDOUT _stdcall 
#endif

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************************
* ��������CT_ReadIDCard
*  
* ���ܣ�  ������֤
*  
*������   pszDevice[in]-----ͨѶ�豸,SDT,HID,PCSC,Window�´���ΪCOM1...
*         BpNo[in]------��չ�ڣ�NULL��""������չ�ڣ���������չ��
*         nBaud[in]-----������
*         nTimeOut[in]--��ʱ����λ��s
*         szHeadDir[in]--ͷ�񱣴�Ŀ¼��ȡֵΪNULL�򡰡�ʱȡ������ͼƬ
*         pInfo[out]----����֤������Ϣ
*  
*����ֵ�� ERR_SUCCES---�ɹ�
*         ERR_DEVICE---�豸����
*         ERR_SEND-----���ʹ���
*         ERR_RECV-----���մ���
*         ERR_TIMEOUT--��ʱ
*         ERR_CHECK----У�����
*         ERR_NOCARD---�޶���֤
************************************************************************************/
long _STDOUT CT_ReadIDCard( const char *pszDevice, 
							const char *szBpNo,
							const long  nBaud,
							const long  nTimeOut, 
							char* szHeadDir, 
							PERSONINFO* pInfo );


long _STDOUT CT_ReadIDCardForeigner( const char *pszDevice, 
									const char* szBpNo,
									const long  nBaud,
									const long  nTimeOut, 
									char* szHeadDir, 
									PERSONINFO* pInfo );

//������ʹ��
long _STDOUT CT_SaveFrontImg( const PERSONINFO* pInfo, char* szFrontImg );

long _STDOUT CT_SaveFrontImgEx( const PERSONINFO* pInfo, char* szFrontImg, char *pLogoMsg, int  nMakeLogo );

//������ʹ��
long _STDOUT CT_SaveBackImg( const PERSONINFO* pInfo, char* szBackImg );


//������ʹ��
long _STDOUT CT_SaveCardImg( const PERSONINFO* pInfo, char* szCardImg, BOOL bMakeLogo );


//������ʹ��
long _STDOUT CT_SaveHeadImg( const PERSONINFO* pInfo, char* szHeadImg );


 //��ˮӡ��ѡ
long _STDOUT CT_SaveImg( int nImgType, BOOL nMaker, const PERSONINFO* pInfo, char* szImg );


long _STDOUT CT_GetImgBase64( int nImgType, const PERSONINFO* pInfo, char* szBase64, int nBase64 );


long _STDOUT CT_CancelRead( );


#ifdef __cplusplus
}
#endif


#endif //_CENT_IDCARD_H_