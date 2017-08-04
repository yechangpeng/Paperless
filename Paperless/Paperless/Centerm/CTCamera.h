	/**
	* @brief ����sdk�������;
	* @param[in] hwnd Ԥ�����ھ��;
	* @return 0/-1 �ɹ�/ʧ��;
	*/
	HANDLE WINAPI hhCreateHighCamera(HWND hwnd);

	/*
	* @brief �ͷ�SDK�������,������Դ;
	* @return 0
	*/
	void WINAPI hhReleaseHighCamera(HANDLE handle);

	/*
	* @brief ö������ͷ�豸;
	* @return �豸��;
	*/
	int WINAPI hhDetectDevice(HANDLE handle);

	/*
	* @brief ��ָ������ͷ;
	* @param[in] index ����ͷ���: 0/1 ������ͷ/������ͷ;
	* @return 0/1 �ɹ�/ʧ��;
	*/
	int WINAPI hhOpenDevice(HANDLE handle, int index);

	/*
	* @brief �رյ�ǰ����ͷ;
	* @param[in] handle sdk�������;
	* @return 0/-1 �ɹ�/ʧ��;
	*/
	int WINAPI hhCloseDevice(HANDLE handle);

	/*
	* @brief ����;
	* @param[in] handle sdk�������;
	* @param[in] index ����ͷ���: 0/1 ������ͷ/������ͷ;
	* @return -1 ʧ�� ; ������ �ֱ��ʸ���;
	*/
	int WINAPI hhScanImage(HANDLE handle, const wchar_t* path);

	/*
	* @brief �ر�ָ������ͷ;
	* @param[in] handle sdk�������;
	* @param[in] index ����ͷ���: 0/1 ������ͷ/������ͷ;
	* @return 0/-1 �ɹ�/ʧ��;
	*/
	int WINAPI hhGetScanSizeCount(HANDLE handle, int index);

	/*
	* @brief ��ȡָ���豸�ķֱ���;
	* @param[in] handle sdk�������;
	* @param[in] index ����ͷ���: 0/1 ������ͷ/������ͷ;
	* @param[in] format �ֱ���������;
	* @param[out] width/height  �����ȡ�ķֱ���ֵ;
	* @return 0/-1 �ɹ�/ʧ��;
	*/
	int WINAPI hhGetScanSize(HANDLE handle, int index, int format, int* width, int* height);

	/*
	* @brief ����ָ���豸�ķֱ���;
	* @param[in] handle sdk�������;
	* @param[in] index ����ͷ���: 0/1 ������ͷ/������ͷ;
	* @param[in] format �ֱ���������;
	* @return 0/-1 �ɹ�/ʧ��;
	*/
	int WINAPI hhSetScanSize(HANDLE handle, int index, int format);

	/*
	* @brief �����������ý���;
	* @param[in] handle sdk�������;
	* @return 0/-1 �ɹ�/ʧ��;
	*/
	int WINAPI hhUpdatePreview(HANDLE handle);

	/*
	* @brief �����Զ��ñ�;
	* @param[in] handle sdk�������;
	* @param[in] index ����ͷ���: 0/1 ������ͷ/������ͷ;
	* @param[in] crop true/false ����/�ر�;
	* @return 0/-1 �ɹ�/ʧ��;
	*/
	int WINAPI hhSetAutoCrop(HANDLE handle, int index, bool crop);

	/*
	* @brief ����ɨ����Ƭ��dpi;
	* @param[in] handle sdk�������;
	* @param[in] dpiX / dpiY  ����dpi/����dpi;
	* @return 0/-1 �ɹ�/ʧ��;
	*/
	int WINAPI hhSetCaptureDPI(HANDLE handle, int dpiX, int dpiY);

	/*
	* @brief �����Ƿ�ɨ���ѡ����;
	* @param[in] handle sdk�������;
	* @param[in] captureCrop true/false ����/�ر�;
	*/
	int WINAPI SetCaptureCrop(HANDLE handle, bool captureCrop);