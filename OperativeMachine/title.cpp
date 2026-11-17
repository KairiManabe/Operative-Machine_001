//=============================================================================
// 
// タイトル処理[title.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "title.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "color.h"

#include "game.h"

#include "camera.h"
#include "textureUI.h"
#include "player.h"
#include "object2D.h"
#include "object3D.h"
#include "objectX.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_MINI		(D3DXVECTOR3(100.0f, 0.0f, 100.0f))
#define SIZE_NORMAL		(D3DXVECTOR3(300.0f, 0.0f, 300.0f))

//静的メンバ変数宣言-----------------------------------------------------------
CPlayer* CTitle::m_pPlayer = NULL;
CObjectX* CTitle::m_pObjectX = NULL;
CObject3D* CTitle::m_pObject3D[256] = {};
CTextureUI* CTitle::m_pTextureUI[MENU_MAX] = {};

//=============================================================================
//コンストラクタ
CTitle::CTitle() :CScene(CScene::MODE_TITLE)
{
	m_CametaRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Menu = MENU_TUTORIAL;
	m_nStickCounter = 20;
}

//=============================================================================
//デストラクタ
CTitle::~CTitle()
{
}

//=============================================================================
//初期化
void CTitle::Init(void)
{
	//カメラ情報の初期化
	CManager::GetCamera()->SetAll(D3DXVECTOR3(0.0f, CAMERA_ANGLE_PY, -700.0f - CAMERA_ANGLE_PZ),
		D3DXVECTOR3(0.0f, 30.0f, -950.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//各テクスチャの生成=========================
	CObject3D::Load();

	//BGMを再生
	CSound::PlaySound(CSound::SOUND_LABEL_TITLEBGM);

	for (int nCntField = 0; nCntField < 2; nCntField++)
	{
		m_pObject3D[nCntField] = CObject3D::Create(D3DXVECTOR3(SIZE_NORMAL.x * nCntField, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI / 2.0f, 0.0f), SIZE_NORMAL);
	}

	CTextureUI::Create("data\\TEXTURE\\UI\\title000.png", D3DXVECTOR3(200.0f, 200.0f, 0.0f), D3DXVECTOR3(320.0f, 100.0f, 0.0f), COLOR_NORMAL);

	//メニューを生成
	m_pTextureUI[MENU_TUTORIAL] = CTextureUI::Create("data\\TEXTURE\\UI\\tutorial000.png", D3DXVECTOR3(215.0f, 350.0f, 0.0f), D3DXVECTOR3(280.0f, 60.0f, 0.0f), COLOR_NORMAL);
	m_pTextureUI[MENU_GAME] = CTextureUI::Create("data\\TEXTURE\\UI\\playgame000.png", D3DXVECTOR3(240.0f, 420.0f, 0.0f), D3DXVECTOR3(230.0f, 60.0f, 0.0f), COLOR_NORMAL);

	//生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

//=============================================================================
//終了
void CTitle::Uninit(void)
{
	//BGMを停止
	CSound::StopSound(CSound::SOUND_LABEL_TITLEBGM);

	//すべて破棄
	CObject::ReleaseAll();

	if (m_pPlayer != NULL)
	{
		m_pPlayer = NULL;
	}

	for (int nCnt = 0; nCnt < MENU_MAX; nCnt++)
	{
		if (m_pTextureUI[nCnt] != NULL)
		{
			m_pTextureUI[nCnt] = NULL;
		}
	}

	//各テクスチャの破棄=========================
	CObject3D::Unload();
}

//=============================================================================
//更新
void CTitle::Update(void)
{
	CDebugproc* pDebug = CManager::GetDebugproc();
	CInputKeyboard* pInputKey = CManager::GetInputKeyboard();
	CInputJoypad* pInputJoy = CManager::GetInputJoypad();
	CFade* pFade = CManager::GetFade();
	CCamera* pCamera = CManager::GetCamera();

	int nResult = m_Menu;		//メニュー用

	if (pCamera != NULL)
	{//カメラを回す
		m_CametaRot.y += 0.01f;

		if (m_CametaRot.y <= -D3DX_PI)
		{
			m_CametaRot.y += D3DX_PI * 2;
		}
		else if (m_CametaRot.y >= D3DX_PI)
		{
			m_CametaRot.y -= D3DX_PI * 2;
		}

		pCamera->SetRotation(m_CametaRot);
	}

	if (pInputKey->GetTrigger(DIK_RETURN) == true && CManager::GetFadePause() == false && m_Menu == MENU_GAME && CManager::GetFadePause() != true)
	{//ゲームに移行
		pFade->SetFade(MODE_GAME);
	}
	else if (pInputKey->GetTrigger(DIK_RETURN) == true && CManager::GetFadePause() == false && m_Menu == MENU_TUTORIAL && CManager::GetFadePause() != true)
	{//チュートリアルに移行
		pFade->SetFade(MODE_TUTORIAL);
	}

	if (pInputJoy->GetTrigger(JOYKEY_A) == true && CManager::GetFadePause() == false && m_Menu == MENU_GAME && CManager::GetFadePause() != true)
	{//ゲームに移行
		pFade->SetFade(MODE_GAME);
	}
	else if (pInputJoy->GetTrigger(JOYKEY_A) == true && CManager::GetFadePause() == false && m_Menu == MENU_TUTORIAL && CManager::GetFadePause() != true)
	{//チュートリアルに移行
		pFade->SetFade(MODE_TUTORIAL);
	}

	int LValueH = 0, LValueV = 0;

	if (pInputJoy->GetStickLeft(&LValueH, &LValueV))
	{//ジョイパットのポーズ操作
		if (LValueV >= 5000.0f)
		{
			m_nStickCounter++;
			if (m_nStickCounter > 20)
			{
				m_nStickCounter = 0;
				nResult--;
			}
			if (nResult <= -1)
			{
				nResult = MENU_GAME;
			}
		}
		else if (LValueV <= -5000.0f)
		{
			m_nStickCounter++;
			if (m_nStickCounter > 20)
			{
				m_nStickCounter = 0;
				nResult++;
			}

			if (nResult >= MENU_MAX)
			{
				nResult = MENU_TUTORIAL;
			}
		}
	}
	if (pInputJoy->GetStickLeft(&LValueH, &LValueV) == false)
	{
		m_nStickCounter = 20;
	}

	if (pInputJoy->GetTrigger(JOYKEY_UP) == true || pInputKey->GetTrigger(DIK_W) == true)
	{
		nResult--;

		if (nResult <= -1)
		{
			nResult = MENU_GAME;
		}
	}

	if (pInputJoy->GetTrigger(JOYKEY_DOWN) == true || pInputKey->GetTrigger(DIK_S) == true)
	{
		nResult++;

		if (nResult >= MENU_MAX)
		{
			nResult = MENU_TUTORIAL;
		}
	}

	//メニューに渡す
	m_Menu = (TITLE_MENU)nResult;

	//色と大きさを変える
	m_pTextureUI[m_Menu]->SetColor(COLOR_RED);

	if (m_Menu == MENU_TUTORIAL)
	{
		m_pTextureUI[MENU_GAME]->SetColor(COLOR_NORMAL);
	}
	else
	{
		m_pTextureUI[MENU_TUTORIAL]->SetColor(COLOR_NORMAL);
	}
}

//=============================================================================
//描画
void CTitle::Draw(void)
{

}