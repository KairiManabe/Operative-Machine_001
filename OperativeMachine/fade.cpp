//=============================================================================
// 
// フェード処理[fade.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "fade.h"

#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"

#include "camera.h"

//静的メンバ変数宣言-----------------------------------------------------------
CScene::MODE CFade::m_modeNext = CScene::MODE_TITLE;

//=============================================================================
//コンストラクタ
CFade::CFade()
{
	m_fade = FADE_NONE;
	m_pVtxBuff = {};
	m_colorFade = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//=============================================================================
//デストラクタ
CFade::~CFade()
{
}

//=============================================================================
//フェードの初期化処理
void CFade::Init(CScene::MODE modeNext)
{
	m_fade = FADE_IN;
	m_modeNext = modeNext;
	m_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);		//黒いポリゴンを不透明にしておく

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;		//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);

	//カラー
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_colorFade.a);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_colorFade.a);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_colorFade.a);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_colorFade.a);

	//rhw
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
//フェードの描画
void CFade::Draw(void)
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);      // ソースは自身のアルファ値を使う
	//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); // デスト（背景）は 1 - アルファ

#if 0
	// 念のためライティング（陰影処理）をオフ
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

		////頂点フォーマットの設定
		//pDevice->SetFVF(FVF_VERTEX_2D);

		////テクスチャの設定
		//pDevice->SetTexture(0, NULL);

		////ポリゴンの描画
		//pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
#endif

	//頂点バッファ・フォーマット・テクスチャの設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));
	pDevice->SetFVF(FVF_VERTEX_2D);
	pDevice->SetTexture(0, NULL);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	/*pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);*/
}

//=============================================================================
//フェード終了処理
void CFade::Uninit(void)
{	
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//=============================================================================
//フェードの更新処理
void CFade::Update(void)
{
	CDebugproc*pDebug = CManager::GetDebugproc();

	VERTEX_2D* pVtx;		//頂点情報へのポインタ

	if (m_fade != FADE_NONE)
	{
		if (m_fade == FADE_IN)
		{//フェードイン状態
			m_colorFade.a -= 0.01f;		//ポリゴンを透明にしていく
			if (m_colorFade.a <= 0.0f)
			{
				m_colorFade.a = 0.0f;
				m_fade = FADE_NONE;		//何もしていない状態にする

				//ポーズ状態を解除する
				CManager::SetFadePause(false);
			}
		}
		else if (m_fade == FADE_OUT)
		{//フェードアウト
			m_colorFade.a += 0.01f;		//ポリゴンを不透明にしていく
			if (m_colorFade.a >= 1.0f)
			{
				m_colorFade.a = 1.0f;
				m_fade = FADE_IN;		//フェードイン状態にする

				//モード設定(次の画面に移行)
				CManager::SetMode(m_modeNext);
			}
		}
		//頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		//カラー
		pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_colorFade.a);
		pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_colorFade.a);
		pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_colorFade.a);
		pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_colorFade.a);

		pDebug->PrintDebugProc("α値:%.1f\n", m_colorFade.a);

		//頂点バッファのアンロック
		m_pVtxBuff->Unlock();
	}

}

//=====================================
//設定
void CFade::SetFade(CScene::MODE modeNext)
{	
	//シーンの取得
	CScene* pScene = CManager::GetScene();
	CFade* pFade = CManager::GetFade();
	CCamera* pCamera = CManager::GetCamera();

	if (pFade != NULL)
	{
		pFade->m_fade = FADE_OUT;
		m_modeNext = modeNext;
		pCamera->Uninit();
		pFade->m_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);		//黒いポリゴンを透明にしておく
		
	}

	//ポーズ状態にする
	CManager::SetFadePause(true);		//キー入力は受け付けちゃうから今度修正<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}