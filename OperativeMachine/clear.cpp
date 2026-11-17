//=============================================================================
// 
// クリア処理[clear.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "clear.h"

#include "renderer.h"
#include "manager.h"
#include "color.h"
#include "debugproc.h"
#include "input.h"
#include "fade.h"

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
CPlayer* CClear::m_pPlayer = NULL;
CObjectX* CClear::m_pObjectX = NULL;
CObject3D* CClear::m_pObject3D[256] = {};

//=============================================================================
//コンストラクタ
CClear::CClear() :CScene(CScene::MODE_CLEAR)
{

}

//=============================================================================
//デストラクタ
CClear::~CClear()
{
}

//=============================================================================
//初期化
void CClear::Init(void)
{
	//カメラ情報の初期化
	CManager::GetCamera()->SetAll(D3DXVECTOR3(0.0f, CAMERA_ANGLE_PY, -700.0f - CAMERA_ANGLE_PZ),
		D3DXVECTOR3(0.0f, 30.0f, -950.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//各テクスチャの生成=========================
	CObject3D::Load();

	for (int nCntField = 0; nCntField < 2; nCntField++)
	{//床
		m_pObject3D[nCntField] = CObject3D::Create(D3DXVECTOR3(SIZE_MINI.x * nCntField, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI / 2.0f, 0.0f), SIZE_NORMAL);
	}

	//クリアUI表示
	CTextureUI::Create("data\\TEXTURE\\UI\\clear000.png",D3DXVECTOR3(240.0f, 210.0f, 0.0f), D3DXVECTOR3(880.0f, 100.0f, 0.0f),COLOR_NONE);

	//プレイヤーの生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

//=============================================================================
//終了
void CClear::Uninit(void)
{
	//すべて破棄
	CObject::ReleaseAll();

	//各テクスチャの破棄=========================
	CObject3D::Unload();
}

//=============================================================================
//更新
void CClear::Update(void)
{
	CDebugproc* pDebug = CManager::GetDebugproc();
	CInputKeyboard* pInputKey = CManager::GetInputKeyboard();
	CInputJoypad* pInputJoy = CManager::GetInputJoypad();
	CFade* pFade = CManager::GetFade();

	if (pInputKey->GetTrigger(DIK_RETURN) == true || pInputJoy->GetTrigger(JOYKEY_A) && CManager::GetFadePause() == false)
	{
		pFade->SetFade(MODE_TITLE);
	}
}

//=============================================================================
//描画
void CClear::Draw(void)
{

}