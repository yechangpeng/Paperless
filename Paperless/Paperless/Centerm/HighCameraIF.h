/***************************************

***************************************/
#ifndef __HIGHCAMERAIF_H__
#define __HIGHCAMERAIF_H__

class CHighCameraIF
{
public:
	CHighCameraIF(CStatic *pVideoWnd ){}
	virtual ~CHighCameraIF(){}

//Ҫ��OpenDevice��ScanImage��UpdatePreview�Ƚӿڣ��ɹ�����0������Ϊʧ�ܡ���Ҫ�ر�˵���ģ���ע������ֵ
public:
	virtual int DetectDevice() = 0;//̽���豸�Ƿ���ڡ����س��̸����ǵ�����ͷ����
	virtual	int OpenDevice( int idx ) = 0;//�򿪸����ǵ�����ͷ Ҫ��id=0Ϊ������ͷ id=1Ϊ������ͷ
	virtual int CloseDevice() = 0;//�رյ�ǰ�豸���ͷ������Դ���������˳�ʱ����   --- ����������л���ͷʱ�Ƿ���Ҫ���ã�
	virtual int ScanImage( CString strFileName ) = 0;//���գ���Ҫ�󱣴�ͼ��	-- �ӿ�д�ú���˵��һ��strFileName�ĸ�ʽ
	virtual int GetScanSizeCount( int idx ) = 0;//��ȡָ������ͷ�ֱ���ģʽ����������֧��800*600,1024*768�򷵻�2
	virtual int GetScanSize( int  nIndex,
                             int  idx,
                             int *pWidth,
                             int *pHeight ) = 0;//ָ������ͷָ���ֱ���ģʽ�ľ�����ֵ
	virtual int SetScanSize( int nIndex, int idx ) = 0;//����ָ������ͷ�ķֱ���
	virtual int UpdatePreview() = 0;//���¸����ǲ�������--���ع�ȵ�
	virtual int SetAutoCrop( bool bIsAuto, int idx ) = 0;//ָ������ͷ�Զ��ñ�
};

#endif