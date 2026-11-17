//=============================================================================
// 
// チュートリアル処理[tutorial.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "tutorial.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "color.h"
#include "input.h"
#include "fade.h"
#include "pause.h"
#include "sound.h"
#include "load.h"

#include "camera.h"
#include "number.h"
#include "player.h"
#include "enemy.h"
#include "textureUI.h"
#include "energy.h"
#include "score.h"
#include "effect.h"
#include "particle.h"
#include "bullet.h"
#include "object2D.h"
#include "object3D.h"
#include "objectX.h"
#include "meshfield.h"
#include "billboard.h"
#include "range.h"
#include "particle.h"
#include "objectX.h"
#include "crosshair.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_MINI		(D3DXVECTOR3(100.0f, 0.0f, 100.0f))
#define SIZE_NORMAL		(D3DXVECTOR3(1000.0f, 0.0f, 1000.0f))
#define MODEL_WALL		"data/MODEL/object/wall002.x"

//静的メンバ変数宣言-----------------------------------------------------------
CPause* CTutorial::m_pPause = NULL;
CPlayer* CTutorial::m_pPlayer = NULL;
CBullet* CTutorial::m_pBullet = NULL;
CScore* CTutorial::m_pScore = NULL;
CEffect* CTutorial::m_pEffect = NULL;
CParticle* CTutorial::m_pParticle = NULL;
CObject3D* CTutorial::m_pObject3D[256] = {};
CMeshField* CTutorial::m_pMeshField = NULL;
CEnergy* CTutorial::m_pEnergy = NULL;
CObjectX* CTutorial::m_apObjxWall[MAX_WALL] = {};
CTextureUI* CTutorial::m_pTextureUI[UI_MAX] = {};
CCrosshair* CTutorial::m_pCrosshair = NULL;
bool CTutorial::m_tutorialUI = false;

//=============================================================================
//コンストラクタ
CTutorial::CTutorial() :CScene(CScene::MODE_TUTORIAL)
{
}

//=============================================================================
//デストラクタ
CTutorial::~CTutorial()
{
}

//=============================================================================
//初期化
void CTutorial::Init(void)
{
	//カメラ情報の初期化
	CManager::GetCamera()->SetAll(D3DXVECTOR3(0.0f, CAMERA_ANGLE_PY, -700.0f - CAMERA_ANGLE_PZ),
		D3DXVECTOR3(0.0f, 30.0f, -950.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//各テクスチャの生成
	CObject3D::Load();
	CEffect::Load();
	CBullet::Load();
	CNumber::Load();
	CRange::Load();

	if (m_pPause == NULL)
	{ //ポーズ処理
		m_pPause = new CPause;
		if (m_pPause != NULL)
		{
			m_pPause->Init();
		}
	}

	//BGMを再生
	CSound::PlaySound(CSound::SOUND_LABEL_GAMEBGM);

	//=========================================================================
	// フィールド床面の生成
	int nCntField = 0;
	float fTileSizeX = (SIZE_NORMAL.x > 0.0f) ? SIZE_NORMAL.x : 200.0f;
	float fTileSizeZ = (SIZE_NORMAL.z > 0.0f) ? SIZE_NORMAL.z : 200.0f;

	for (float fPosZ = -800.0f; fPosZ <= 2000.0f; fPosZ += fTileSizeZ)
	{
		for (float fPosX = -800.0f; fPosX <= 800.0f; fPosX += fTileSizeX)
		{
			if (nCntField < NUM_OBJ3D1)
			{
				m_pObject3D[nCntField++] = CObject3D::Create(
					D3DXVECTOR3(fPosX + (fTileSizeX * 0.5f), 0.0f, fPosZ + (fTileSizeZ * 0.5f)),
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					SIZE_NORMAL);
			}
		}
	}

	for (int nIndexField = nCntField; nIndexField < NUM_OBJ3D1; nIndexField++)
	{
		m_pObject3D[nIndexField] = NULL;
	}

	//UI・タイマーの生成
	CTextureUI::Create("data\\TEXTURE\\UI\\energy000.png", D3DXVECTOR3(19.0f, 3.0f, 0.0f), D3DXVECTOR3(322.0f, 40.0f, 0.0f), COLOR_CYAN);
	m_pEnergy = CEnergy::Create("data\\TEXTURE\\UI\\energy001.png", D3DXVECTOR3(20.0f, 3.0f, 0.0f), D3DXVECTOR3(MAX_ENERGY, 40.0f, 0.0f), COLOR_NORMAL);

	//クロスヘアUIの生成
	m_pCrosshair = CCrosshair::Create(CENTER_2DPOS, D3DXVECTOR3(20.0f, 20.0f, 0.0f), COLOR_NONE);

	//プレイヤー生成（スタート位置）
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 0.0f, -400.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//=========================================================================
	// 壁の配置
	int wallIdx = 0;
	const int maxWallCount = sizeof(m_apObjxWall) / sizeof(m_apObjxWall[0]);

	for (float fPosX = -600.0f; fPosX <= 600.0f; fPosX += 400.0f)
	{
		if (wallIdx < maxWallCount)
			m_apObjxWall[wallIdx++] = CObjectX::Create(CObjectX::TYPE_WALL, MODEL_WALL, D3DXVECTOR3(fPosX, 0.0f, -600.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	for (float fPosX = -600.0f; fPosX <= 600.0f; fPosX += 400.0f)
	{
		if (wallIdx < maxWallCount)
			m_apObjxWall[wallIdx++] = CObjectX::Create(CObjectX::TYPE_WALL, MODEL_WALL, D3DXVECTOR3(fPosX, 0.0f, 1800.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	for (float fPosZ = -200.0f; fPosZ <= 1400.0f; fPosZ += 400.0f)
	{
		if (wallIdx < maxWallCount)
			m_apObjxWall[wallIdx++] = CObjectX::Create(CObjectX::TYPE_WALL, MODEL_WALL, D3DXVECTOR3(-600.0f, 0.0f, fPosZ), D3DXVECTOR3(0.0f, 1.57f, 0.0f));
	}

	for (float fPosZ = -200.0f; fPosZ <= 1400.0f; fPosZ += 400.0f)
	{
		if (wallIdx < maxWallCount)
			m_apObjxWall[wallIdx++] = CObjectX::Create(CObjectX::TYPE_WALL, MODEL_WALL, D3DXVECTOR3(600.0f, 0.0f, fPosZ), D3DXVECTOR3(0.0f, 1.57f, 0.0f));
	}

	//通路全体を塞ぐように配置し、越えるアクションを強制・学習させます
	for (float fPosX = -400.0f; fPosX <= 400.0f; fPosX += 400.0f)
	{
		if (wallIdx < maxWallCount)
		{
			//Y座標を-70.0fに下げて低く設置
			m_apObjxWall[wallIdx++] = CObjectX::Create(CObjectX::TYPE_WALL, MODEL_WALL, D3DXVECTOR3(fPosX, -70.0f, 600.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
	}

	// 配列の残りをNULLクリア
	for (int nCntWall = wallIdx; nCntWall < maxWallCount; nCntWall++)
	{
		m_apObjxWall[nCntWall] = NULL;
	}

	//敵
	CEnemy::Create(D3DXVECTOR3(300.0f, 100.0f, 10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 3.14f, 0.0f), CEnemy::TYPE_NONE);

	//=========================================================================
	// ゴール
	CObjectX::Create(CObjectX::TYPE_GOAL, "data/MODEL/object/safe001.x", D3DXVECTOR3(0.0f, 0.0f, 1200.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	if (m_pTextureUI[UI_TUTORIAL] == NULL)
	{
		m_pTextureUI[UI_TUTORIAL] = CTextureUI::Create("data\\TEXTURE\\UI\\tutorial001.png", D3DXVECTOR3(100.0f, 50.0f, 0.0f), D3DXVECTOR3(1000.0f, 600.0f, 0.0f), COLOR_NORMAL);
	}

	m_pTextureUI[UI_BOTTUN] = CTextureUI::Create("data\\TEXTURE\\UI\\PressEnter.png", D3DXVECTOR3(490.0f, 600.0f, 0.0f), D3DXVECTOR3(300.0f, 100.0f, 0.0f), COLOR_NORMAL);
}

//=============================================================================
//終了
void CTutorial::Uninit(void)
{
	if (m_pPause != NULL)
	{//終了、NULLにする
		m_pPause->Uninit();
		delete m_pPause;
		m_pPause = NULL;
	}

	//配列数を取得
	int nWallNum = sizeof(m_apObjxWall) / sizeof(m_apObjxWall[0]);

	for (int nCntWall = 0; nCntWall < nWallNum; nCntWall++)
	{//NULLにする
		m_apObjxWall[nCntWall] = NULL;
	}

	for (int nCntUI = 0; nCntUI < UI_MAX; nCntUI++)
	{
		if (m_pTextureUI[nCntUI] != NULL)
		{
			m_pTextureUI[nCntUI] = NULL;
		}
	}

	//BGMを停止
	CSound::StopSound(CSound::SOUND_LABEL_GAMEBGM);

	//すべて破棄
	CObject::ReleaseAll();

	//各テクスチャの破棄
	CNumber::Unload();
	CBullet::Unload();
	CEffect::Unload();
	CObject3D::Unload();
}

//=============================================================================
//更新
void CTutorial::Update(void)
{
	//キー入力
	CInputKeyboard* pInputKey = CManager::GetInputKeyboard();
	CInputJoypad* pInputJoy = CManager::GetInputJoypad();

	//フェードを取得
	CFade* pFade = CManager::GetFade();

	if (m_pPause != NULL)
	{//ポーズのアップデート
		m_pPause->Update();
	}

	if (pInputKey->GetTrigger(DIK_RETURN) || pInputJoy->GetTrigger(JOYKEY_A))
	{
		if (m_pTextureUI[UI_TUTORIAL] != NULL && m_pTextureUI[UI_BOTTUN] != NULL && m_pTextureUI[UI_SPIDER] == NULL)
		{
			m_pTextureUI[UI_TUTORIAL]->SetColor(COLOR_TOUMEI);
			m_pTextureUI[UI_BOTTUN]->SetColor(COLOR_TOUMEI);

			//m_pTextureUI[UI_SPIDER]->CTextureUI::Create("data\\TEXTURE\\UI\\player_tutorial001.png", D3DXVECTOR3(390.0f, 20.0f, 0.0f), D3DXVECTOR3(500.0f, 600.0f, 0.0f), COLOR_NORMAL);
		}
		//else if (m_pTextureUI[UI_SPIDER] != NULL && m_pTextureUI[UI_BOTTUN] != NULL)
		//{
		//	m_pTextureUI[UI_SPIDER]->SetColor(COLOR_TOUMEI);
		//	m_pTextureUI[UI_BOTTUN]->SetColor(COLOR_TOUMEI);
		//}
	}
}

//=============================================================================
//描画
void CTutorial::Draw(void)
{
}