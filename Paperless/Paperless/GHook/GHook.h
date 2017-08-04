#ifndef _GHOOK_H_
#define _GHOOK_H_

#include <windows.h>

#pragma once
#ifdef DLL_IMPLEMENT
#define DLL_API /*__declspec(dllexport)*/ __stdcall
#else
#define DLL_API /*__declspec(dllimport)*/ __stdcall
#endif
#include <stdio.h>
//��ϼ��ṹ��
typedef struct S_COMBKEY
{
	int count;//��ϼ�����
	BYTE keys[4];
}COMBKEY;

#ifdef __cplusplus
extern "C"
{
#endif

BOOL DLL_API SetGHook_KEYBOARD();
//Ŀǰ��������һ�����̹��ӣ�����ж�����ӵĻ����͸��ݹ���id���ͷŹ���
BOOL DLL_API FreeGHook_KEYBOARD();
void DLL_API SetCombKey(COMBKEY combKey[]);
void DLL_API SetXCombKey(int nX, COMBKEY combKey);
void DLL_API SetHWndAndMsgType(HWND hWnd[],UINT msgType[]);

#ifdef __cplusplus
}
#endif

#endif