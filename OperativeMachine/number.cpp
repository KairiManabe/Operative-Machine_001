//=============================================================================
// 
// カウンタークラス[number.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "number.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "color.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CNumber::m_nIdxTexture = -1;

//=============================================================================
//コンストラクタ
CNumber::CNumber()
{
	//値のクリア
	m_pVtxBuff = NULL;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = COLOR_ORANGE;
	m_nInterval = 0;
	m_nIdx = 0;
	m_nNumber = 0;
	m_type = TYPE_NONE;
}

//=============================================================================
//デストラクタ
CNumber::~CNumber()
{
}

//=============================================================================
//テクスチャの生成
HRESULT CNumber::Load(void)
{
	//レンダラーを取得
	

	//テクスチャの読み込み
	m_nIdxTexture = CManager::GetTexture()->Register("data\\TEXTURE\\UI\\number100.png");
	return (m_nIdxTexture >= 0) ? S_OK : E_FAIL;
}

//=============================================================================
//テクスチャの破棄
void CNumber::Unload(void)
{
	m_nIdxTexture = -1;
}

//=============================================================================
//ナンバーの生成
CNumber* CNumber::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, int nInterval, int nIdx)
{
	//プレイヤー
	CNumber* pNumber = NULL;

	if (pNumber == NULL)
	{
		//弾生成
		pNumber = new CNumber;

		if (pNumber != NULL)
		{
			//インデックスを渡す
			pNumber->m_nIdx = nIdx;

			//サイズを代入
			pNumber->m_size = size;

			//間隔を代入
			pNumber->m_nInterval = nInterval;

			//pos
			pNumber->m_pos = pos;

			pNumber->Init();
		}
	}
	return pNumber;
}

//=============================================================================
//スコアの初期化処理
HRESULT CNumber::Init(void)
{
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

	VERTEX_2D* pVtx;

	//頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x + (m_nIdx * m_nInterval), m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_size.x + (m_nIdx * m_nInterval), m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x + (m_nIdx * m_nInterval), m_pos.y + m_size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_size.x + (m_nIdx * m_nInterval), m_pos.y + m_size.y, 0.0f);

	//rhw
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//頂点カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(0.09f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.09f, 1.0f);

	//アンロック
	m_pVtxBuff->Unlock();

	return S_OK;
}

//=============================================================================
//終了処理
void CNumber::Uninit(void)
{
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//=============================================================================
//更新処理
void CNumber::Update(void)
{
	VERTEX_2D* pVtx;

	//頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x + (-m_nIdx * m_nInterval), m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_size.x + (-m_nIdx * m_nInterval), m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x + (-m_nIdx * m_nInterval), m_pos.y + m_size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_size.x + (-m_nIdx * m_nInterval), m_pos.y + m_size.y, 0.0f);

	//頂点カラー
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//アンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
//描画処理
void CNumber::Draw(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定5
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddres(m_nIdxTexture));

	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
//数値のセット
void CNumber::SetNumber(int nNumber)
{
	VERTEX_2D* pVtx;

	//頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].tex = D3DXVECTOR2(0.0f + (0.09f * nNumber), 0.0f);
	pVtx[1].tex = D3DXVECTOR2(0.09f * (nNumber + 1), 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f + (0.09f * nNumber), 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.09f * (nNumber + 1), 1.0f);

	//アンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
//スコアの設定処理
void CNumber::AddCounter(int nCounter)
{
#if 0
	int aTexU[NUM_TIMER];		//各桁の数字を格納
	VERTEX_2D* pVtx;

	m_nCounter = nCounter;
	if (m_nCounter <= 0)
	{
		m_nCounter = 0;
	}
	aTexU[0] = m_nCounter % 1000 / 100;
	aTexU[1] = m_nCounter % 100 / 10;
	aTexU[2] = m_nCounter % 10 / 1;

	//頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nC = 0; nC < NUM_TIMER; nC++)
	{
		pVtx[0].tex = D3DXVECTOR2(0.0f + (0.1f * aTexU[nC]), 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f * (aTexU[nC] + 1), 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f + (0.1f * aTexU[nC]), 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f * (aTexU[nC] + 1), 1.0f);

		pVtx += 4;
	}

	//アンロック
	m_pVtxBuff->Unlock();
#endif
}
