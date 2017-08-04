/******************************************************************************************
* 
*  Copyright(C), 2002-2013, Centerm Information Co.,Ltd,All Rights Reserved
*
*  FileName: PrjDef.h
*
*  Author:  quexinsheng
*
*  Date:   2014/1/7 14:47:01
*
*  Description: ��Ŀ�Զ���
*
*  History:     
*               
*     <flag>   <author>       <date>          <version>     <description>
*******************************************************************************************/
/*!<
           ͨ�ŷ�ʽ˵��
Ŀǰ��PBOC2.0v1.10�Ĵ���֧�ֵ�ͨ�ŷ�ʽ��
window�����У�
COM_WIN_MODE         ����ͨ�ŷ�ʽ
HID_WIN_MODE         hidͨ�ŷ�ʽ
PCSC_WIN_MODE        smartcardͨ�ŷ�ʽ
Linux�����У�        
COM_LINUX_MODE       ����ͨ�ŷ�ʽ
��Щ����ͨ��_LINUX_����ƣ�δ����������window����
*/

#ifndef _SELF_DEF_H_
#define _SELF_DEF_H_

#define USE_READER_CKB51XX
#define USE_READER_PCSC
#define WINDOWS_EDITION	          //windows����

#define USE_TYPE_PCSC
#define USE_TYPE_HID
#define USE_TYPE_COM              //����ͨ�ŷ�ʽ
#define DEBUG_OUTPUT            //�򿪵��� 
#define REG_DEBUG_FLG           //ʹ��ע������DEBUG

#define BANGK                   "XX����"

//����usb������
#define  HID_VID              0x0403
#define  HID_PID              0xBCD7//0xBCD5
#define  HID_PID_BCD5   0xBCD5
#define  HID_PID_BCD7   0xBCD7

typedef struct ST_HID
{
	int pid;
	int vid;
}HID;

#define  BUAD_RATE            115200 //����Ĭ��ͨѶ������
#define  BP_CMD               "\x1B\x25" //ת��ָ��
#define  ICC_PORT_25_9600     "\x1b\x25\x49\x34"
#define  ICC_DEFAULT_25_PORT  "\x1b\x25\x42"
#define  ICC_PORT_24_9600     "\x1b\x24\x49\x34"
#define  ICC_DEFAULT_24_PORT  "\x1b\x24\x42"

//��������꣬��Ĭ��ʹ��A000000333�������ӿ�Ƭ�л�ȡ
//#define  SPEED_UP_NO_GET_AID_FROM_CARD

//��55������������Ӧ��pan��Ӧ��pan���к�
#define ARQC_MORE_TAG

//�ⲿ��֤ʧ���Ƿ��˳�
//#define  EX_AUTH_FAIL_EXIT
#define  DEVICE_AUTH
#define AUTH_MODE

#ifndef LINUX_EDITION
#define _STDOUT _stdcall 
#include <windows.h>
#else
#define _STDOUT __attribute__  ((visibility("default"))) 
#endif

#endif



