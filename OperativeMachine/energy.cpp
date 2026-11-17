//=============================================================================
//
// エナジー(エネルギー残量表示処理)[energy.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "energy.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"

#include "tutorial.h"
#include "game.h"

#include "player.h"
#include "particle.h"
#include "effect.h"

#include "texture.h"
#include "textureUI.h"
#include "color.h"

//=============================================================================
//コンストラクタ
CEnergy::CEnergy(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_filename = {};
	 m_nIdxTexture = -1;
	m_col = COLOR_NONE;
}

//=============================================================================
//デストラクタ
CEnergy::~CEnergy()
{
}

//=============================================================================
//生成処理
CEnergy* CEnergy::Create(std::string pFileName, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col)
{
	//UI
	CEnergy* pEnergy = NULL;

	if (pEnergy == NULL)
	{
		//UI生成
		pEnergy = new CEnergy;

		if (pEnergy != NULL)
		{
			//各種代入
			pEnergy->m_pos = pos;
			pEnergy->m_size = size;
			pEnergy->m_col = col;
			pEnergy->m_filename = pFileName;

			//プレイヤー初期化
			pEnergy->Init(pEnergy->m_pos, pEnergy->m_size);
		}
	}
	return pEnergy;
}

//=============================================================================
//エナジーの初期化
HRESULT CEnergy::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
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

	CTextureUI::Create("data\\TEXTURE\\UI\\energy000.png", D3DXVECTOR3(19.0f, 3.0f, 0.0f), D3DXVECTOR3(322.0f, 40.0f, 0.0f), COLOR_CYAN);

	return S_OK;
}

//=============================================================================
//エナジーの終了
void CEnergy::Uninit(void)
{
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	CObject::Release();
}

//=============================================================================
//エナジーの描画
void CEnergy::Draw(void)
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
//エナジーの更新
void CEnergy::Update(void)
{
	//デバッグ表示情報の取得
	CDebugproc* pDebug = CManager::GetDebugproc();

	//シーンの取得
	CScene* pScene = CManager::GetScene();

	CPlayer* pPlayer = NULL;		//プレイヤー

	float fGauge = 0;

	fGauge = m_size.x / 3.2f;

	pDebug->PrintDebugProc("[残りエナジー:%.1f％]\n", fGauge);

	//プレイヤーの取得
	if (pScene->GetMode() == CScene::MODE_GAME && pPlayer == NULL)
	{//ゲームシーン
		pPlayer = CGame::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL && pPlayer == NULL)
	{//ゲームシーン
		pPlayer = CTutorial::GetPlayer();
	}

	if (pPlayer == NULL)
	{//プレイヤーを取得できていないとき
		return;
	}

	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	if (pPlayer->GetMode() == CPlayer::MODE_HUMAN && pPlayer->GetChangeNow() == false)
	{//人型の時は回復させる
		m_size.x += 0.2f;
	}
	else if (pPlayer->GetMode() == CPlayer::MODE_SPIDER && pPlayer->GetChangeNow() == false)
	{//クモ型の時は早く減る
		m_size.x -= 0.4f;
	}

	if (m_size.x <= 40.0f && m_col != COLOR_RED)
	{//エナジーの残量が40をきったとき赤にする
		m_col = COLOR_RED;
	}
	else if (m_size.x >= 40.0f && m_col == COLOR_RED)
	{//エナジーの残量が40をきったとき赤にする
		m_col = COLOR_NONE;
	}

	if (m_size.x <= 0.0f && pPlayer->GetMode() != CPlayer::MODE_HUMAN)
	{//エナジーが尽きた時人型未強制的に戻す
		CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(posPlayer.x, 50.0f, posPlayer.z), D3DXVECTOR3(5.0f, 5.0f, 0.0f), COLOR_RED, 40, 1);
		CParticle::Create(CEffect::TEX_NONE, CParticle::TYPE_GATE, D3DXVECTOR3(posPlayer.x, 50.0f, posPlayer.z), D3DXVECTOR3(3.0f, 3.0f, 0.0f), COLOR_BLUE, 40, 1);
		pPlayer->SetMode(CPlayer::MODE_HUMAN);
		m_size.x = 0.0f;
	}

	if (m_size.x >= MAX_ENERGY)
	{//320.0fを超えないようにする
		m_size.x = MAX_ENERGY;
	}

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

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}