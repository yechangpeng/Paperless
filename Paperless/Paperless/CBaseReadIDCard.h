#pragma once


const int N_ID_CARD_CODE_COUNT = 9;
const char S_ID_CARD_CODE[][5] = 
	{"000",
	"200",
	"201",
	"202",
	"203",
	"204",
	"205",
	"206",
	"207"};
const char S_ID_CARD_MSG[][128] = 
	{"�ɹ�",
	"�����֤оƬͷ����ʧ��",
	"��ȡ���֤оƬͷ����ʧ��",
	"��ȡ���֤оƬͷ����ǰ�����ڴ�ʧ��",
	"δ��⵽���֤ʶ���ǣ�����Ƿ����������Լ������Ƿ���ȷ",
	"��ȡ���֤ʧ�ܣ�������֤�Ƿ�ź�",
	"��ȡ���֤����",
	"���֤У�����",
	"�������֤оƬͷ����ʧ��"};


const int N_READ_PIC_CODE_COUNT = 13;
const char S_READ_PIC_CODE[][5] = 
	{"000",
	"101",
	"102",
	"103",
	"104",
	"105",
	"106",
	"107",
	"108",
	"109",
	"110",
	"111",
	"112"};
const char S_READ_PIC_MSG[][128] = 
	{"�ɹ�",
	"�����֤��Ƭʧ��",
	"��ȡ���֤��Ƭʧ��",
	"��ȡ���֤��Ƭǰ�����ڴ�ʧ��",
	"δ��⵽����������ͷ",
	"�򿪸���������ͷʧ��",
	"���ø���������ͷ�ֱ���ʧ��",
	"���ø���������ͷ�Զ��ñ�ʧ��",
	"��������ȡ��Ƭʧ��",
	"�رո���������ͷʧ��",
	"�ֱ���ת��ʱ����ԴͼƬʧ��",
	"�ֱ���ת��ʱ����Ŀ��ͼƬʧ��",
	"�ֱ���ת��ʱ����Ŀ��ͼƬʧ��"};



// �Զ�������Ϣ
typedef struct S_MYPERSONINFO
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
}MYPERSONINFO;


/**
  * ���֤оƬ��Ϣ��ȡ����
 **/
class CBaseReadIDCardInfo
{
public:
	CBaseReadIDCardInfo(){}
	virtual ~CBaseReadIDCardInfo(){}
public:
	// ���麯������ȡ���֤оƬ��Ϣ
	virtual int MyReadIDCardInfo(const char *pSaveHeadPicFilenm, MYPERSONINFO *pPersonInfo) = 0;
};



/**
  * ������ ��������ͷ ͼƬ��ȡ����
 **/
class CBaseSaveDeskPic
{
public:
	CBaseSaveDeskPic():F_AREA_RATE((float)2.213), F_HEIGHT_WIDTH_RATE((float)((21 * 1.0) / 17)){}
	virtual ~CBaseSaveDeskPic() {}
public:
	const float F_AREA_RATE;
	const float F_HEIGHT_WIDTH_RATE;
	// ���麯���������� ��������ͷ ��ȡ���֤��Ƭ
	virtual int MySaveDeskIDPic(const char *pSaveDesktopIDPicFilenm) = 0;
	// ���麯���������� ��������ͷ ��ȡ ������Ƭ
	virtual int MySaveEnvPic(const char *pSaveEnvPicFilenm) = 0;
};



/**
  * ������ ��������ͷ ͼƬ��ȡ����
 **/
class CBaseSaveEnvPic
{
public:
	CBaseSaveEnvPic(){}
	virtual ~CBaseSaveEnvPic(){}
public:
	// ���麯���������� ��������ͷ ��ȡ ������Ƭ
	virtual int MySaveEnvPic(const char *pSaveEnvPicFilenm) = 0;
};