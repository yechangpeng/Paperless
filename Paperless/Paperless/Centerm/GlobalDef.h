#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

#include "PrjDef.h"

/*!<data type define */
typedef unsigned char         U8;
typedef char                  S8;
typedef unsigned int          U32;
typedef int                   S32;
typedef S32 ( *EndCondition )( U8 *, S32 );

/*!<same useful marco define */
#define TABLESIZE(a)                                ( sizeof(a)/sizeof(a[0]) )
#define INVALID_DESCRIPTOR                          (-1)
#define MAX_LEN                                     (1024)
#define PACKET_LEN                                  (2048)
#define ICC_CMD_HAS_LE                              (0x1)
/*!<define return values */
#define RET_SUCCESS                                 (0)
#define ERR_SUCCESS                                 (0)
#define RET_ERROR                                   (-1)
#define RET_INVALID_PARAM                           (-2)
#define RET_FAIL_OPEN_FILE                          (-3)
#define RET_FAIL_TIMEOUT                            (-4)
#define RET_WRITE_ERROR                             (-5)
#define RET_READ_ERROR                              (-6)
#define RET_DEVICE_UNOPEN                           (-7)
#define RET_WRONG_PACKET                            (-8)
#define RET_CARDSLOT_ERROR                          (-9)
#define RET_NO_FOUND_SFI                            (-10)
#define RET_SFI_LEN_ERR                             (-11)
#define RET_MALLOC_ERR                              (-12)
#define RET_DOL_NO_FOUND                            (-13)
#define RET_DOL_DATA_ERR                            (-14)
#define RET_DOL_DATA_NO_FOUND                       (-15)
#define RET_APP_INIT_ERR                            (-16)
#define RET_LOG_NO_ENTRY                            (-17)
#define RET_RECORD_NO_FOUND                         (-18)
#define RET_GET_GAC_TOO_MUCH                        (-19)
#define RET_CMD_ERR                                 (-20)
#define RET_GAC_RESP_ERR                            (-21)
#define RET_AUTH_DATA_ERROR                         (-22)
#define RET_TRADE_ACC                               (-23)
#define RET_NO_SCRIPT                               (-24)
#define RET_WRITE_SCRIPT_ERR                        (-25)
#define RET_SCRIPT_CMD_ERR                          (-26)
#define  ERR_OPENFAILED         					(-27)//���豸ʧ��
#define  ERR_WRITE									(-28)//����ָ��ʧ��----���豸ͨѶʧ��
#define  ERR_READ									(-29)//��������ʧ��----���豸ͨѶʧ��
#define  ERR_PACKAGE_FORMAT							(-30)//���յ��ı��ĸ�ʽ����
#define  ERR_READ_CARD								(-31)//��������ʧ��
#define  ERR_WRITE_CARD								(-32)//д������ʧ��
#define  ERR_CANCELED           					(-33)//�û�ȡ��
#define  ERR_TIMEOUT            					(-34)//��ʱ
#define  ERR_PARAM              					(-35)//�����쳣
//IDcard���ؽ��
#define     ERR_DEVICE								-36    //�豸����
#define     ERR_RECV								-37    //���մ���
#define     ERR_SEND							    -38   //���ʹ���
#define     ERR_CHECK								-39    //У�����
#define     ERR_IMAGE        						-40    //����ͷ�����
#define     ERR_SAVEIMG      						-41    //����ͼ�����
#define     ERR_NO_CARD      						-42   //δ�ҵ���Ƭ
#define     ERR_OTHER        						-100  //��������


/*!<define Communication types */
#define TYPE_HID                          (0x0)   //hid
#define TYPE_COM                          (0x1)   //com
#define TYPE_PCSC                         (0x2)   //pc/sc
#define TYPE_USBLIB_HID              (0x03) //usblibapi
#define TYPE_AUX                     (0x04)
#define TYPE_SDT                     (0x05)

/*!<define some flags */
#define TERM_LOCK                        (0x1)
#define TERM_UNLOCK                      !TERM_LOCK
#define TERM_INIT_VALUE                  (0xAA)
#define DEFAULT_TIME_OUT                 (10)
#define DEFAULT_BAUD                     (9600)

/*!<define card reader types */
#define READER_CKB51XX                   (0x0)
#define READER_PCSC                      (0x1)


//������ 
//�Ӵ�ʽ��
#define  SMART_CARD_BASE                0x0
#define  SMART_CARD                     SMART_CARD_BASE      //�Ӵ�ʽ��
#define  SMART_CARD_MAX                 (SMART_CARD_BASE+0xF)//�Ӵ�ʽ�����Ŀ���
#define  IS_ICC_SLOT( a )               ( ( (a) >= SMART_CARD_BASE )&&( (a) <= SMART_CARD_MAX ) ? 1 : 0 )

 //�ǽӴ�ʽ��
#define PICC_CARD_SLOT                  (0xFF)

//PSAM��
#define  PSAM_CARD_BASE                 0x10
#define  PSAM_CARD                      PSAM_CARD_BASE      
#define  PSAM_CARD_MAX                  (PSAM_CARD_BASE+0xF)//PSAM�����Ŀ���
#define  IS_PSAM_SLOT( a )              ( ( (a) >= PSAM_CARD )&&( (a) <= PSAM_CARD_MAX ) ? 1 : 0 )



//GAC�������
#define CMD_GAC_ARQC    (1)
#define CMD_GAC_TC      (2)
#define CMD_GAC_AAC     (3)


//ʹ������ָ�
#define SOH						(0x01)
#define EOT						(0x04)
#define CENT_COMMAND_SEPARATOR	"\x20\x01\x7C"  //���÷ָ��,'|'
#define CENT_COMMAND_RD123		"\x20\x02"  //���ŵ�123
#define CENT_COMMAND_WT123		"\x20\x03"  //д�ŵ�123
#define CENT_COMMAND_TIMEOUT	"\x20\x04"  //���ó�ʱ


//����ſ���������
#define COMMAND_RESET         "\x1B\x30"//��λ
#define COMMAND_STATUS        "\x1B\x6A"//״̬��ѯ

#define COMMAND_ISO           "\x1B\x3D"//���ôŵ�ΪISO
#define COMMAND_IBM           "\x1B\x27"//���ôŵ���ʽΪIBM
#define COMMAND_ISO_D         "\x1B\x4F"//���ôŵ���ʽΪISO(�¿�)
#define COMMAND_IBM_D         "\x1B\x41"//���ôŵ���ʽΪIBM(�¿�)

//�����������
#define COMMAND_RD1           "\x1B\x72"
#define COMMAND_RD2           "\x1B\x5D"
#define COMMAND_RD3           "\x1B\x54\x5D"
#define COMMAND_RD12          "\x1B\x44\x5D"
#define COMMAND_RD23          "\x1B\x42\x5D"

//����д������
#define COMMAND_WT1           "\x1B\x77%s\x1D\x1B\x5C"
#define COMMAND_WT2           "\x1B\x74%s\x1D\x1B\x5C"
#define COMMAND_WT3           "\x1B\x74\x41%s\x1D\x1B\x5C"
#define COMMAND_WT12          "\x1B\x77%s\x41%s\x1D\x1B\x5C"
#define COMMAND_WT23          "\x1B\x74%s\x41%s\x1D\x1B\x5C"



//��������
#define  MAXLEN_TRCKDATA          512


//�ŵ�λ�ö���
#define  MODE_TRACK1             1
#define  MODE_TRACK2             2
#define  MODE_TRACK3             3
#define  MODE_TRACK12            12
#define  MODE_TRACK23            23
#define  MODE_TRACK123           123

#ifndef FALSE
#define FALSE  0
#endif

#ifndef TRUE 
#define TRUE  1
#endif

#ifndef NULL
#define NULL 0
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;

typedef struct 
{
  S32 nInitFlag;/*!< termparam init flag*/

  S32 nTransType;/*!< ͨ������*/
  S32 nCardReader;/*!< ����������*/
  
  S32 nPID;
  S32 nVID;
  
  S32 nCom;/*!<���ں�*/
  S32 nBaud;/*!<���ڲ�����*/
  
  #if defined( LINUX_EDITION )
  S32 nAuxNo;/*!<���ں�*/ 
  S32 nTermType;/*!<�ն�����*/  
  S32 nLockKeyboard;/*!<�Ƿ�������*/
  S8  szTermPath[256];
  #endif
  
  S32 nTimeOut;/*!<��ʱʱ������*/
  EndCondition lpEndCondition;/*!<���������ж�*/

  #if defined( USE_TYPE_PCSC )
  S8 szPcscReader[256];
  #endif

  //R20006 add by sulin ȫ�ֱ������
  U8 szTermData[1024];          //�γ�Tc��Ҫ�ĳ�ʼ������
  S32 nTermDataLen;
  
}TERMPARAM;

typedef struct 
{
	U8 szIccAppData[4096] ;
	U8 szAip[2+1];
	U32 nIccAppDataLen ;
	U8 szTcF55[4096];		//add by cjm
	U32 nTcF55Len;			//add by cjm
}ICCARDPARAM;


#define TAG_LEN     32
typedef struct
{
	char UsrTag[ TAG_LEN ];     //�û��ı�ǩ
	int  UsrTagLen;             //�û���ǩ����
	char OurTag[ TAG_LEN ];     //�����Լ��ı�ǩ
	int OurTagLen;              //��ǩ����
} TAGLIST;

#endif


