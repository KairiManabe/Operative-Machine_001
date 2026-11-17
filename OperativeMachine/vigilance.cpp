//=============================================================================
// 
// 警戒モード移行範囲[vigilance.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "vigilance.h"

#include "sound.h"

#include "game.h"

#include "objectX.h"
#include "particle.h"

//静的メンバ変数宣言-----------------------------------------------------------
bool CVigilance::m_bVigilanceEnd = false;

//=============================================================================
//コンストラクタ
CVigilance::CVigilance(int nPriority) : CObject(nPriority)
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fCurrentRadius = 0.0f;
	m_fMaxRadius = 0.0f;
	m_fExpandSpeed = 0.0f; //速度を調整することで連鎖破棄のテンポが変わる
}

//=============================================================================
//デストラクタ
CVigilance::~CVigilance() 
{
}

//=============================================================================
//生成
CVigilance* CVigilance::Create(D3DXVECTOR3 pos, float fMaxRadius, float fExpandSpeed)
{
	CVigilance* pVigilance = new CVigilance();
	if (pVigilance != NULL)
	{
		pVigilance->m_fMaxRadius = fMaxRadius;
		pVigilance->m_fExpandSpeed = fExpandSpeed;
		pVigilance->Init(pos, D3DXVECTOR3(fMaxRadius, fMaxRadius, fMaxRadius));
	}
	return pVigilance;
}

//=============================================================================
//初期化
HRESULT CVigilance::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_pos = pos;

	//爆発エフェクト・SE再生
	//CParticle::CreateModel(CParticle::TYPE_BOMBER, m_pos, 25, 3);
	//CSound::PlaySound_(CSound::SOUND_LABEL_SE_BOMB);

	//種類の設定
	SetType(TYPE_TARGET);

	return S_OK;
}

//=============================================================================
//終了
void CVigilance::Uninit(void)
{
	Release(); //CObjectの死亡フラグを立てて自動破棄
}

//=============================================================================
//更新
void CVigilance::Update(void)
{
	//毎フレーム範囲を拡大させる
	m_fCurrentRadius += m_fExpandSpeed;
	m_bVigilanceEnd = true;

	// 最大半径に達したらオブジェクトを削除
	if (m_fCurrentRadius >= m_fMaxRadius)
	{
		m_bVigilanceEnd = false;
		Uninit();
	}
}

//=============================================================================
//描画
void CVigilance::Draw(void)
{
}