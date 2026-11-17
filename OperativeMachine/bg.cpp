//=============================================================================
// 
//	背景出力[bg.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "bg.h"

#include "manager.h"
#include "renderer.h"
#include "texture.h"



//=============================================================================
//コンストラクタ
CBG::CBG()
{
	//値のクリア
	m_nIdxTexture = -1;
	m_pVtxBuff = NULL;
}

//=============================================================================
//デストラクタ
CBG::~CBG()
{
}

//=============================================================================
//プレイヤーの初期化処理
void CBG::Init(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 D3DDevice = renderer->GetDevice();

	m_nIdxTexture = CManager::GetTexture()->Register("data\\TEXTURE\\images.jpg");

	D3DDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_BG,
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
	pVtx[1].pos = D3DXVECTOR3(BG_W, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, BG_H, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(BG_W, BG_H, 0.0f);

	//rhw
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//カラー
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
//プレイヤーの描画
void CBG::Draw(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 D3DDevice = renderer->GetDevice();

	D3DDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	D3DDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	D3DDevice->SetTexture(0, CManager::GetTexture()->GetAddres(m_nIdxTexture));
	//ポリゴンの描画
	D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
//背景の終了処理
void CBG::Uninit(void)
{
	//テクスチャの破棄
	m_nIdxTexture = -1;

	//頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

}

//=============================================================================
//背景の更新処理
void CBG::Update(void)
{
	//float Speed = 0.1f;

	//VERTEX_2D* pVtx;		//頂点情報へのポインタ

	////頂点バッファをロックし、頂点情報へのポインタを取得
	//g_pVtxBuffBG->Lock(0, 0, (void**)&pVtx, 0);

	//for (int nCnt = 0; nCnt < MAX_BG; nCnt++)
	//{
	//	if (g_screen[nCnt].pos.x <= 1.0f)
	//	{
	//		g_screen[nCnt].pos.x += Speed;
	//	}
	//	else
	//	{
	//		g_screen[nCnt].pos.x -= Speed;
	//	}

	//	//頂点座標
	//	pVtx[0].pos = D3DXVECTOR3(g_screen[nCnt].pos.x, g_screen[nCnt].pos.y, 0.0f);
	//	pVtx[1].pos = D3DXVECTOR3(g_screen[nCnt].pos.x + BG_W, g_screen[nCnt].pos.y, 0.0f);
	//	pVtx[2].pos = D3DXVECTOR3(g_screen[nCnt].pos.x, g_screen[nCnt].pos.y + BG_H, 0.0f);
	//	pVtx[3].pos = D3DXVECTOR3(g_screen[nCnt].pos.x + BG_W, g_screen[nCnt].pos.y + BG_H, 0.0f);

	//	if (g_screen[nCnt].pos.x >= BG_W)
	//	{
	//		g_screen[nCnt].pos.x = g_screen[!nCnt].pos.x - BG_W;
	//	}

	//	pVtx += 4;			//頂点データのポインタを4つ分集める
	//}
	////頂点バッファのアンロック
	//g_pVtxBuffBG->Unlock();
}
