//=============================================================================
//
//2Dオブジェクト[object2D.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "object2D.h"
#include "renderer.h"
#include "manager.h"
#include "number.h"
#include "input.h"
#include "debugproc.h"

//=============================================================================
//コンストラクタ
CObject2D::CObject2D(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	m_pTexture = NULL;
	m_pVtxBuff = NULL;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
}

//=============================================================================
//デストラクタ
CObject2D::~CObject2D()
{
}

//=============================================================================
//オブジェクトの初期化
HRESULT CObject2D::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_pos = pos;
	m_size = size;

	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
//オブジェクトの終了
void CObject2D::Uninit(void)
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
//オブジェクトの更新
void CObject2D::Update(void)
{
	VERTEX_2D* pVtx;		//頂点情報へのポインタ

	////レンダラーの取得
	//CRenderer* renderer = CManager::GetRenderer();

	////デバッグ表示の取得
	//CDebugproc* pDebug = CManager::GetDebugproc();

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点情報の設定(初期化)
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_size.x, m_pos.y - m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_size.x, m_pos.y - m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_size.x, m_pos.y + m_size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_size.x, m_pos.y + m_size.y, 0.0f);

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

#if 0
	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f + (0.25f * m_nAnimNum), 0.0f + (0.5f * m_AnimRot));
	pVtx[1].tex = D3DXVECTOR2(0.25f + (0.25f * m_nAnimNum), 0.0f + (0.5f * m_AnimRot));
	pVtx[2].tex = D3DXVECTOR2(0.0f + (0.25f * m_nAnimNum), 0.5f + (0.5f * m_AnimRot));
	pVtx[3].tex = D3DXVECTOR2(0.25f + (0.25f * m_nAnimNum), 0.5f + (0.5f * m_AnimRot));
#endif

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
//オブジェクトの描画
void CObject2D::Draw(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイスの取得
	LPDIRECT3DDEVICE9 D3DDevice = renderer->GetDevice();

	D3DDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	D3DDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	D3DDevice->SetTexture(0, m_pTexture);

	//ポリゴンの描画
	D3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
//テクスチャの割り当て
void CObject2D::BindTexture(const LPDIRECT3DTEXTURE9 pTexture)
{
	m_pTexture = pTexture;
}

//=============================================================================
//ポジション設定
void CObject2D::SetPosition(const D3DXVECTOR3 pos)
{
	//posを代入
	m_pos = pos;
}