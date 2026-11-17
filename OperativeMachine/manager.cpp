//=============================================================================
//
//マネージャー[manager.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "fade.h"
#include "object.h"
#include "input.h"
#include "debugproc.h"
#include "color.h"
#include "sound.h"
#include "camera.h"
#include "light.h"
#include "texture.h"
#include "pause.h"
#include "ModelManager.h"

//静的メンバ変数宣言-----------------------------------------------------------
//システム系
CScene* CManager::m_pScene = NULL;
CRenderer* CManager::m_pRenderer = NULL;
CFade* CManager::m_pFade = NULL;
CDebugproc* CManager::m_pDebug = NULL;
CModelManager* CManager::m_pModelManager = NULL;

//カメラ
CCamera* CManager::m_pCamera = NULL;
CLight* CManager::m_pLight = NULL;
CTexture* CManager::m_pTexture = NULL;
CSound* CManager::m_pSound = NULL;

//入力系
CInputKeyboard* CManager::m_pInputKeyboard = NULL;
CInputMouse* CManager::m_pInputMouse = NULL;
CInputJoypad* CManager::m_pInputJoypad = NULL;

bool CManager::m_bPause = false;
bool CManager::m_bFade = false;

//=============================================================================
//コンストラクタ
CManager::CManager()
{
}

//=============================================================================
//デストラクタ
CManager::~CManager()
{
}

//=============================================================================
//マネージャーの初期化
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	if (m_pCamera == NULL)
	{//カメラクラスのポインタ
		m_pCamera = new CCamera;

		if (m_pCamera != NULL)
		{
			//カメラの初期化処理
			m_pCamera->Init();
		}
	}

	if (m_pTexture == NULL)
	{
		m_pTexture = new CTexture;
		if (m_pTexture != NULL)
		{
			m_pTexture->Load();
		}
	}

	if (m_pRenderer == NULL)
	{//レンダラ－クラスのポインタ
		m_pRenderer = new CRenderer;

		if (m_pRenderer != NULL)
		{
			//レンダラーの初期化処理
			m_pRenderer->Init(hWnd, bWindow);
		}
	}

	if (m_pModelManager == NULL)
	{
		m_pModelManager = new CModelManager;
	}

	if (m_pSound == NULL)
	{//サウンドクラスのポインタ
		m_pSound = new CSound;

		if (m_pSound != NULL)
		{
			m_pSound->Init(hWnd);
		}
	}

	if (m_pLight == NULL)
	{//ライトクラスのポインタ
		m_pLight = new CLight;

		if (m_pLight != NULL)
		{
			//ライトの初期化処理
			m_pLight->Init();
		}
	}
	
	if (m_pDebug == NULL)
	{//デバッグクラスのポインタ
		m_pDebug = new CDebugproc;

		if (m_pDebug != NULL)
		{
			//レンダラーの初期化処理
			m_pDebug->Init();
		}
	}

	if (m_pInputKeyboard == NULL)
	{//キーボードクラスのポインタ
		m_pInputKeyboard = new CInputKeyboard;

		if (m_pInputKeyboard != NULL)
		{
			m_pInputKeyboard->Init(hInstance, hWnd);
		}
	}

	if (m_pInputMouse == NULL)
	{//マウスクラスのポインタ
		m_pInputMouse = new CInputMouse;

		if (m_pInputMouse != NULL)
		{
			m_pInputMouse->Init(hInstance, hWnd);
		}
	}

	if (m_pInputJoypad == NULL)
	{//マウスクラスのポインタ
		m_pInputJoypad = new CInputJoypad;

		if (m_pInputJoypad != NULL)
		{
			m_pInputJoypad->Init(hInstance, hWnd);
		}
	}

	//シーン設定
	m_pScene = CScene::Create(CScene::MODE_TITLE);

	if (m_pFade == NULL)
	{
		m_pFade = new CFade;

		if (m_pFade != NULL)
		{
			//レンダラーの初期化処理
			m_pFade->Init(m_pScene->GetMode());
		}
	}

	return S_OK;
}

//=============================================================================
//マネージャーの終了
void CManager::Uninit(void)
{
	if (m_pRenderer != NULL)
	{
		//レンダラーの終了処理
		m_pRenderer->Uninit();

		//破棄
		delete m_pRenderer;
		m_pRenderer = NULL;
	}

	if (m_pScene != NULL)
	{
		m_pScene->Uninit();

		//破棄
		delete m_pScene;
		m_pScene = NULL;
	}

	if (m_pFade != NULL)
	{
		//フェードの終了処理
		m_pFade->Uninit();

		//破棄
		delete m_pFade;
		m_pFade = NULL;
	}

	if (m_pTexture != NULL)
	{
		//テクスチャのアンロード
		m_pTexture->Unload();

		//破棄
		delete m_pTexture;
		m_pTexture = NULL;
	}

	if (m_pModelManager != NULL)
	{
		//テクスチャのアンロード
		m_pModelManager->Unload();

		//破棄
		delete m_pModelManager;
		m_pModelManager = NULL;
	}

	if (m_pSound != NULL)
	{
		m_pSound->Uninit();

		//破棄
		delete m_pSound;
		m_pSound = NULL;
	}

	if (m_pCamera != NULL)
	{
		//カメラの終了処理
		m_pCamera->Uninit();

		//破棄
		delete m_pCamera;
		m_pCamera = NULL;
	}

	if (m_pLight != NULL)
	{//ライトの終了処理
		m_pLight->Uninit();

		//破棄
		delete m_pLight;
		m_pLight = NULL;
	}

	if (m_pDebug != NULL)
	{//デバッグ表示の終了処理
		m_pDebug->Uninit();

		//破棄
		delete m_pDebug;
		m_pDebug = NULL;
	}

	if (m_pInputKeyboard != NULL)
	{//キーボードの終了処理
		m_pInputKeyboard->Uninit();

		//破棄
		delete m_pInputKeyboard;
		m_pInputKeyboard = NULL;
	}

	if (m_pInputMouse != NULL)
	{//マウスの終了処理
		m_pInputMouse->Uninit();

		//破棄
		delete m_pInputMouse;
		m_pInputMouse = NULL;
	}

	if (m_pInputJoypad != NULL)
	{//ジョイパッドの終了処理
		m_pInputJoypad->Uninit();

		//破棄
		delete m_pInputJoypad;
		m_pInputJoypad = NULL;
	}
}

//=============================================================================
//マネージャーの更新
void CManager::Update(int FPS)
{
	//デバッグ表示情報の取得
	CDebugproc* pDebug = CManager::GetDebugproc();

	pDebug->PrintDebugProc("[FPS : %d]", FPS);

	if (m_pInputKeyboard != NULL)
	{
		//キーボード入力の更新
		m_pInputKeyboard->Update();
	}

	if (m_pDebug != NULL)
	{
		//デバッグ表示の更新処理
		m_pDebug->Update();
	}

	if (m_pInputMouse != NULL)
	{
		//マウス入力の更新
		m_pInputMouse->Update();
	}

	if (m_pInputJoypad != NULL)
	{//ジョイパッドの終了処理
		m_pInputJoypad->Update();
	}

	if (m_pCamera != NULL)
	{
		//カメラの更新
		m_pCamera->Update();
	}

	if (m_pLight != NULL)
	{
		//ライトの終了処理
		m_pLight->Update();
	}

	if (m_pInputKeyboard->GetTrigger(DIK_P) == true || m_pInputJoypad->GetTrigger(JOYKEY_START) == true)
	{//ポーズ
		m_bPause = !m_bPause;
	}

	if (m_pFade != NULL)
	{//フェードの更新
		m_pFade->Update();
	}
	if (m_pScene != NULL)
	{//シーンの更新
		m_pScene->Update();
	}
	if (m_bPause != false || m_bFade != false)
	{//ポーズ状態
	}
	else
	{//ポーズ状態じゃなかったら
			//if (m_pScene != NULL)
				//{//シーンの更新
				//	m_pScene->Update();
				//}

		if (m_pRenderer != NULL)
		{
			//レンダラーの更新処理
			m_pRenderer->Update();
		}
	}
}

//=============================================================================
//マネージャーの描画
void CManager::Draw(void)
{
	if (m_pRenderer != NULL)
	{//レンダラーの描画
		m_pRenderer->Draw();
	}

	if (m_pScene != NULL)
	{//シーンの描画
		m_pScene->Draw();
	}

	//if (m_pFade != NULL)
	//{//フェードの描画
	//	m_pFade->Draw();
	//}
}

//=============================================================================
//モード設定
void CManager::SetMode(CScene::MODE mode)
{
	//現在のモードの破棄
	if (m_pScene != NULL)
	{//シーンの終了
		m_pScene->Uninit();
	}

	//すべてのオブジェクトの破棄
	CObject::ReleaseAll();

	//シーンを作成
	m_pScene = CScene::Create(mode);
}
