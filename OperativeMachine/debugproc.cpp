//=============================================================================
// 
//  デバッグ表示[debugproc.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "manager.h"
#include "bullet.h"
#include "color.h"

//=============================================================================
//コンストラクタ
CDebugproc::CDebugproc()
{
	//値をクリア
	m_pDevice = NULL;
	m_pFont = NULL;
	m_aStrDebug[0] = {};
	m_bOn = true;
}

//=============================================================================
//デストラクタ
CDebugproc::~CDebugproc()
{
}

//=============================================================================
//初期化
void CDebugproc::Init(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();

	m_pDevice = pRenderer->GetDevice();

	//デバッグ表示用フォントの生成
	D3DXCreateFont(m_pDevice, 25, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		"Terminal", &m_pFont);

	//デバッグ情報バッファのクリア
	memset(&m_aStrDebug[0], 0, sizeof m_aStrDebug);
}

//=============================================================================
//終了
void CDebugproc::Uninit(void)
{
	//デバッグ表示用フォントの破棄
	if (m_pFont != NULL)
	{
		m_pFont->Release();
		m_pFont = NULL;
	}
}

//=============================================================================
//更新
void CDebugproc::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetTrigger(DIK_F1) == true)
	{
		m_bOn = !m_bOn;
	}
}

//=============================================================================
//描画
void CDebugproc::Draw(void)
{
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	if (m_bOn != true)
	{
		// テキストの描画
		m_pFont->DrawText(NULL, &m_aStrDebug[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 50, 50, 255));
	}

	//デバッグ情報バッファのクリア
	memset(&m_aStrDebug[0], 0, sizeof m_aStrDebug);
}

//=============================================================================
//Printf
void CDebugproc::PrintDebugProc(const char* fmt, ...)
{
	static char buff[1024];

	va_list arg;
	va_start(arg, fmt);
	vsnprintf_s(buff, sizeof(buff), _TRUNCATE, fmt, arg);
	va_end(arg);

	size_t used = strlen(m_aStrDebug);
	if (used >= sizeof(m_aStrDebug) - 1)
	{
		return;
	}

	size_t available = sizeof(m_aStrDebug) - used - 1;
	size_t appendLength = strlen(buff);
	if (appendLength > available)
	{
		appendLength = available;
	}

	memcpy(m_aStrDebug + used, buff, appendLength);
	m_aStrDebug[used + appendLength] = '\0';
}

//=============================================================================
//オンかオフか
bool CDebugproc::GetOnOff(void)
{
	return m_bOn;
}


