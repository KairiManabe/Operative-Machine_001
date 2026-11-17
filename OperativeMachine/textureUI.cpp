//=============================================================================
//
// UI[textureUI.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "textureUI.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "color.h"
#include "texture.h"

//静的メンバ変数宣言-----------------------------------------------------------

//=============================================================================
//コンストラクタ
CTextureUI::CTextureUI(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_filename = {};
	 m_nIdxTexture = -1;
	m_col = COLOR_NORMAL;
}

//=============================================================================
//デストラクタ
CTextureUI::~CTextureUI()
{
}

//=============================================================================
//生成処理
CTextureUI* CTextureUI::Create(std::string pFileName, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col)
{
	//UI
	CTextureUI* pTexUI = NULL;

	if (pTexUI == NULL)
	{
		//UI生成
		pTexUI = new CTextureUI;

		if (pTexUI != NULL)
		{
			//各種代入
			pTexUI->m_pos = pos;
			pTexUI->m_size = size;
			pTexUI->m_col = col;
			pTexUI->m_filename = pFileName;

			//プレイヤー初期化
			pTexUI->Init(pTexUI->m_pos, pTexUI->m_size);
		}
	}
	return pTexUI;
}

//=============================================================================
//敵の初期化
HRESULT CTextureUI::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

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
	pVtx[0].pos = D3DXVECTOR3(m_pos.x, m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_size.x, m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x, m_pos.y + m_size.y, 0.0f);
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
void CTextureUI::Uninit(void)
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
void CTextureUI::Draw(void)
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
void CTextureUI::Update(void)
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
}