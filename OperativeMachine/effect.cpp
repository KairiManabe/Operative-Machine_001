//=============================================================================
// 
// エフェクトの描画[effect.cpp]
// 
// Author : 眞鍋快陸
//
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "effect.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "color.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CEffect::m_nIdxTexture[TEX_MAX] = {};
int CEffect::m_nNumAll = 0;

//=============================================================================
//コンストラクタ
CEffect::CEffect(int nPriority) : CBillboard(nPriority)
{
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = COLOR_ORANGE;
	m_nLife = 0;
	m_tex = TEX_NONE;
	m_nNumAll++;
}

//=============================================================================
//デストラクタ
CEffect::~CEffect()
{
}

//=============================================================================
//テクスチャの読み込み
HRESULT CEffect::Load(void)
{
	CTexture* pTexture = CManager::GetTexture();
	//m_nIdxTexture[TEX_NONE] = -1;
	m_nIdxTexture[TEX_CIRCLE] = pTexture->Register("data/TEXTURE/effect000.jpg");
	m_nIdxTexture[TEX_DIAMOND] = pTexture->Register("data/TEXTURE/effect000.jpg");

	return S_OK;
}

//=============================================================================
//テクスチャの破棄
void CEffect::Unload(void)
{//テクスチャクラスで管理してるからいらないかも
	
}

//=============================================================================
//エフェクトの生成
CEffect* CEffect::Create(EFFECT_TEX tex,D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXVECTOR3 size, D3DXCOLOR col, int nLife)
{
	CEffect* pEffect = new CEffect;

	if (pEffect != NULL)
	{
		pEffect->m_col = col;
		pEffect->m_move = move;
		pEffect->m_nLife = nLife;
		pEffect->SetTex(tex);

		//3Dオブジェクトとしての初期化（位置・サイズを渡す）
		pEffect->Init(pos, size);
	}
	return pEffect;
}

//=============================================================================
//初期化処理
HRESULT CEffect::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	//初期カラーの設定
	CBillboard::SetColor(m_col);
	CBillboard::Init(pos, size);
	CBillboard::BindTexture(CManager::GetTexture()->GetAddres(m_nIdxTexture[m_tex]));

	return S_OK;
}

//=============================================================================
//終了処理
void CEffect::Uninit(void)
{
	m_nNumAll--;

	//ビルボードクラスの終了
	CBillboard::Uninit();
}

//=============================================================================
//更新処理
void CEffect::Update(void)
{
	//現在の位置に移動量を加算
	D3DXVECTOR3 pos = GetPosition();
	pos += m_move;
	SetPosition(pos);

	//ライフを減らす
	m_nLife--;
	if (m_nLife <= 0)
	{
		Uninit();
		return;
	}

	//ビルボードクラスの更新
	CBillboard::Update();
}

//=============================================================================
//描画処理
void CEffect::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//エフェクト用にライティングを無効化する
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//Zテスト(前後関係の比較)は行うが、Zバッファへの書き込みはOFFにする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//テクスチャの設定(CTextureを利用)
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddres(m_nIdxTexture[m_tex]));

	//3Dオブジェクトの描画
	CBillboard::Draw();

	//Zバッファの書き込み設定を元に戻す(ONにする)
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//レンダーステートをもとに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}