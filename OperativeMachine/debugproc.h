//=============================================================================
// 
//  デバッグ表示[debugproc.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _DEBUGPROC_H_ 
#define _DEBUGPROC_H_

//インクルードファイル---------------------------------------------------------
#include"main.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef USE_DEBUG_PRINT
#define DEBUG_PRINT(fmt, ...) debug_print(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) 
#endif

//デバッグクラス---------------------------------------------------------------
class CDebugproc
{
public:
	CDebugproc();
	~CDebugproc();

	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void PrintDebugProc(const char* fmt, ...);
	bool GetOnOff(void);
private:
	LPDIRECT3DDEVICE9 m_pDevice;
	LPD3DXFONT m_pFont;
	char m_aStrDebug[4096];		//文字列(デバッグ情報)を格納するバッファ
	bool m_bOn;
};

#endif 