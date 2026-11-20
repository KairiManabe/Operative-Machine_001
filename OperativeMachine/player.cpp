//=============================================================================
//
// プレイヤー[player.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "player.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "input.h"
#include "texture.h"
#include "ModelManager.h"
#include "color.h"
#include "sound.h"

//シーン
#include "scene.h"
#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "clear.h"
#include "fail.h"
#include "result.h"

#include "model.h"
#include "camera.h"
#include "meshfield.h"
#include "objectX.h"
#include "particle.h"
#include "bullet.h"
#include "effect.h"
#include "field.h"
#include "crosshair.h"

//マクロ定義-------------------------------------------------------------------
#define NO_USE
//#undef NO_USE

#define OFSET_HUMAN		"data/SCRIPT/player000.txt"
#define OFSET_SPIDER	"data/SCRIPT/spider.txt"
#define OFSET_CANNON	"data/SCRIPT/player.txt"
#define MOTION_HUMAN	"data/Motion/motion_player.txt"
#define MOTION_SPIDER	"data/Motion/motion_spider.txt"
#define MOTION_CANNON	"data/Motion/motion.txt"

// 形態ごとのパラメータ定義
#define HUMAN_SPEED     (1.0f)   // 人型の移動速度
#define SPIDER_SPEED    (0.3f)   // クモ型の移動速度（通常より下がる）
#define CANNON_SPEED    (0.0f)   // 銃型の移動速度（移動不可にする）

#define JUMP_POWER      (8.0f)   // ジャンプ力
#define GRAVITY         (0.5f)   // 重力
#define JETT	        (D3DXVECTOR3(2.0f,2.0f,0.0f)) //パーティクルjettの拡散幅など
#define JETT_NUM	    (5) //パーティクルjettの量

//静的メンバ変数宣言-----------------------------------------------------------
CPlayer::MODEL_INFO CPlayer::m_aModelInfo[50];
CMotion::INFO CPlayer::m_aMotionInfo[MOTIONTYPE_MAX];
CPlayer::MODE CPlayer::m_Mode = CPlayer::MODE_HUMAN;
bool CPlayer::m_bChangeMode = false;

//=============================================================================
//コンストラクタ
CPlayer::CPlayer(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	memset(&m_apModel[0], NULL, sizeof(m_apModel));
	for (int nModelInfo = 0; nModelInfo < sizeof(m_aModelInfo) / sizeof(m_aModelInfo[0]); ++nModelInfo)
	{
		m_aModelInfo[nModelInfo] = MODEL_INFO{};
	}
	memset(&m_aMotionInfo[0], NULL, sizeof(m_aMotionInfo));

	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 1.57f, 0.0f);
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_bLand = false;
	m_pMotion = NULL;
	m_nNumModel = 0;
	m_fModelChange = 1.0f;		//デフォルトが1.0f倍
	m_fCollisionSize = 25.0f;

	m_bClimbingWall = false;
	m_nIdxModel = 0;
}

//=============================================================================
//デストラクタ
CPlayer::~CPlayer()
{
}

//=============================================================================
//生成処理
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	//プレイヤー
	CPlayer* pPlayer = NULL;

	if (pPlayer == NULL)
	{
		//プレイヤー生成
		pPlayer = new CPlayer;

		if (pPlayer != NULL)
		{
			pPlayer->m_pos = pos;

			pPlayer->m_size = size;

			//プレイヤー初期化に失敗したオブジェクトは返さない
			if (FAILED(pPlayer->Init(pPlayer->m_pos, pPlayer->m_size)))
			{
				delete pPlayer;
				return NULL;
			}
		}
	}
	return pPlayer;
}

//=============================================================================
//プレイヤーの初期化
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	if (m_pMotion == NULL)
	{//モーションを生成
		m_pMotion = new CMotion;
	}
	else
	{
		return E_FAIL;
	}

	//階層構造のプレイヤーモデルファイル名を取得
	LoadParentModel(OFSET_HUMAN);
	LoadMotionInfo(MOTION_HUMAN);

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		//モデルの生成[0]体
		m_apModel[nCntModel] = CModel::Create(m_aModelInfo[nCntModel].FileName, m_aModelInfo[nCntModel].pos,m_aModelInfo[nCntModel].rot);
		if (m_apModel[nCntModel] == NULL)
		{
			continue;
		}

		const int parentIndex = m_aModelInfo[nCntModel].nIdxParent;
		if (parentIndex < 0 || parentIndex >= nCntModel || m_apModel[parentIndex] == NULL)
		{
			//親モデルの設定
			m_apModel[nCntModel]->SetParent(NULL);
		}
		else
		{
			//親モデルの設定
			m_apModel[nCntModel]->SetParent(m_apModel[parentIndex]);
		}
	}

	if (m_pMotion != NULL)
	{//モーションに必要なモデルの設定
		m_pMotion->SetModel(m_apModel, m_nNumModel);
	}

	for (int nCntInfo = 0; nCntInfo < MOTIONTYPE_MAX; nCntInfo++)
	{//読み込んだモーションを代入
		m_pMotion->SetInfo(m_aMotionInfo[nCntInfo]);
	}

	//ニュートラルモーション
	m_pMotion->Set(MOTIONTYPE_NEUTRAL,3);

	//種類の設定
	SetType(TYPE_PLAYER);

	return S_OK;
}

//=============================================================================
//プレイヤーの終了
void CPlayer::Uninit(void)
{
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL)
		{
			m_apModel[nCntModel]->Uninit();
			delete m_apModel[nCntModel];
			m_apModel[nCntModel] = NULL;
		}
	}
	m_nNumModel = 0;

	if (m_pMotion != NULL)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// 人型のモードに戻す
	m_Mode = MODE_HUMAN;

	// 破棄
	CObject::Release();
}

//=============================================================================
//プレイヤーの描画
void CPlayer::Draw(void)
{
	/*LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;
	D3DVIEWPORT9 viewport;
	D3DXMATRIX mtxViewDef, mtxProjectionDef;*/
	D3DXMATRIX mtxRot, mtxTrans;			//計算用マトリックス

	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

#ifndef NO_USE

	//現在のレンダリングターゲットを取得(保存)
	pDevice->GetRenderTarget(0, &pRenderDef);

	//現在のZバッファを取得(保存)
	pDevice->GetDepthStencilSurface(&pZBuffDef);

	//現在のビューポートを取得(保存)
	pDevice->GetViewport(&viewport);

	//現在のビューマトリックスを取得(保存)
	pDevice->GetTransform(D3DTS_VIEW, &mtxViewDef);

	//現在のプロジェクションマトリックスを取得(保存)
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

	//レンダリングターゲットの変更
	CManager::GetRenderer()->ChangeTarget(CManager::GetCamera()->GetPositionV(),
		CManager::GetCamera()->GetPositionR(),
		CManager::GetCamera()->GetPositionU());

	//レンダリングターゲット用テクスチャのクリア
	pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
	
#endif // DEBUG

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_rot.y, m_rot.x, m_rot.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//シャドーマトリックスの描画
	DrawShadow();

	//モデルの描画(全パーツ)
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL)
		{
			m_apModel[nCntModel]->Draw();
		}
	}

#ifndef NO_USE
	//レンダリングターゲット用のテクスチャを元に戻す
	pDevice->SetRenderTarget(0, pRenderDef);

	//Zバッファを元に戻す
	pDevice->SetDepthStencilSurface(pZBuffDef);
#endif
}

//=============================================================================
//プレイヤーの更新
void CPlayer::Update(void)
{
	m_posOld = m_pos;

	//取得
	CScene* pScene = CManager::GetScene();
	CCamera* pCamera = CManager::GetCamera();
	CDebugproc* pDebug = CManager::GetDebugproc();

	//入力系の取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();

	// Update can be called during partial initialization or shutdown.
	if (pScene == NULL || pCamera == NULL || pInputKeyboard == NULL ||
		pInputJoypad == NULL || m_pMotion == NULL)
	{
		return;
	}

	//カメラの向きを取得
	D3DXVECTOR3 rotCamera = pCamera->GetRotation();

	//オブジェクト3Dへのポインタ
	CObject3D** pObj3D = NULL;

	//各シーンのデータを取得する
	if (pScene->GetMode() == CScene::MODE_GAME)
	{
		pObj3D = CGame::GetObject3D();
	}
	else if (pScene->GetMode() == CScene::MODE_TITLE)
	{
		pObj3D = CTitle::GetObject3D();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	{
		pObj3D = CTutorial::GetObject3D();
	}
	else if (pScene->GetMode() == CScene::MODE_CLEAR)
	{
		pObj3D = CClear::GetObject3D();
	}
	else if (pScene->GetMode() == CScene::MODE_FAIL)
	{
		pObj3D = CFail::GetObject3D();
	}
	else if (pScene->GetMode() == CScene::MODE_RESULT)
	{
		pObj3D = CResult::GetObject3D();
	}

	D3DXVECTOR3 CameraAngle = pCamera->GetAngle();
	bool bStickMoving = false;

	//形態ごとの速度決定
	float fSpeed = HUMAN_SPEED;

	switch (m_Mode)
	{
	case MODE_HUMAN:
		fSpeed = HUMAN_SPEED;
		if (CCrosshair::GetDisp() == true)CCrosshair::SetDisp(false);
		break;
	case MODE_SPIDER:
		fSpeed = SPIDER_SPEED;
		if (CCrosshair::GetDisp() == true)CCrosshair::SetDisp(false);
		break;
	case MODE_CANNON:
		fSpeed = CANNON_SPEED; //動けないようにする
		if (CCrosshair::GetDisp() == false)CCrosshair::SetDisp(true);
		break;
	}

	//クモ型に変形=============================================================
	if (m_bChangeMode == true && m_Mode == MODE_SPIDER)
	{
		m_fModelChange -= 0.007f;
		CameraAngle.y -= 0.1f;

		if (m_fModelChange <= 0.9f)
		{
			pCamera->SetAngle(CameraAngle);
			m_fModelChange = 0.9f;
			m_fCollisionSize = 15.0f;
			m_bChangeMode = false;
		}
	}

	if (m_bChangeMode == true && m_Mode == MODE_CANNON)
	{
		m_fModelChange -= 0.007f;
		CameraAngle.y -= 0.1f;

		if (m_fModelChange <= 0.9f)
		{
			pCamera->SetAngle(CameraAngle);
			m_fModelChange = 0.9f;
			m_fCollisionSize = 15.0f;
			m_bChangeMode = false;
		}
	}

	//人型に変形===============================================================
	if (m_bChangeMode == true && m_Mode == MODE_HUMAN)
	{
		m_fModelChange += 0.007f;

		if (m_fModelChange >= 1.0f)
		{
			m_fModelChange = 1.0f;
			m_fCollisionSize = 25.0f;
			m_bChangeMode = false;
		}
	}

	if (pObj3D != NULL && m_bChangeMode == false)
	{
		if (pInputKeyboard->GetTrigger(DIK_Q) == true && m_Mode == MODE_HUMAN)
		{//クモ型に変形
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 10.0f, m_pos.z), D3DXVECTOR3(2.0f, 2.0f, 0.0f), COLOR_GREEN, 40, 1);
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 10.0f, m_pos.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_BLUE, 40, 1);
			SetMode(MODE_SPIDER);
		}
		else if (pInputKeyboard->GetTrigger(DIK_Q) == true && m_Mode == MODE_SPIDER)
		{//人型に変形
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_GREEN, 40, 1);
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(4.0f, 4.0f, 0.0f), COLOR_BLUE, 40, 1);
			SetMode(MODE_HUMAN);
		}

		if (pInputKeyboard->GetTrigger(DIK_E) == true && m_Mode == MODE_HUMAN)
		{//銃型に変形
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_GREEN, 40, 1);
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(4.0f, 4.0f, 0.0f), COLOR_BLUE, 40, 1);
			SetMode(MODE_CANNON);
		}
		else if (pInputKeyboard->GetTrigger(DIK_E) == true && m_Mode == MODE_CANNON)
		{//人型に変形
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_GREEN, 40, 1);
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(4.0f, 4.0f, 0.0f), COLOR_BLUE, 40, 1);
			SetMode(MODE_HUMAN);
		}

		if (pInputJoypad->GetTrigger(JOYKEY_X) && m_Mode == MODE_HUMAN)
		{//クモ型に変形
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 10.0f, m_pos.z), D3DXVECTOR3(2.0f, 2.0f, 0.0f), COLOR_GREEN, 40, 1);
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 10.0f, m_pos.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_BLUE, 40, 1);
			SetMode(MODE_SPIDER);
		}
		else if (pInputJoypad->GetTrigger(JOYKEY_X) && m_Mode == MODE_SPIDER)
		{//人型に変形
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_GREEN, 40, 1);
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(4.0f, 4.0f, 0.0f), COLOR_BLUE, 40, 1);
			SetMode(MODE_HUMAN);
		}
		
		if (pInputJoypad->GetTrigger(JOYKEY_B) == true && m_Mode == MODE_HUMAN)
		{//銃型に変形
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_GREEN, 40, 1);
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(4.0f, 4.0f, 0.0f), COLOR_BLUE, 40, 1);
			SetMode(MODE_CANNON);
		}
		else if (pInputJoypad->GetTrigger(JOYKEY_B) == true && m_Mode == MODE_CANNON)
		{//人型に変形
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_GREEN, 40, 1);
			CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(m_pos.x, 50.0f, m_pos.z), D3DXVECTOR3(4.0f, 4.0f, 0.0f), COLOR_BLUE, 40, 1);
			SetMode(MODE_HUMAN);
		}

		//銃形態の時弾を発射できる
		if (CCrosshair::GetDisp() == true)
		{
			if (m_Mode == MODE_CANNON && (pScene->GetMode() == CScene::MODE_GAME || pScene->GetMode() == CScene::MODE_TUTORIAL))
			{
				CInputMouse* pInputMouse = CManager::GetInputMouse();
				if (pInputMouse != NULL && pInputKeyboard->GetTrigger(DIK_SPACE) || pInputJoypad->GetTrigger(JOYKEY_RB))
				{
					CCrosshair* pCrosshair = (pScene->GetMode() == CScene::MODE_GAME) ? CGame::GetCrosshair() : CTutorial::GetCrosshair();
					D3DXVECTOR3 crosshairPos = pCrosshair != NULL ? pCrosshair->GetPosition() : CENTER_2DPOS;
					D3DXVECTOR3 aimRay = pCamera->GetRayDirection(crosshairPos.x, crosshairPos.y);

					if (D3DXVec3LengthSq(&aimRay) > 0.0001f)
					{
						D3DXVECTOR3 cameraForward = pCamera->GetPositionR() - pCamera->GetPositionV();
						D3DXVec3Normalize(&cameraForward, &cameraForward);

						D3DXVECTOR3 bulletPos = m_pos + cameraForward * 35.0f;
						bulletPos.y += 45.0f;

						D3DXVECTOR3 aimPoint = pCamera->GetPositionV() + aimRay * 1000.0f;
						D3DXVECTOR3 direction = aimPoint - bulletPos;
						D3DXVec3Normalize(&direction, &direction);

						CBullet::Create(bulletPos, D3DXVECTOR3(8.0f, 8.0f, 0.0f), direction * 55.0f, 120);
					}
				}
			}
		}
		float rotSpeed = 0.15f; //振り向き速度（数値が大きいほど早い）

		//キーボード入力(移動判定)+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (m_Mode != MODE_CANNON &&
			(pScene->GetMode() == CScene::MODE_GAME || pScene->GetMode() == CScene::MODE_TUTORIAL))
		{
			//斜め移動（W + A）
			if ((pInputKeyboard->GetPress(DIK_W) && pInputKeyboard->GetPress(DIK_A)) || (pInputJoypad->GetPress(JOYKEY_UP) && pInputJoypad->GetPress(JOYKEY_LEFT)))
			{
				m_move.x -= fSpeed * (sinf(D3DX_PI + rotCamera.y - D3DX_PI / 4));
				m_move.z -= fSpeed * (cosf(D3DX_PI + rotCamera.y - D3DX_PI / 4));

				RotateTarget(m_rot.y, D3DX_PI + rotCamera.y - D3DX_PI / 4, rotSpeed);
				m_pMotion->Set(MOTIONTYPE_MOVE, 20);

				if (m_Mode == MODE_HUMAN)
				{
					//毎フレームまたはジャンプ/移動時に呼び出し
					D3DXVECTOR3 footPos = m_pos; // プレイヤーの位置

					//足元からジェットを噴射
					CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
						D3DXVECTOR3(footPos.x, footPos.y + 10.0f, footPos.z), JETT, D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
				}
			}
			//斜め移動（W + D）
			else if ((pInputKeyboard->GetPress(DIK_W) && pInputKeyboard->GetPress(DIK_D)) || (pInputJoypad->GetPress(JOYKEY_UP) && pInputJoypad->GetPress(JOYKEY_RIGHT)))
			{
				m_move.x -= fSpeed * (sinf(D3DX_PI + rotCamera.y + D3DX_PI / 4));
				m_move.z -= fSpeed * (cosf(D3DX_PI + rotCamera.y + D3DX_PI / 4));

				RotateTarget(m_rot.y, D3DX_PI + rotCamera.y + D3DX_PI / 4, rotSpeed);
				m_pMotion->Set(MOTIONTYPE_MOVE, 20);

				if (m_Mode == MODE_HUMAN)
				{
					//毎フレームまたはジャンプ/移動時に呼び出し
					D3DXVECTOR3 footPos = m_pos; // プレイヤーの位置

					//足元からジェットを噴射
					CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
						D3DXVECTOR3(footPos.x, footPos.y + 10.0f, footPos.z), JETT, D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
				}
			}
			//斜め移動（S + A）
			else if ((pInputKeyboard->GetPress(DIK_S) && pInputKeyboard->GetPress(DIK_A)) || (pInputJoypad->GetPress(JOYKEY_DOWN) && pInputJoypad->GetPress(JOYKEY_LEFT)))
			{
				m_move.x -= fSpeed * (sinf(D3DX_PI + rotCamera.y - (D3DX_PI / 2 + D3DX_PI / 4)));
				m_move.z -= fSpeed * (cosf(D3DX_PI + rotCamera.y - (D3DX_PI / 2 + D3DX_PI / 4)));

				RotateTarget(m_rot.y, D3DX_PI + rotCamera.y - (D3DX_PI / 2 + D3DX_PI / 4), rotSpeed);
				m_pMotion->Set(MOTIONTYPE_MOVE, 20);

				if (m_Mode == MODE_HUMAN)
				{
					//毎フレームまたはジャンプ/移動時に呼び出し
					D3DXVECTOR3 footPos = m_pos; // プレイヤーの位置

					//足元からジェットを噴射
					CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
						D3DXVECTOR3(footPos.x, footPos.y + 10.0f, footPos.z), JETT, D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
				}
			}
			//斜め移動（S + D）
			else if ((pInputKeyboard->GetPress(DIK_S) && pInputKeyboard->GetPress(DIK_D)) || (pInputJoypad->GetPress(JOYKEY_DOWN) && pInputJoypad->GetPress(JOYKEY_RIGHT)))
			{
				m_move.x -= fSpeed * (sinf(D3DX_PI + rotCamera.y + (D3DX_PI / 2 + D3DX_PI / 4)));
				m_move.z -= fSpeed * (cosf(D3DX_PI + rotCamera.y + (D3DX_PI / 2 + D3DX_PI / 4)));

				RotateTarget(m_rot.y, D3DX_PI + rotCamera.y + (D3DX_PI / 2 + D3DX_PI / 4), rotSpeed);
				m_pMotion->Set(MOTIONTYPE_MOVE, 20);

				if (m_Mode == MODE_HUMAN)
				{
					//毎フレームまたはジャンプ/移動時に呼び出し
					D3DXVECTOR3 footPos = m_pos; // プレイヤーの位置

					//足元からジェットを噴射
					CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
						D3DXVECTOR3(footPos.x, footPos.y + 10.0f, footPos.z), JETT, D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
				}
			}
			//前進（W）
			else if (pInputKeyboard->GetPress(DIK_W) || pInputJoypad->GetPress(JOYKEY_UP))
			{
				m_move.x -= fSpeed * (sinf(D3DX_PI + rotCamera.y));
				m_move.z -= fSpeed * (cosf(D3DX_PI + rotCamera.y));

				RotateTarget(m_rot.y, D3DX_PI + rotCamera.y, rotSpeed);
				m_pMotion->Set(MOTIONTYPE_MOVE, 20);

				if (m_Mode == MODE_HUMAN)
				{
					//毎フレームまたはジャンプ/移動時に呼び出し
					D3DXVECTOR3 footPos = m_pos; //プレイヤーの位置

					//足元からジェットを噴射
					CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
						D3DXVECTOR3(footPos.x, footPos.y + 10.0f, footPos.z), JETT, D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
				}
			}
			//後退（S）
			else if (pInputKeyboard->GetPress(DIK_S) || pInputJoypad->GetPress(JOYKEY_DOWN))
			{
				m_move.x -= fSpeed * (sinf(rotCamera.y));
				m_move.z -= fSpeed * (cosf(rotCamera.y));

				RotateTarget(m_rot.y, rotCamera.y, rotSpeed);
				m_pMotion->Set(MOTIONTYPE_MOVE, 20);

				if (m_Mode == MODE_HUMAN)
				{
					//毎フレームまたはジャンプ/移動時に呼び出し
					D3DXVECTOR3 footPos = m_pos; //プレイヤーの位置

					//足元からジェットを噴射
					CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
						D3DXVECTOR3(footPos.x, footPos.y + 10.0f, footPos.z), JETT, D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
				}
			}
			//左移動（A）
			else if (pInputKeyboard->GetPress(DIK_A) || pInputJoypad->GetPress(JOYKEY_LEFT))
			{
				m_move.x -= fSpeed * (sinf(D3DX_PI + rotCamera.y - D3DX_PI / 2));
				m_move.z -= fSpeed * (cosf(D3DX_PI + rotCamera.y - D3DX_PI / 2));

				RotateTarget(m_rot.y, D3DX_PI + rotCamera.y - D3DX_PI / 2, rotSpeed);
				m_pMotion->Set(MOTIONTYPE_MOVE, 20);

				if (m_Mode == MODE_HUMAN)
				{
					//毎フレームまたはジャンプ/移動時に呼び出し
					D3DXVECTOR3 footPos = m_pos; // プレイヤーの位置

					//足元からジェットを噴射
					CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
						D3DXVECTOR3(footPos.x, footPos.y + 10.0f, footPos.z), JETT, D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
				}
			}
			//右移動（D）
			else if (pInputKeyboard->GetPress(DIK_D) || pInputJoypad->GetPress(JOYKEY_RIGHT))
			{
				m_move.x -= fSpeed * (sinf(D3DX_PI + rotCamera.y + D3DX_PI / 2));
				m_move.z -= fSpeed * (cosf(D3DX_PI + rotCamera.y + D3DX_PI / 2));

				RotateTarget(m_rot.y, D3DX_PI + rotCamera.y + D3DX_PI / 2, rotSpeed);
				m_pMotion->Set(MOTIONTYPE_MOVE, 20);

				if (m_Mode == MODE_HUMAN)
				{
					//毎フレームまたはジャンプ/移動時に呼び出し
					D3DXVECTOR3 footPos = m_pos; //プレイヤーの位置

					//足元からジェットを噴射
					CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
						D3DXVECTOR3(footPos.x, footPos.y + 10.0f, footPos.z), JETT, D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
				}
			}
		}

		// 左スティックはデジタル方向キーと重ねず、強さを保ってカメラ基準で移動する。
		const bool bDigitalMove =
			pInputKeyboard->GetPress(DIK_W) || pInputKeyboard->GetPress(DIK_A) ||
			pInputKeyboard->GetPress(DIK_S) || pInputKeyboard->GetPress(DIK_D) ||
			pInputJoypad->GetPress(JOYKEY_UP) || pInputJoypad->GetPress(JOYKEY_DOWN) ||
			pInputJoypad->GetPress(JOYKEY_LEFT) || pInputJoypad->GetPress(JOYKEY_RIGHT);
		if (!bDigitalMove && m_Mode != MODE_CANNON &&
			(pScene->GetMode() == CScene::MODE_GAME || pScene->GetMode() == CScene::MODE_TUTORIAL))
		{
			int stickX = 0;
			int stickY = 0;
			if (pInputJoypad->GetStickLeft(&stickX, &stickY))
			{
				const float axisX = static_cast<float>(stickX) / 32767.0f;
				const float axisY = static_cast<float>(stickY) / 32767.0f;
				const float magnitude = sqrtf(axisX * axisX + axisY * axisY);
				const float deadZone = 0.2f;
				if (magnitude > deadZone)
				{
					const float clampedMagnitude = (magnitude > 1.0f) ? 1.0f : magnitude;
					const float scaledMagnitude = (clampedMagnitude - deadZone) / (1.0f - deadZone);
					const float scale = scaledMagnitude / magnitude;
					const float moveX = axisX * scale;
					const float moveY = axisY * scale;

					m_move.x += fSpeed * (moveX * cosf(rotCamera.y) + moveY * sinf(rotCamera.y));
					m_move.z += fSpeed * (moveY * cosf(rotCamera.y) - moveX * sinf(rotCamera.y));
					RotateTarget(m_rot.y, D3DX_PI + rotCamera.y + atan2f(moveX, moveY), rotSpeed);
					m_pMotion->Set(MOTIONTYPE_MOVE, 20);
					bStickMoving = true;

					if (m_Mode == MODE_HUMAN)
					{
						CParticle::Create(CEffect::TEX_CIRCLE, CParticle::TYPE_JETT,
							D3DXVECTOR3(m_pos.x, m_pos.y + 10.0f, m_pos.z), JETT,
							D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), 3, JETT_NUM);
					}
				}
			}
		}

		//ジャンプ判定
		if (m_Mode != MODE_CANNON)
		{
			if ((pInputKeyboard->GetTrigger(DIK_SPACE) == true && m_bLand == false) || (pInputJoypad->GetTrigger(JOYKEY_A) && m_bLand == false))
			{
				m_move.y += 10.0f;
				m_bLand = true;
			}
		}
	}

	//モーション／アクション判定
	if (m_bLand == true)
	{
		m_pMotion->Set(MOTIONTYPE_JUMP, 0);
	}

	//移動量の適用と重力処理
	if (m_bClimbingWall)
	{
		m_rot.x = 1.57f;
		m_bLand = false;
	}
	else if(m_Mode != MODE_CANNON)
	{
		m_move.y -= 0.5f;
		m_rot.x = 0.0f;
	}

	//実際の座標更新
	m_pos += m_move;

	//リセット
	m_bClimbingWall = false;

	if (pScene->GetMode() != CScene::MODE_GAME)
	{
		for (int nCntField = 0; pObj3D != NULL && nCntField < CObject3D::GetNumAll(); nCntField++)
		{
			if (pObj3D[nCntField] != NULL)
			{
				if (pObj3D[nCntField]->CollisionPlayer() == true && m_pos.y <= pObj3D[nCntField]->GetHeight() && pObj3D[nCntField]->GetType() == CObject3D::TYPE_FLOOR)
				{
					m_pos.y = pObj3D[nCntField]->GetHeight();
					m_move.y = 0.0f;
					m_bLand = false;
				}
			}
		}
	}
	else
	{
		CField *pField = CGame::GetField();
		if (pField != NULL)
		{
			if (pField->CollisionPlayer() == true && m_pos.y <= pField->GetHeight() && pField->GetType() == CObject3D::TYPE_FLOOR)
			{
				m_pos.y = pField->GetHeight();
				m_move.y = 0.0f;
				m_bLand = false;
			}
		}
	}

	//移動量の減衰
	m_move.x += (0.0f - m_move.x) * 0.08f;
	m_move.z += (0.0f - m_move.z) * 0.08f;

	if (m_Mode == MODE_CANNON)
	{
		m_rot = pCamera->GetRotation();
	}

	if (pInputKeyboard->GetPress(DIK_W) == false &&
		pInputKeyboard->GetPress(DIK_A) == false &&
		pInputKeyboard->GetPress(DIK_S) == false &&
		pInputKeyboard->GetPress(DIK_D) == false &&
		pInputKeyboard->GetPress(DIK_Q) == false &&
		pInputJoypad->GetPress(JOYKEY_LEFT) == false &&
		pInputJoypad->GetPress(JOYKEY_RIGHT) == false &&
		pInputJoypad->GetPress(JOYKEY_UP) == false &&
		pInputJoypad->GetPress(JOYKEY_DOWN) == false &&
		m_bLand == false && !bStickMoving)
	{
		m_pMotion->Set(MOTIONTYPE_NEUTRAL, 20);
	}

	//モーションの更新処理
	m_pMotion->Update();

	if (pDebug != NULL)
	{
		pDebug->PrintDebugProc("プレイヤーのモード変更\n++人型時[1:クモ型に変形]++\n++クモ型時[1:人型に変形]++\n");

		if (m_Mode == MODE_HUMAN)
		{
			pDebug->PrintDebugProc("[人型]\n");
		}
		else if (m_Mode == MODE_SPIDER)
		{
			pDebug->PrintDebugProc("[クモ型]\n");
		}
		else if (m_Mode == MODE_CANNON)
		{
			pDebug->PrintDebugProc("[銃型]\n");
		}

		pDebug->PrintDebugProc("プレイヤーの座標[X:%.1f,|Y:%.1f|%.1f]\n", m_pos.x, m_pos.y, m_pos.z);
		pDebug->PrintDebugProc("プレイヤーの向き[X:%.2f,|Y:%.2f|%.2f]\n", m_rot.x, m_rot.y, m_rot.z);
		pDebug->PrintDebugProc("プレイヤーモデル数:%d\n", m_nNumModel);
		pDebug->PrintDebugProc("エフェクトの総数:%d\n", CEffect::GetNumAll());
	}
}

//=============================================================================
//モードの設定
void CPlayer::SetMode(const MODE mode)
{
	m_Mode = mode;
	m_bChangeMode = true;

	//一応変型音
	CSound::PlaySound(CSound::SOUND_LASBEL_TRANS000);

	//既存のモデルを安全に破棄
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL)
		{
			m_apModel[nCntModel]->Uninit();
			delete m_apModel[nCntModel];
			m_apModel[nCntModel] = NULL;
		}
	}
	m_nNumModel = 0; //解放後にモデル数をリセット
	memset(m_apModel, 0, sizeof(m_apModel));
	for (int nModelInfo = 0; nModelInfo < sizeof(m_aModelInfo) / sizeof(m_aModelInfo[0]); ++nModelInfo)
	{
		m_aModelInfo[nModelInfo] = MODEL_INFO{};
	}

	//既存のモーションを破棄
	if (m_pMotion != NULL)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}

	//モードに応じたスクリプト・モーション情報の読み込み
	switch (mode)
	{
	case MODE_HUMAN:
		LoadParentModel(OFSET_HUMAN);
		LoadMotionInfo(MOTION_HUMAN);
		break;

	case MODE_SPIDER:
		LoadParentModel(OFSET_SPIDER);
		LoadMotionInfo(MOTION_SPIDER);
		break;

	case MODE_CANNON:
		LoadParentModel(OFSET_SPIDER);
		LoadMotionInfo(MOTION_SPIDER);
		break;
	}

	//新しいモデルのインスタンスの生成と階層構造
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		m_apModel[nCntModel] = CModel::Create(
			m_aModelInfo[nCntModel].FileName,
			m_aModelInfo[nCntModel].pos,
			m_aModelInfo[nCntModel].rot);

		if (m_apModel[nCntModel] == NULL)
		{
			continue;
		}

		const int parentIndex = m_aModelInfo[nCntModel].nIdxParent;
		if (parentIndex < 0 || parentIndex >= nCntModel || m_apModel[parentIndex] == NULL)
		{
			m_apModel[nCntModel]->SetParent(NULL);
		}
		else
		{
			m_apModel[nCntModel]->SetParent(m_apModel[parentIndex]);
		}
	}

	//モーションの再設定とモデル設定
	m_pMotion = new CMotion;

	if (m_pMotion != NULL)
	{//モデルを設定
		m_pMotion->SetModel(m_apModel, m_nNumModel);

		for (int nCntInfo = 0; nCntInfo < MOTIONTYPE_MAX; nCntInfo++)
		{
			m_pMotion->SetInfo(m_aMotionInfo[nCntInfo]);
		}

		//ニュートラルモーションのセット
		m_pMotion->Set(MOTIONTYPE_NEUTRAL, 3);
	}
}

//=============================================================================
//影の描画
void CPlayer::DrawShadow(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
	D3DXMATRIX originalWorld;
	pDevice->GetTransform(D3DTS_WORLD, &originalWorld);

	DWORD oldAlphaBlend, oldSrcBlend, oldDestBlend, oldZWrite, oldLighting, oldColorVertex;
	DWORD oldDepthBias, oldSlopeScaleDepthBias;
	pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlend);
	pDevice->GetRenderState(D3DRS_SRCBLEND, &oldSrcBlend);
	pDevice->GetRenderState(D3DRS_DESTBLEND, &oldDestBlend);
	pDevice->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWrite);
	pDevice->GetRenderState(D3DRS_LIGHTING, &oldLighting);
	pDevice->GetRenderState(D3DRS_COLORVERTEX, &oldColorVertex);
	pDevice->GetRenderState(D3DRS_DEPTHBIAS, &oldDepthBias);
	pDevice->GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, &oldSlopeScaleDepthBias);

	//減算合成(大事)
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);


	D3DMATERIAL9 oldMaterial;
	pDevice->GetMaterial(&oldMaterial);
	LPDIRECT3DBASETEXTURE9 oldTexture = NULL;
	pDevice->GetTexture(0, &oldTexture);

	D3DXVECTOR4 lightDir(-30.0f, 20.0f, 10.0f, 0.0f);
	D3DXVECTOR3 planePoint(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 planeNormal(0.0f, 1.0f, 0.0f);
	CField* pField = NULL;
	CScene* pScene = CManager::GetScene();

	if (pScene != NULL && pScene->GetMode() == CScene::MODE_GAME)
	{
		pField = CGame::GetField();
	}
	if (pField != NULL)
	{
		planePoint = pField->GetPlanePoint();
		planePoint.y += 1.0f;
		planeNormal = pField->GetPlaneNormal();
	}

	D3DXPLANE plane;
	D3DXPlaneFromPointNormal(&plane, &planePoint, &planeNormal);
	D3DXMATRIX shadowMatrix;
	D3DXMatrixShadow(&shadowMatrix, &lightDir, &plane);

	float slopeBias = 1.0f;
	float depthBias = -0.00001f;
	pDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *reinterpret_cast<DWORD*>(&slopeBias));
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, *reinterpret_cast<DWORD*>(&depthBias));
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
	pDevice->SetTexture(0, NULL);

	D3DMATERIAL9 shadowMaterial;
	ZeroMemory(&shadowMaterial, sizeof(shadowMaterial));
	shadowMaterial.Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.45f);
	pDevice->SetMaterial(&shadowMaterial);

	// Parent parts are created before their children, so each part can resolve
	// its current world matrix while drawing the projected shadow.
	for (int nCntModel = 0; nCntModel < m_nNumModel; ++nCntModel)
	{
		if (m_apModel[nCntModel] != NULL)
		{
			m_apModel[nCntModel]->DrawShadow(shadowMatrix);
		}
	}

	pDevice->SetTransform(D3DTS_WORLD, &originalWorld);
	pDevice->SetMaterial(&oldMaterial);
	pDevice->SetTexture(0, oldTexture);
	if (oldTexture != NULL)
	{
		oldTexture->Release();
	}
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlend);
	pDevice->SetRenderState(D3DRS_SRCBLEND, oldSrcBlend);
	pDevice->SetRenderState(D3DRS_DESTBLEND, oldDestBlend);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, oldZWrite);
	pDevice->SetRenderState(D3DRS_LIGHTING, oldLighting);
	pDevice->SetRenderState(D3DRS_COLORVERTEX, oldColorVertex);
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, oldDepthBias);
	pDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, oldSlopeScaleDepthBias);

	//減算合成をリセット
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//=============================================================================
//壁登り中のY軸移動速度を設定する関数
void CPlayer::ClimbUp(float speed)
{
	if (m_pos.y <= 560.0f)
	{
		m_bClimbingWall = true;		//壁登り状態にする
		m_move.y = speed;			//Y軸移動量(速度)を直接セット
	}
	else
	{//560.0fを超えたら速度を0にして上限でストップさせる
		m_pos.y = 560.0f;
		m_move.y = 0.0f;
	}
}

//=============================================================================
// 目標の角度へ向かって慣性回転させる関数
void CPlayer::RotateTarget(float& currentRotY, float targetRotY, float speed)
{
	float diff = targetRotY - currentRotY;

	//-PI～PIに正規化
	while (diff > D3DX_PI)
	{
		diff -= D3DX_PI * 2.0f;
	}
	while (diff < -D3DX_PI)
	{
		diff += D3DX_PI * 2.0f;
	}

	currentRotY += diff * speed;

	// 範囲を修正
	if (currentRotY > D3DX_PI)
	{
		currentRotY -= D3DX_PI * 2.0f;
	}
	else if (currentRotY < -D3DX_PI)
	{
		currentRotY += D3DX_PI * 2.0f;
	}
}

//=============================================================================
//階層構造モデル読み込み処理
HRESULT CPlayer::LoadParentModel(const std::string& fileName)
{
	FILE* pFile = fopen(fileName.c_str(), "r");

	if (pFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};		    // 文字列読み込み
	static char aStrCpy[MAX_STRING] = {};	    // 文字列複製(整理)
	static char* pStart = NULL;				    // 文字列開始位置
	static char aModelPath[FILE_MAX] = {};		// モデルのファイル名読み込み
	int nIdx = 0;								// モデルのインデックス読み込み
	int nParent = 0;							// モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};						// 位置読み込み
	D3DXVECTOR3 rot = {};						// 向き読み込み
	int nCntParts = 0;							// パーツ数カウント
	int nCntFileName = 0;						// ファイル名カウント
	int nCntModel = 0;							// モデル数

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));			// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));				// 文字列クリア
		memset(aStrCpy, 0, sizeof(aStrCpy));		// コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pFile);		// 一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);	// 有効文字だけ抜き取って複製

		if (strstr(aStr, LOAD_NUMMODEL))
		{// NUM_MODELを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%d", &nCntModel);
		}

		if (strstr(aStr, LOAD_MODEL))
		{// MODEL_FILENAMEを読み込んだ

			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%s", &aModelPath);

			LoadParentFile(aModelPath, nCntFileName, nCntModel);
			nCntFileName++;
		}

		if (strstr(aStr, LOAD_PLAYER))
		{// CHARACTERSETを読み込んだ
			while (true)
			{
				memset(aStr, 0, sizeof(aStr));				// 文字列クリア
				memset(aStrCpy, 0, sizeof(aStrCpy));		// コピーもクリア
				(void)fgets(aStr, sizeof(aStr), pFile);		// 一列読み込み
				LoadEnableString(&aStrCpy[0], &aStr[0]);	// 有効文字だけ抜き取って複製

				if (strcmp(aStrCpy, LOAD_PARTS) == 0)
				{// PARTSSETを読み込んだ
					while (true)
					{
						memset(aStr, 0, sizeof(aStr));
						memset(aStrCpy, 0, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_INDEX))
						{// INDEXを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nIdx);
							continue;
						}

						if (strstr(aStr, LOAD_PARENT))
						{// PARENTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nParent);
							continue;
						}

						if (strstr(aStr, LOAD_POS))
						{// POSを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);
							continue;
						}

						if (strstr(aStr, LOAD_ROT))
						{// ROTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &rot.x, &rot.y, &rot.z);
							continue;
						}

						if (strcmp(aStrCpy, LOAD_ENDPARTS) == 0)
						{// END_PARTSSETを読み込んだ
							LoadOffSet(pos, rot, nParent, nIdx);
							nCntParts++;

							//値をクリア
							memset(&pos, NULL, sizeof(D3DXVECTOR3));
							memset(&rot, NULL, sizeof(D3DXVECTOR3));
							nIdx = 0;
							nParent = 0;
							break;
						}
					}
				}

				if (strcmp(aStrCpy, LOAD_ENDPLAYER) == 0)
				{// END_CHARACTERSETを読み込んだ
					nCntParts = 0;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pFile);
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//階層構造のモデルファイルをロード
void CPlayer::LoadParentFile(const std::string& fileName, int nIdxFile, int nMaxModel)
{
	if (nIdxFile < nMaxModel)
	{
		m_aModelInfo[nIdxFile].FileName = fileName;
	}
}

//=============================================================================
//オフセット読み込み処理
void CPlayer::LoadOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxParent, int nIdxModel)
{
	const int modelCapacity = sizeof(m_aModelInfo) / sizeof(m_aModelInfo[0]);
	if (m_nNumModel < 0 || m_nNumModel >= modelCapacity)
	{
		return;
	}

	int nIdx = m_nNumModel;

	m_aModelInfo[nIdx].nIdxModel = nIdxModel;
	m_aModelInfo[nIdx].nIdxParent = nIdxParent;
	m_aModelInfo[nIdx].pos = pos;
	m_aModelInfo[nIdx].rot = rot;

	m_nNumModel++;
}

//=============================================================================
//モーション情報読み込み処理
HRESULT CPlayer::LoadMotionInfo(const std::string& fileName)
{
	FILE* pMotionFile = fopen(fileName.c_str(), "r");

	if (pMotionFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};					// 文字列読み込み
	static char aStrCpy[MAX_STRING] = {};				// 文字列複製(整理)
	static char* pStart = NULL;						// 文字列開始位置
	static char aModelPath[FILE_MAX] = {};			// モデルのファイル名読み込み
	int nIdx = 0;								// モデルのインデックス読み込み
	int nParent = 0;							// モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};						// 位置読み込み
	D3DXVECTOR3 rot = {};						// 向き読み込み
	int nNumModel = 0;							// モデル数読み込み
	int nCntMotion = 0;							// モーション数カウント
	int bLoop = false;							// ループするかどうか読み込み
	int nNumKey = 0;							// キー数読み込み
	static CMotion::KEY_INFO KeyInfo[MAX_KEY] = {};	// キー情報読み込み
	int nCntKey = 0;							// キー数カウント
	int nCntParts = 0;							// パーツ数カウント

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pMotionFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pMotionFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));
		memset(aStrCpy, NULL, sizeof(aStrCpy));
		(void)fgets(aStr, sizeof(aStr), pMotionFile);
		LoadEnableString(&aStrCpy[0], &aStr[0]);

		if (strcmp(aStrCpy, LOAD_MOTIONINFO) == 0)
		{// MOTIONSETを読み込んだ
			while (true)
			{
				memset(aStr, NULL, sizeof(aStr));
				memset(aStrCpy, NULL, sizeof(aStrCpy));
				(void)fgets(aStr, sizeof(aStr), pMotionFile);
				LoadEnableString(&aStrCpy[0], &aStr[0]);

				if (strstr(aStr, LOAD_MOTIONLOOP))
				{// LOOPを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &bLoop);
					continue;
				}

				if (strstr(aStr, LOAD_NUMKEY))
				{// NUM_KEYを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &nNumKey);
					continue;
				}

				if (strcmp(aStrCpy, LOAD_KEYSET) == 0)
				{// KEYSETを読み込んだ
					while (true)
					{
						memset(aStr, NULL, sizeof(aStr));
						memset(aStrCpy, NULL, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pMotionFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_FRAME))
						{// FRAMEを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &KeyInfo[nCntKey].nFrame);
							continue;
						}

						if (strcmp(aStrCpy, LOAD_KEY) == 0)
						{// KEYを読み込んだ
							while (true)
							{
								memset(aStr, NULL, sizeof(aStr));
								memset(aStrCpy, NULL, sizeof(aStrCpy));
								(void)fgets(aStr, sizeof(aStr), pMotionFile);
								LoadEnableString(&aStrCpy[0], &aStr[0]);

								if (strstr(aStr, LOAD_POS))
								{// POSを読み込んだ
									if ((pStart = strchr(aStr, '=')) == NULL)
									{
										continue;
									}

									(void)sscanf(pStart + 1, "%f %f %f", &KeyInfo[nCntKey].aKey[nCntParts].fPosX, &KeyInfo[nCntKey].aKey[nCntParts].fPosY, &KeyInfo[nCntKey].aKey[nCntParts].fPosZ);
									continue;
								}

								if (strstr(aStr, LOAD_ROT))
								{// ROTを読み込んだ
									if ((pStart = strchr(aStr, '=')) == NULL)
									{
										continue;
									}

									(void)sscanf(pStart + 1, "%f %f %f", &KeyInfo[nCntKey].aKey[nCntParts].fRotX, &KeyInfo[nCntKey].aKey[nCntParts].fRotY, &KeyInfo[nCntKey].aKey[nCntParts].fRotZ);
									continue;
								}

								if (strcmp(aStrCpy, LOAD_ENDKEY) == 0)
								{// END_KEYを読み込んだ
									nCntParts++;
									break;
								}
							}
						}

						if (strcmp(aStrCpy, LOAD_ENDKEYSET) == 0)
						{// END_KEYSET
							nCntKey++;
							nCntParts = 0;
							break;
						}
					}
				}

				if (strcmp(aStrCpy, LOAD_ENDMOTIONINFO) == 0)
				{// END_MOTIONSETを読み込んだ
					LoadMotion(bLoop, nNumKey, &KeyInfo[0], nCntMotion);
					nCntMotion++;

					// 各種変数初期化
					memset(&KeyInfo, NULL, sizeof(CMotion::KEY_INFO));
					bLoop = false;
					nNumKey = 0;
					nCntKey = 0;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pMotionFile);
			break;
		}
	}
	return S_OK;
}

//=============================================================================
//	モーション読み込み処理
void CPlayer::LoadMotion(bool bLoop, int nNumKey, CMotion::KEY_INFO* pKeyInfo, int nMotion)
{
	if (nMotion < MOTIONTYPE_MAX)
	{
		m_aMotionInfo[nMotion].bLoop = bLoop;
		m_aMotionInfo[nMotion].nNumKey = nNumKey;

		for (int nCntKey = 0; nCntKey < m_aMotionInfo[nMotion].nNumKey; nCntKey++)
		{
			m_aMotionInfo[nMotion].aKeyInfo[nCntKey] = pKeyInfo[nCntKey];
		}
	}
}

//=============================================================================
//有効文字抜き出し処理
void CPlayer::LoadEnableString(char* aStrCpy, char* pStart)
{
	while (true)
	{
		if (*pStart == '\n')
		{
			pStart++;
			continue;
		}
		else if (*pStart == '\t')
		{
			pStart++;
			continue;
		}
		else if (*pStart == 0x32)
		{
			pStart++;
			continue;
		}
		else if (*pStart == '#' || *pStart == '\0')
		{
			break;
		}
		else
		{
			strncat(aStrCpy, pStart, 1);
			pStart++;
		}
	}
}
