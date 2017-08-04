#ifndef __IOCP_H__
#define __IOCP_H__

#include <windows.h>
#include "singlelist.h"
#ifdef BUILD_IOCP_DLL
#define IOCP_DLL_EXPORT /*__declspec(dllexport)*/ __stdcall
#else
#define IOCP_DLL_EXPORT /*__declspec(dllimport)*/ __stdcall
#endif

#define MAX_IOCP_BUF_LEN       (32*1024)

typedef void (__stdcall *PFN_IO_EVENT)(LPVOID, HANDLE, PCHAR, DWORD);

// ������
typedef struct _IO_BUF
{
	union {
		char sUnionStruct[16+MAX_IOCP_BUF_LEN];
		struct {
			S_LIST_ENTRY         item;
			LONG volatile        ref_c;
			WSABUF               wsa_buf;
			int                  addr_len;
			struct sockaddr      addr;
			unsigned int         buf_len;
			char                 buf[MAX_IOCP_BUF_LEN];
		};
	};
} IO_BUF, *PIO_BUF;

// ��ɼ� �׽���״̬
typedef struct _IO_OP_KEY
{
	union {
		char sUnionStruct[32];
		struct {
			S_LIST_ENTRY         item;
			union
			{
				SOCKET           s;
				HANDLE           h;
			};
			LONG volatile        ref_c;                 // ���ô���
			LONG volatile        is_not_closed;         // �Ƿ��Ѿ��ر�
			BYTE                 key_type;              // ����
			BYTE                 is_link           :1;  // �������
			BYTE                 is_io_check       :1;  // ���ݳ�ʱ���
			BYTE                 is_l_w            :1;  // ��Ҫ����д
			BYTE                 is_close_call     :1;  // ���ùرջص�
			BYTE                 is_except_call    :1;  // �����쳣�ص�
			BYTE                 reserved1         :3;
			BYTE                 read_cout;             // ͬʱͶ�ݶ���
			BYTE                 is_limit_read_len;     // ��ȡ�������ƣ�0-�����ƣ�2-������ͷ���ƣ�1-���̶���������
			WORD                 io_timeout;            // ��ʱʱ��
			WORD                 remote_port;           // Զ�̶˿�
			DWORD                remote_addr;           // Զ��IP��ַ
			WORD                 last_error;
			WORD                 local_port;            // ���ض˿�
			DWORD                local_addr;            // ����IP��ַ
			clock_t              io_clock;              // ����ʱ��
			clock_t              read_clock;            // ��ȡ����ʱ��
			DWORD_PTR            read_limit;            // ��ȡ���Ƶĳ���
			PFN_IO_EVENT         fnOpenEvt;             // ��������
			PFN_IO_EVENT         fnCloseEvt;            // �ر�,һ�����ã��˽ṹ��������ʹ��
			PFN_IO_EVENT         fnReadEvt;             // ��ȡ
			PFN_IO_EVENT         fnWriteEvt;            // д�루������
			PFN_IO_EVENT         fnExceptEvt;           // �쳣
			LPVOID               lpParam;               // �¼�����
			LPVOID               lpReadParam;           // ��ȡ�¼�����
			LPVOID               lpUserParam;           // �û�����������
			DWORD volatile       s_r_c;                 // ���ɹ�������
			DWORD volatile       s_w_c;                 // ���ɹ�д����
			DWORD volatile       l_w_c;                 // ������д����
		};
	};
} IO_OP_KEY, *PIO_OP_KEY;

// �������
typedef struct _IO_OP_DATA
{
	union {
		char sUnionStruct[16];
		struct {
			S_LIST_ENTRY         item;
			OVERLAPPED           overlap;
			LONG                 op_type;
			PIO_OP_KEY           pop_key;
			PIO_BUF              pio_buf;
		};
	};
} IO_OP_DATA, *PIO_OP_DATA;

// ��Դ
typedef struct _IO_RESOURCE
{
	HANDLE              completion_port;

	LONG                max_key_c;
	LONG                max_buf_c_per_key;
	LONG                data_or_buf_c;

	S_LIST_HEADER       op_key_list;
	PIO_OP_KEY          pop_keys;

	S_LIST_HEADER       op_data_list;
	PIO_OP_DATA         pop_datas;

	S_LIST_HEADER       io_buf_list;
	PIO_BUF             pio_bufs;

	WORD                check_io_time;
	WORD                thread_c;
	LPHANDLE            pthreads;
	LONG volatile       real_thread_c;

	LONG volatile       key_error_c;
	LONG volatile       listener_c;
	LONG volatile       acceptor_c;
	LONG volatile       connector_c;
	LONG volatile       udp_c;

	LONG volatile       data_net_accept_c;
	LONG volatile       data_net_connect_c;
	LONG volatile       data_read_c;
	LONG volatile       data_write_c;

	HWND                hErrMsgWnd;
	DWORD               dwErrMsgID;

} IO_RESOURCE, *PIO_RESOURCE;

typedef struct
{
    // ȫ��״̬ hIoRes!=NULL ʱ��Ч
    LONG                io_error_c;
	LONG                listener_c;
    LONG                acceptor_c;
    LONG                connector_c;
    LONG                udp_c;
    LONG                data_net_accept_c;
    LONG                data_net_connect_c;
    LONG                data_read_c;
    LONG                data_write_c;
    LONG                remain_key_c;
    LONG                remain_data_c;
    LONG                remain_buf_c;
    LONG                reserved1[20];
    // ����״̬ hObject!=NULL ʱ��Ч
    DWORD               s_r_c;                 // ���ɹ�������
    DWORD               s_w_c;                 // ���ɹ�д����
    DWORD               l_w_c;                 // ������д����
    LONG                ref_c;
    LONG                reserved2[12];
} IO_USED_STATE, *PIO_USED_STATE;

#ifdef __cplusplus
extern "C"
{
#endif

// ����NULLʧ�ܣ�������Ϊ���º����ĵ�һ������
HANDLE IOCP_DLL_EXPORT CreateIoResource(
    DWORD dwWorkerThreadCount,  // �����߳�����,0-Ĭ���ں���*3
    DWORD dwMaxIoCount,         // ���IO�������
    DWORD dwMaxBufCountPerIo,   // ÿ��IO�������󻺳�������,����Ϊ3
    DWORD dwCheckIoTime         // ÿN�������ݣ�0�����
);

// ������Դ���˺�����ȴ����й����߳��˳�
void IOCP_DLL_EXPORT DestroyIoResource(HANDLE hIoRes);

// ����TCP����˶���
// �ɹ����غ�������StartListenerAccept
HANDLE IOCP_DLL_EXPORT CreateTcpListener(
    HANDLE hIoRes,
    const char *sListenAddr,    // ������ַ��NULL-Ĭ��"0.0.0.0"
    WORD wListenPort,           // �����˿�
    int  iListenCount,          // �������и��������������5
    PFN_IO_EVENT fnAcceptEvt,   // �������ӻص�
    PFN_IO_EVENT fnCloseEvt,    // ���ӶϿ��ص�
    PFN_IO_EVENT fnReadEvt,     // ��ȡ�ص�
    PFN_IO_EVENT fnWriteEvt,    // ����� IOCP_WRITE_COMPLETE_CALL ʱ��Ч
    PFN_IO_EVENT fnExceptEvt,   // �����쳣�ص�
    LPVOID lpParam
);

// ����FALSEʱ���˶����Զ��ͷ�
// �˺����ɹ����غ���ܿ�ʼ��������
BOOL IOCP_DLL_EXPORT StartListenerAccept(
    HANDLE hIoRes,
    HANDLE hListener,
    LONG lAcceptCountTheTime // >0 && <=dwMaxBufCountPerIo
);

// ����TCP�ͻ��˶���
// �ɹ����غ�������StartConnectorReceive
HANDLE IOCP_DLL_EXPORT CreateTcpConnector(
    HANDLE hIoRes,
    const char *sListenerAddr,  // ���ӵ�ַ
    WORD wListenerPort,         // ���Ӷ˿�
    DWORD dwWaitTime,           // ���ӵȴ�,��0-���ӳɹ��󷵻�
    PFN_IO_EVENT fnOpenEvt,     // ��Ч
    PFN_IO_EVENT fnCloseEvt,    // �رջص�
    PFN_IO_EVENT fnReadEvt,     // ��ȡ�ص�
    PFN_IO_EVENT fnWriteEvt,    // ����� IOCP_WRITE_COMPLETE_CALL ʱ��Ч
    PFN_IO_EVENT fnExceptEvt,   // ��Ч
    LPVOID lpParam
);

// ����FALSEʱ���˶����Զ��ͷ�
// �˺����ɹ����غ���ܿ�ʼ��д����
BOOL IOCP_DLL_EXPORT StartConnectorReceive(
    HANDLE hIoRes,
    HANDLE hConnector
);

// BUF �����ü��������Զ��д����
// ���ô˺������������ͷź���
// len ����С�ڵ��� MAX_IOCP_BUF_LEN
// UDP����ʱ:sRemoteAddr��wRemotePort��Ч
HANDLE IOCP_DLL_EXPORT GetFreeBuf(
    HANDLE hIoRes,
    PCHAR sRemoteAddr, WORD wRemotePort,
    PCHAR buf, DWORD len);

void IOCP_DLL_EXPORT ReleaseBuf(HANDLE hIoRes, HANDLE hIoBuf);

// д���ݺ���
//     ����ֵ -3:Ͷ��ʧ��,-2:����ѹر�,-1:��Դ����
//         0:Ͷ�ݳɹ�,1:�������,2:UDPͶ��ʧ��
//     ����<-1��Ͳ�Ҫ��д��

// WriteData�������ȡ����BUF����ʹ��
// ����˳�� GetFreeBuf -> WriteData -> ReleaseBuf
int IOCP_DLL_EXPORT WriteData(
    HANDLE hIoRes, HANDLE hObject,
    HANDLE hIoBuf
);

int IOCP_DLL_EXPORT WriteDataEx(
    HANDLE hIoRes, HANDLE hObject,
    PCHAR sRemoteAddr, WORD wRemotePort,
    PCHAR buf, DWORD len
);
//����ȫ����Դ
void IOCP_DLL_EXPORT SetGIoRes(HANDLE IoRes);
// ֻ���������߳ɹ����ء�Accept�ص���������ݻص��е���
void IOCP_DLL_EXPORT SetLimitReadLen(
    HANDLE hIoRes, HANDLE hObject, BOOL bLimit);

// ��������һ��ʱ����û�з��ʣ������ر�
// ���ô˺������ֶ����¶������ʱ��
void IOCP_DLL_EXPORT TouchObject(HANDLE hIoRes, HANDLE hObject);

// ������Чʱ����ʱ���Ե���
// �쳣��رջص��������Ч��

void IOCP_DLL_EXPORT SetDataTimeOut(HANDLE hIoRes, HANDLE hObject, WORD wIoTimeout);

void IOCP_DLL_EXPORT CloseObject(HANDLE hIoRes, HANDLE hObject);

BOOL IOCP_DLL_EXPORT GetPeerAddr(
	HANDLE hIoRes, HANDLE hObject, PCHAR ip, PWORD port);

// ��ȡIO״̬
void IOCP_DLL_EXPORT GetIoUsedState(
    HANDLE hIoRes, HANDLE hObject, PIO_USED_STATE);

// ����UDP����˶���
HANDLE IOCP_DLL_EXPORT CreateUdpObject(
    HANDLE hIoRes,
    const char *sListenAddr,    // ������ַ��NULL-Ĭ��"0.0.0.0"
    WORD wListenPort,           // �����˿ڣ�0-���
    PFN_IO_EVENT fnOpenEvt,     // ��Ч
    PFN_IO_EVENT fnCloseEvt,    // �رջص�
    PFN_IO_EVENT fnReadEvt,     // ��ȡ�ص�
    PFN_IO_EVENT fnWriteEvt,    // ����� IOCP_WRITE_COMPLETE_CALL ʱ��Ч
    PFN_IO_EVENT fnExceptEvt,   // ��Ч
    LPVOID lpParam
);

// ��ʼUDP�׽ӿڽ�������
BOOL IOCP_DLL_EXPORT StartUdpReceive(
    HANDLE hIoRes,
    HANDLE hUdp,
    LONG lRecvCountTheTime      // ͬʱ�̽�������, >0 && <=dwMaxBufCountPerIo
);

BOOL IOCP_DLL_EXPORT GetUdpPeerAddr(
    HANDLE hIoRes, HANDLE hUdp,
    PCHAR pDataBuf,             // ����ɻص������ݲ���
    PCHAR ip, PWORD port);

// hIoBuf: Ϊ GetFreeBuf �ķ���ֵ
// ��ȡ��������ַ
PCHAR IOCP_DLL_EXPORT GetIoBufAddr(HANDLE hIoRes, HANDLE hIoBuf);
// ���û���������
void IOCP_DLL_EXPORT SetIoBufLen(HANDLE hIoRes, HANDLE hIoBuf, DWORD len);

// ���ý��չ̶�����
// nLimit=1 ʱ dwLimitLen ��Ч������������
// nLimit=2 ʱ ��ͷ4���ֽڵ������򳤶����ƣ����Ȱ���4���ֽ�
// nLimit=3 ʱ ��ͷ4���ֽڵ��ڴ�ֵ���ƣ�������4���ֽ�
// nLimit=4 ʱ dwLimitLen ��Ч�����������ƣ����Ǵ˳��ȶ���
// nLimit=5 ʱ ��ͷ4���ֽڵ������򳤶����ƣ����Ȱ���4���ֽڣ���һ���������˳��ȣ��ص���Σ�ֱ��һ�����ص����
void IOCP_DLL_EXPORT SetLimitReadLen2(HANDLE hIoRes, HANDLE hObject, LONG nLimit, WORD wLimitLen);

// ���ûص��������ص�������Ŀǰֻ�����ö�ȡ�¼�
// lEvtType��1-OpenEvt,2-CloseEvt,3-ReadEvt,4-WriteEvt,5-ExceptEvt,6-AllEvt's Param,7-AllEvt
void IOCP_DLL_EXPORT SetIoEvent(HANDLE hIoRes, HANDLE hObject, LONG lEvtType, PFN_IO_EVENT pFunc, LPVOID lpParam);

// ��������ʱ����Ϣ���͸�������
void IOCP_DLL_EXPORT SetErrMsgWnd(HANDLE hIoRes, HWND hWnd, DWORD dwMsg);

// ��ȡ���ص�ַ
BOOL IOCP_DLL_EXPORT GetLocalAddr(
	HANDLE hIoRes, HANDLE hObject, PCHAR ip, PWORD port);

// ����TCP�ͻ��˶������ӱ��ض˿�
// �ɹ����غ�������StartConnectorReceive
HANDLE IOCP_DLL_EXPORT CreateTcpConnectorEx(
    HANDLE hIoRes,
    WORD wBindPort,             // ���ض˿�
    const char *sListenerAddr,  // ���ӵ�ַ
    WORD wListenerPort,         // ���Ӷ˿�
    DWORD dwWaitTime,           // ���ӵȴ�,��0-���ӳɹ��󷵻�
    PFN_IO_EVENT fnOpenEvt,     // ��Ч
    PFN_IO_EVENT fnCloseEvt,    // �رջص�
    PFN_IO_EVENT fnReadEvt,     // ��ȡ�ص�
    PFN_IO_EVENT fnWriteEvt,    // ����� IOCP_WRITE_COMPLETE_CALL ʱ��Ч
    PFN_IO_EVENT fnExceptEvt,   // ��Ч
    LPVOID lpParam
);

// ���Ӳ���
void IOCP_DLL_EXPORT SetObjectParam(HANDLE hIoRes, HANDLE hObject, LPVOID lpParam);
LPVOID IOCP_DLL_EXPORT GetObjectParam(HANDLE hIoRes, HANDLE hObject);

// ͬ����ȡ����������ú���Start...Receive��Ͳ����ٵ��ô˺���
// ���أ�0����ʱ��-1������>0����������С
// ��ȡ������ SetLimitReadLen2 ����
// nMsecOfTimeout: ��ʱʱ�䣬����
int IOCP_DLL_EXPORT IocpSyncRead(HANDLE hIoRes, HANDLE hObject, HANDLE hIoBuf, LONG nMsecOfTimeout);

#ifdef OLD_IOCP
typedef enum _IO_OPERATION
{
	IO_CONNECT_COMPLETED = 0,			//Client�����������
	IO_ACCEPT_COMPLETED,			//Server�����������
	IO_INITIAL,						//IO��ʼ������ͣ��
	IO_RECV,						//���ݶ�ȡ��
	IO_RECV_COMPLETED,				//���ݽ������
	IO_SEND,						//���ݷ�����
	IO_SEND_COMPLETED,				//���ݷ������
	IO_NET_EXCEPTION,				//�����쳣,��ʾԶ�����ӶϿ�
	IO_ZERO_READ,					//0��ʼ����ȡ��ͣ��
	IO_ZERO_READ_COMPLETED,			//0��ʼ����ȡ��ϣ�ͣ��
	IO_TRANSMIT_FILE_COMPLETED,		//�ļ��������
	IO_POST_PACKAGE,				//����IO�����ź� 

}IO_OPERATION;
typedef void (CALLBACK *fNetSDKCallBack)(int iClientID,/*�ͻ�ID��ʶ*/
										 IO_OPERATION ioFlag,/*IO��־���μ�IO_OPERATION*/
										 int iSuccess,/*0=ERR_OK,���������*/
										 char *pBuffer,/*������*/
										 int iLen,/*���������ݳ���*/
										 DWORD dwUser/*�û��Զ�������*/);
BOOL IOCP_DLL_EXPORT NET_Startup();
BOOL IOCP_DLL_EXPORT NET_Listen(DWORD dwPort);
BOOL IOCP_DLL_EXPORT NET_RegisterNetCallBack(fNetSDKCallBack lpFunc,DWORD dwUser);
BOOL IOCP_DLL_EXPORT NET_SendPacket(int iClientID,char *pPacket,int iLen);
BOOL IOCP_DLL_EXPORT NET_Connect(char *pClientIP,DWORD dwPort,int *iClientID);
BOOL IOCP_DLL_EXPORT NET_Disconnect(int iClientID);
BOOL IOCP_DLL_EXPORT NET_Close();
char* IOCP_DLL_EXPORT NET_GetRemoteIP(int iClientID);
#endif

#ifdef __cplusplus
}
#endif

#endif // __IOCP_H__

