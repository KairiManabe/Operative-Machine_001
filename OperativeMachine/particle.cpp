//=============================================================================
// 
// パーティクル処理[particle.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "particle.h"

#include "manager.h"
#include "renderer.h"
#include "color.h"
#include "debugproc.h"

#include "effectmodel.h"

//=============================================================================
//コンストラクタ
CParticle::CParticle()
{
	//値のクリア
	memset(&m_apEffect[0], 0, sizeof(m_apEffect));
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_nLife = 0;
	m_nNumParticle = 0;
	m_type = TYPE_NONE;
	m_bUse = false;
}

//=============================================================================
//デストラクタ
CParticle::~CParticle()
{
}

//=============================================================================
//パーティクルの生成
CParticle* CParticle::Create(CEffect::EFFECT_TEX tex, TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col, int nLife, int nNumParticle)
{
	//プレイヤー
	CParticle* pParticle = NULL;

	if (pParticle == NULL)
	{
		//パーティクル
		pParticle = new CParticle;

		if (pParticle != NULL)
		{
			//各種代入
			pParticle->m_size = size;
			pParticle->m_col = col;
			pParticle->m_pos = pos;
			pParticle->m_nLife = nLife;
			pParticle->m_nNumParticle = nNumParticle;

			//使用状態にする
			pParticle->m_bUse = true;
			
			//テクスチャタイプを設定
			pParticle->SetTex(tex);

			//パーティクルのタイプを設定
			pParticle->SetType(type);

			pParticle->Init(pParticle->m_pos, pParticle->m_size);
		}
	}
	return pParticle;
}

//=============================================================================
//モデルパーティクルの生成
CParticle* CParticle::CreateModel(TYPE type, D3DXVECTOR3 pos, int nLife, int nNumParticle)
{
	//プレイヤー
	CParticle* pParticle = NULL;

	if (pParticle == NULL)
	{
		//パーティクル
		pParticle = new CParticle;

		if (pParticle != NULL)
		{
			//各種代入
			pParticle->m_pos = pos;
			pParticle->m_nLife = nLife;
			pParticle->m_nNumParticle = nNumParticle;

			//使用状態にする
			pParticle->m_bUse = true;

			//パーティクルのタイプを設定
			pParticle->SetType(type);

			pParticle->Init(pParticle->m_pos, pParticle->m_size);
		}
	}
	return pParticle;
}

//======================================================================================
//パーティクルの初期化処理
HRESULT CParticle::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_pos = pos;
	m_size = size;

	return S_OK;
}

//======================================================================================
//パーティクルの終了処理
void CParticle::Uninit(void)
{
	for (int nCnt = 0; nCnt < m_nNumParticle; nCnt++)
	{
		if (m_apEffect[nCnt] != NULL)
		{//NULLチェック
			//終了処理
			m_apEffect[nCnt] = NULL;
		}
	}

	//破棄
	CObject::Release();
}

//======================================================================================
//パーティクルの更新処理
void CParticle::Update(void)
{
	D3DXVECTOR3 rot;	//向き

	float fRadius = m_size.x; //ゲートの半径
	float fHeight = m_size.y; //ゲートの高さオフセット

	switch (m_type)
	{
		//パーティクルのタイプがゲートだったら
	case TYPE_GATE:
		for (int nCnt = 0; nCnt < m_nNumParticle; nCnt++)
		{
			float fAngle = ((float)rand() / RAND_MAX) * D3DX_PI;

			//アーチ上の座標を計算
			D3DXVECTOR3 spawnPos;
			spawnPos.x = m_pos.x + cosf(fAngle) * fRadius;
			spawnPos.y = m_pos.y + sinf(fAngle) * fRadius + fHeight; //上半円
			spawnPos.z = m_pos.z;

			//厚みや揺らぎを出すための微小ランダムオフセット
			spawnPos.x += (float)(rand() % 10 - 5) * 1.0f;
			spawnPos.y += (float)(rand() % 10 - 5) * 1.0f;
			spawnPos.z += (float)(rand() % 20 - 10) * 1.0f;

			//パーティクルの移動方向
			D3DXVECTOR3 move;
			float fSpeed = (float)(rand() % 100) * 0.01f + 0.2f;	//低速で漂う

			move.x = cosf(fAngle) * fSpeed;
			move.y = sinf(fAngle) * fSpeed;
			move.z = ((float)(rand() % 100 - 50) * 0.01f);			//Z方向にすこしだけ

			//ひとつの粒子のサイズ（適度な大きさに調整）
			D3DXVECTOR3 particleSize = D3DXVECTOR3(20.0f, 20.0f, 0.0f);

			int nEffectLife = rand() % 15 + 15;
			CEffect::Create(m_tex, spawnPos, move, particleSize, m_col, nEffectLife);
		}
		break;

		//ジェット噴射処理
	case TYPE_JETT:
		for (int nCnt = 0; nCnt < m_nNumParticle; nCnt++)
		{
			//発生位置
			D3DXVECTOR3 spawnPos = m_pos;
			spawnPos.y += fHeight;

			//足元からの出現位置を広く散らす
			spawnPos.x += ((float)(rand() % 100 - 50) * 0.01f) * (fRadius * 2.0f);
			spawnPos.z += ((float)(rand() % 100 - 50) * 0.01f) * (fRadius * 2.0f);

			//下へ移動しながら外側へ大きく広がる
			D3DXVECTOR3 move;
			float fDownSpeed = -((float)(rand() % 100) * 0.04f + 1.5f); //下方向への速度

			//横方向の拡散速度
			float fSpreadPower = 2.5f;
			move.x = ((float)(rand() % 200 - 100) * 0.01f) * fSpreadPower;
			move.y = fDownSpeed;
			move.z = ((float)(rand() % 200 - 100) * 0.01f) * fSpreadPower;

			//粒子のサイズ
			D3DXVECTOR3 particleSize = D3DXVECTOR3(8.0f, 8.0f, 0.0f);

			int nEffectLife = rand() % 10 + 8;

			CEffect::Create(m_tex, spawnPos, move, particleSize, m_col, nEffectLife);
		}
		break;
		
		//人間死亡時
		case TYPE_DETH_HUMAN1:
			for (int nCnt = 0; nCnt < m_nNumParticle; nCnt++)
			{
				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle000.x", m_spawnPos, m_move, 500);

				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle001.x", m_spawnPos, m_move, 400);

				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle002.x", m_spawnPos, m_move, 400);

				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle003.x", m_spawnPos, m_move, 400);

				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle004.x", m_spawnPos, m_move, 400);
			}
			for (int nCnt = 0; nCnt < 100; nCnt++)
			{
				float fAngle = ((float)rand() / RAND_MAX) * D3DX_PI;

				//アーチ上の座標を計算
				D3DXVECTOR3 spawnPos, move;
				spawnPos.x = m_pos.x + sinf((float)(rand() % 100)) * 40;
				spawnPos.y = m_pos.y + cosf((float)(rand() % 100)) * 40;
				spawnPos.z = m_pos.z + cosf((float)(rand() % 100)) * 40;

				spawnPos.x += (float)(rand() % 20 - 10) * 1.0f;
				spawnPos.y += (float)(rand() % 30 - 10) * 1.0f;
				spawnPos.z += (float)(rand() % 20 - 10) * 1.0f;

				//位置の設定
				spawnPos.x = m_pos.x + (sinf((float)(rand() % 629 - 314 / 100))) * (float)(rand() % 45 + 1);
				spawnPos.y = m_pos.y + (cosf((float)(rand() % 629 - 314 / 100))) * (float)(rand() % 45 + 1);
				spawnPos.z = m_pos.z + (sinf((float)(rand() % 629 - 314 / 100))) * (float)(rand() % 45 + 1);

				move.x = sinf(spawnPos.x + D3DX_PI) * (float)(rand() % 2);
				move.y = sinf(spawnPos.y + D3DX_PI) * (float)(rand() % 2);
				move.z = cosf(spawnPos.z + D3DX_PI) * (float)(rand() % 2);

				CEffect::Create(CEffect::TEX_DIAMOND, spawnPos, move, D3DXVECTOR3(5.0f, 5.0f, 0.0f), COLOR_RED, 50);
			}
			break;
		case TYPE_DETH_HUMAN2:
			for (int nCnt = 0; nCnt < m_nNumParticle; nCnt++)
			{
				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle005.x", m_spawnPos, m_move, 500);

				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle006.x", m_spawnPos, m_move, 400);

				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle007.x", m_spawnPos, m_move, 400);

				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle008.x", m_spawnPos, m_move, 400);

				SpawnParticle();
				CEffectModel::Create(CEffectModel::EFFECT_DETH_HUMAN, "data/MODEL/effect/human_particle009.x", m_spawnPos, m_move, 400);
			}

			for (int nCnt = 0; nCnt < 100; nCnt++)
			{
				float fAngle = ((float)rand() / RAND_MAX) * D3DX_PI;

				//アーチ上の座標を計算
				D3DXVECTOR3 spawnPos, move;
				spawnPos.x = m_pos.x + sinf((float)(rand() % 100)) * 40;
				spawnPos.y = m_pos.y + cosf((float)(rand() % 100)) * 40;
				spawnPos.z = m_pos.z + cosf((float)(rand() % 100)) * 40;

				spawnPos.x += (float)(rand() % 20 - 10) * 1.0f;
				spawnPos.y += (float)(rand() % 30 - 10) * 1.0f;
				spawnPos.z += (float)(rand() % 20 - 10) * 1.0f;

				//位置の設定
				spawnPos.x = m_pos.x + (sinf((float)(rand() % 629 - 314 / 100))) * (float)(rand() % 45 + 1);
				spawnPos.y = m_pos.y + (cosf((float)(rand() % 629 - 314 / 100))) * (float)(rand() % 45 + 1);
				spawnPos.z = m_pos.z + (sinf((float)(rand() % 629 - 314 / 100))) * (float)(rand() % 45 + 1);

				move.x = sinf(spawnPos.x + D3DX_PI) * (float)(rand() % 2);
				move.y = sinf(spawnPos.y + D3DX_PI) * (float)(rand() % 2);
				move.z = cosf(spawnPos.z + D3DX_PI) * (float)(rand() % 2);

				CEffect::Create(CEffect::TEX_DIAMOND, spawnPos, move, D3DXVECTOR3(5.0f, 5.0f, 0.0f), COLOR_RED, 50);
			}
			break;

	}

	m_nLife--;
	if (m_nLife <= 0)
	{
		Uninit();
		return;
	}

}

//======================================================================================
// パーティクルの描画処理
void CParticle::Draw(void)
{	
}

//========================================================================
//スポーンの座標を求める
void CParticle::SpawnParticle(void)
{
	float fRadius = m_size.x; //半径
	float fHeight = m_size.y; //さオフセット
	D3DXVECTOR3 rot;	//向き

	float fAngle = ((float)rand() / RAND_MAX) * D3DX_PI;

	//アーチ上の座標を計算
	m_spawnPos = {};
	m_spawnPos.x = m_pos.x + sinf((float)(rand() % 100)) * 40;
	m_spawnPos.y = m_pos.y + sinf(fAngle) * fRadius + fHeight + 14.0f;
	m_spawnPos.z = m_pos.z + cosf((float)(rand() % 100)) * 40;

	m_spawnPos.x += (float)(rand() % 20 - 10) * 1.0f;
	m_spawnPos.y += (float)(rand() % 15 - 5) * 1.0f;
	m_spawnPos.z += (float)(rand() % 20 - 10) * 1.0f;

	//パーティクルの移動方向
	m_move = {};
	float fSpeed = (float)(rand() % 3 + 1);

	//移動量						　　　
	rot.z = ((float)(rand() % 629 - 314) / 20);

	m_move.x = sinf(rot.z) * fSpeed;
	m_move.z = cosf(rot.z) * fSpeed;
	m_move.y = (float)(rand() % 2 + 1) * 1.8f;
}

//========================================================================
//0.0～1.0の乱数
float CParticle::RandFloat(void)
{
	return (float)rand() / (float)RAND_MAX;
}