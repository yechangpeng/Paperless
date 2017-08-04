////////////////////////////////////////////////////////////////////////////
//���ߣ�  guocuiping
//�ļ�����IDCardDef.h
//������  ���巵��ֵ
//���ڣ�  2013.03.05
////////////////////////////////////////////////////////////////////////////
#ifndef   _IDCARD_DEF_
#define   _IDCARD_DEF_

#include "GlobalDef.h"


//��������Ҫ��ȡ�����֤ͼƬ���Ͷ���
#define		IMG_HEAD			0
#define		IMG_FRONT			1
#define		IMG_BACK			2
#define		IMG_IDCARD          3
#define     IMG_FRONT_LOGO      4

//������Ϣ
typedef struct PERSONINFO
{
	char name[80];      //��FZ����������  30�ֽ�  
	char EngName[120];  //��F��Ӣ������  120�ֽ�
	char version[10];   //��F��֤���汾�� 4�ֽ�
	char govCode[10];   //��F����������������ش���  8�ֽ�
	char cardType[10];  //��F��֤�����ͱ�־ 2�ֽ�
	char otherData[10]; //��F��Ԥ����  6�ֽ�
	char sex[10];       //[Z ͨ���Ա����ת�����Ա�]
	char nation[50];    //[Z ͨ�������Ž���ת��]
	char birthday[30];  //��FZ����������   16�ֽ�
	char address[180];  //��Z����ס��ַ 70�ֽ�
	char cardId[50];    //��FZ�����þ���֤����  30�ֽ�   �����֤��36���ֽڡ�
	char police[80];    //��Z���ɳ�����ַ 30�ֽ�
	char validStart[30];//��FZ��֤��ǩ��ʱ��  16�ֽ�
	char validEnd[30];  //��FZ��֤����ֹʱ��  16�ֽ�
	char sexCode[10];   //��FZ���Ա�  2�ֽ�
	char nationCode[10];//��FZ���������ߵ�������   6���ֽ�  [���� 4�ֽ�]
	char appendMsg[180];//��Z��������Ϣ 70�ֽ�
	int  iFlag;    

}PERSONINFO;


#endif