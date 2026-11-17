//=============================================================================
//
//レンダラー[renderer.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "renderer.h"
#include "manager.h"
#include "scene.h"
#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "object.h"
#include "number.h"
#include "bg.h"
#include "fade.h"
#include "debugproc.h"
#include "pause.h"

//静的メンバ変数宣言-----------------------------------------------------------
CBG* CRenderer::m_pBG = NULL;

//=============================================================================
//コンストラクタ
CRenderer::CRenderer()
{
	//値をクリア
	m_pD3D = NULL;
	m_pDevice = NULL;
}

//=============================================================================
//デストラクタ
CRenderer::~CRenderer()
{

}

//=============================================================================
//初期化処理
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;				//ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;		//プレゼンテーションパラメータ

	//Direct3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}
	//現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	//デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));								//パラメーターのゼロクリア

	d3dpp.BackBufferWidth = SCREEN_WIDTH;							//ゲーム画面サイズ(幅)

	d3dpp.BackBufferHeight = SCREEN_HEIGHT;							//ゲーム画面サイズ(高さ)

	d3dpp.BackBufferFormat = d3ddm.Format;							//バックバッファの形式

	d3dpp.BackBufferCount = 1;										//バックバッファの数

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;						//ダブルバッファの切り替え(映像信号に同期)

	d3dpp.EnableAutoDepthStencil = TRUE;							//デブスバッファとステンシルバッファを作成

	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;						//デブスバッファとして16bitを使う

	d3dpp.Windowed = bWindow;										//ウィンドウモード

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		//リフレッシュレート

	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		//インターバル

	//Direct3Dデバイスの生成
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pDevice)))
	{
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pDevice)))
		{
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pDevice)))
			{
				return E_FAIL;
			}
		}
	}
	//レンダーステートの設定
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//サンプラーステートの設定
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	//テクスチャステージステートの設定
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//LPDIRECT3DSURFACE9 pRenderDef,pZBuffDef;

	//レンダリングターゲット用テクスチャの生成
	m_pDevice->CreateTexture(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8B8G8R8,
		D3DPOOL_DEFAULT,
		&m_pTextureMT,
		NULL);

	//テクスチャレンダリング用インターフェースの生成
	m_pTextureMT->GetSurfaceLevel(0, &m_pRenderMT);
	
	//テクスチャレンダリング用Zバッファの生成
	m_pDevice->CreateDepthStencilSurface(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pZBuffMT,
		NULL);

	//テクスチャレンダリング用ビューポートを設定
	m_viewportMT.X = 0;
	m_viewportMT.Y = 0;
	m_viewportMT.Width = SCREEN_WIDTH;
	m_viewportMT.Height = SCREEN_HEIGHT;
	m_viewportMT.MinZ = 0.0f;
	m_viewportMT.MaxZ = 0.0f;

	if (m_pBG == NULL)
	{
		//カウンターの生成
		m_pBG = new CBG;

		if (m_pBG != NULL)
		{
			//カウンターの初期化
			m_pBG->Init();
		}
	}

	return S_OK;
}

//=============================================================================
//終了処理
void CRenderer::Uninit(void)
{
	if (m_pBG != NULL)
	{
		//BGの初期化
		m_pBG->Uninit();

		//BGの破棄
		delete m_pBG;
		m_pBG = NULL;
	}

	if (m_pTextureMT != NULL)
	{
		m_pTextureMT->Release();
		m_pTextureMT = NULL;
	}

	if (m_pRenderMT != NULL)
	{
		m_pRenderMT->Release();
		m_pRenderMT = NULL;
	}

	if (m_pZBuffMT != NULL)
	{
		m_pZBuffMT->Release();
		m_pZBuffMT = NULL;
	}

	//Direct3Dデバイスの破棄
	if (m_pDevice != NULL)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	//Direct3Dオブジェクトの破棄
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

//=============================================================================
//更新処理
void CRenderer::Update(void)
{
	CObject::UpdateAll();

	if (m_pBG != NULL)
	{
		//BGの更新
		m_pBG->Update();
	}
}

//=============================================================================
//描画処理
void CRenderer::Draw(void)
{
	CDebugproc* pDebug = CManager::GetDebugproc();

	//シーンの取得
	CScene* pScene = CManager::GetScene();

	//ポーズを取得
	CPause* pPauseGame = CGame::GetPause();
	CPause* pPauseTutorial = CTutorial::GetPause();

	//画面クリア
	m_pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//描画開始
	if (SUCCEEDED(m_pDevice->BeginScene()))
	{//描画開始が成功した場合
	//各種オブジェクトの描画
		CObject::DrawAll();

		//フェードの描画
		CManager::GetFade()->Draw();

		if (pDebug != NULL)
		{//デバッグ表示の描画
			pDebug->Draw();
		}
		if (pPauseGame != NULL && CManager::GetPause() == true && pScene->GetMode() == CScene::MODE_GAME)
		{
			pPauseGame->Draw();
		}
		else if (pPauseTutorial != NULL && CManager::GetPause() == true && pScene->GetMode() == CScene::MODE_TUTORIAL)
		{
			pPauseTutorial->Draw();
		}

		//描画終了
		m_pDevice->EndScene();
	}
	//バックバッファとフロントバッファの入れ替え
	m_pDevice->Present(NULL, NULL, NULL, NULL);
}

//=============================================================================
//レンダリングターゲットの変更
void CRenderer::ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU)
{
	D3DXMATRIX mtxView, mtxProjection;
	float fAspect;

	//レンダリングターゲットを生成したテクスチャに設定
	m_pDevice->SetRenderTarget(0, m_pRenderMT);

	//Zバッファを生成したZバッファに設定
	m_pDevice->SetDepthStencilSurface(m_pZBuffMT);

	//テクスチャレンダリング用のビューポートを設定
	m_pDevice->SetViewport(&m_viewportMT);

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&mtxView);

	//プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&mtxProjection);

	//プロジェクションマトリックスの作成
	fAspect = (float)m_viewportMT.Width / (float)m_viewportMT.Height;
	D3DXMatrixLookAtLH(&mtxView, &posV, &posR, &vecU);

	////プロジェクションマトリックスの設定
	//m_pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);
}