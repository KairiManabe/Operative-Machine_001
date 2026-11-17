//=============================================================================
//
// クロスヘアー[crosshair.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "crosshair.h"

#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "color.h"
#include "texture.h"

#include "scene.h"
#include "tutorial.h"
#include "game.h"

//静的メンバ変数宣言-----------------------------------------------------------
bool CCrosshair::m_bDisp = false;

//=============================================================================
//コンストラクタ
CCrosshair::CCrosshair(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_filename = {};
	m_nIdxTexture = -1;
	m_col = COLOR_NORMAL;
	m_bDisp = false;
}

//=============================================================================
//デストラクタ
CCrosshair::~CCrosshair()
{
}

//=============================================================================
//生成処理
CCrosshair* CCrosshair::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col)
{
	//UI
	CCrosshair* pCrosshair = NULL;

	if (pCrosshair == NULL)
	{
		//UI生成
		pCrosshair = new CCrosshair;

		if (pCrosshair != NULL)
		{
			//各種代入
			pCrosshair->m_pos = pos + D3DXVECTOR3(CROSSHAIR_OFFSET_X, CROSSHAIR_OFFSET_Y, 0.0f);
			pCrosshair->m_size = size;
			pCrosshair->m_col = col;

			//プレイヤー初期化
			pCrosshair->Init(pCrosshair->m_pos, pCrosshair->m_size);
		}
	}
	return pCrosshair;
}

//=============================================================================
//敵の初期化
HRESULT CCrosshair::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	m_filename = "data/TEXTURE/Xhear.png";

	//テクスチャの読み込み
	m_nIdxTexture = CManager::GetTexture()->Register(m_filename.c_str());

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

	//頂点情報の設定(初期化)
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_size.x, m_pos.y - m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_size.x, m_pos.y - m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_size.x, m_pos.y + m_size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_size.x, m_pos.y + m_size.y, 0.0f);

	//頂点カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
	return S_OK;
}

//=============================================================================
//敵の終了
void CCrosshair::Uninit(void)
{
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	CObject::Release();
}

//=============================================================================
//敵の描画
void CCrosshair::Draw(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddres(m_nIdxTexture));

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
//敵の更新
void CCrosshair::Update(void)
{
	VERTEX_2D* pVtx;		//頂点情報へのポインタ

		//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();

	//デバッグ表示
	CManager::GetDebugproc()->PrintDebugProc("\n+++クロスヘアのpos[X:%.2f | Y:%.2f | Z:%.2f]+++\n", m_pos.x, m_pos.y, m_pos.z);
}

//=============================================================================
//表示非表示の設定
void CCrosshair::SetDisp(bool bDisp)
{
	CCrosshair* pCrosshair = NULL;

	if (CManager::GetScene()->GetMode() == CScene::MODE_GAME)
	{//ゲームシーンの時
		pCrosshair = CGame::GetCrosshair();
	}
	else if (CManager::GetScene()->GetMode() == CScene::MODE_TUTORIAL)
	{//チュートリアルシーンの時
		pCrosshair = CTutorial::GetCrosshair();
	}

	if (pCrosshair == NULL)
	{//NULLだったら無視
		return;
	}

	m_bDisp = bDisp;

	if (m_bDisp == false)
	{//非表示
		pCrosshair->SetColor(COLOR_NONE);
	}
	else if (m_bDisp == true)
	{//表示
		pCrosshair->SetColor(COLOR_NORMAL);
	}
}

//=============================================================================
//注視点にクロスヘアの座標を移動
void CCrosshair::SetPoint(D3DXVECTOR3 pos)
{
	m_pos = pos;
	VERTEX_2D* pVtx;		//頂点情報へのポインタ

		//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点情報の設定(初期化)
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_size.x, m_pos.y - m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_size.x, m_pos.y - m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_size.x, m_pos.y + m_size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_size.x, m_pos.y + m_size.y, 0.0f);

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}