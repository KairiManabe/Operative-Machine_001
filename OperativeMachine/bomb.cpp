//=============================================================================
// 
// 爆弾管理[bomb.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "bomb.h"
#include "game.h"
#include "objectX.h"
#include "particle.h"
#include "sound.h"

//静的メンバ変数宣言-----------------------------------------------------------
bool CBomb::m_bBombEnd = false;

//=============================================================================
//コンストラクタ
CBomb::CBomb(int nPriority) : CObject(nPriority)
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fCurrentRadius = 0.0f;
	m_fMaxRadius = 0.0f;
	m_fExpandSpeed = 0.0f; // 速度を調整することで連鎖破棄のテンポが変わる
}

//=============================================================================
//デストラクタ
CBomb::~CBomb() 
{
}

//=============================================================================
//生成
CBomb* CBomb::Create(D3DXVECTOR3 pos, float fMaxRadius, float fExpandSpeed)
{
	CBomb* pBomb = new CBomb();
	if (pBomb != NULL)
	{
		pBomb->m_fMaxRadius = fMaxRadius;
		pBomb->m_fExpandSpeed = fExpandSpeed;
		pBomb->Init(pos, D3DXVECTOR3(fMaxRadius, fMaxRadius, fMaxRadius));
	}
	return pBomb;
}

//=============================================================================
//初期化
HRESULT CBomb::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
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
void CBomb::Uninit(void)
{
	Release(); //CObjectの死亡フラグを立てて自動破棄
}

//=============================================================================
//更新
void CBomb::Update(void)
{
	//毎フレーム爆発範囲を拡大させる
	m_fCurrentRadius += m_fExpandSpeed;
	m_bBombEnd = true;

	// 最大半径に達したら爆風オブジェクトを削除
	if (m_fCurrentRadius >= m_fMaxRadius)
	{
		m_bBombEnd = false;
		Uninit();
	}
}

//=============================================================================
//描画
void CBomb::Draw(void)
{
}