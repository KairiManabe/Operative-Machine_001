//=============================================================================
// 
// ビルボード[billboard.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "billboard.h"

#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "texture.h"

#include "camera.h"

//=============================================================================
//コンストラクタ
CBillboard::CBillboard(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	m_pVtxBuff = NULL;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
}

//=============================================================================
//デストラクタ
CBillboard::~CBillboard()
{
}

//=============================================================================
//モデルの生成
CBillboard* CBillboard::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	//プレイヤー
	CBillboard* pBillboard = NULL;

	if (pBillboard == NULL)
	{
		//弾生成
		pBillboard = new CBillboard;

		if (pBillboard != NULL)
		{
			pBillboard->Init(pBillboard->m_pos, pBillboard->m_size);
		}
	}
	return pBillboard;
}

//=============================================================================
//プレイヤーの初期化処理
HRESULT CBillboard::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	CRenderer* pRenderer = CManager::GetRenderer();

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
	
	//各種代入
	m_pos = pos;
	m_size = size;

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx;		//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**) &pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, 0.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, 0.0f, 0.0f);

	//法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
	
	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();

	//影を設定
	//g_nIdxShdowBillboard = SetShadow();

	return S_OK;
}

//=============================================================================
//プレイヤーの描画
void CBillboard::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice;

	CRenderer *pRenderer = CManager::GetRenderer();

	//デバイスの取得
	pDevice = pRenderer->GetDevice();

	D3DXMATRIX mtxTrans;		//計算用マトリックス
	D3DXMATRIX mtxView;			//ビューマトリックス取得用

	// Keep billboard fragments behind opaque geometry occluded.
	DWORD oldZFunc = D3DCMP_LESSEQUAL;
	DWORD oldZWriteEnable = TRUE;
	pDevice->GetRenderState(D3DRS_ZFUNC, &oldZFunc);
	pDevice->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);		//逆行列を求める
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pTexture);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, oldZFunc);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnable);
}

//=============================================================================
//プレイヤーの終了処理
void CBillboard::Uninit(void)
{
	//頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	//破棄
	CObject::Release();
}

//=============================================================================
//ビルボードの更新処理
void CBillboard::Update(void)
{
}

//=============================================================================
//テクスチャの割り当て
void CBillboard::BindTexture(const LPDIRECT3DTEXTURE9 pTexture)
{
	m_pTexture = pTexture;
}