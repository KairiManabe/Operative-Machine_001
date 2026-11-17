//===================================================================
// 
// パーティクル処理[particle.h]
// 
// Author : 眞鍋快陸
// 
//===================================================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

//インクルードファイル-----------------------------------------------
#include "main.h"
#include "object.h"
#include "effect.h"

//マクロ定義---------------------------------------------------------
#define MAX_PARTICLE	(5)

//エフェクトクラス---------------------------------------------------
class CParticle :public CObject
{
public:
	CParticle();
	~CParticle();

	typedef enum
	{
		TYPE_NONE = -1,
		TYPE_GATE,
		TYPE_JETT,
		TYPE_DETH_HUMAN1,
		TYPE_DETH_HUMAN2,
		TYPE_MAX
	}TYPE;

	static CParticle* Create(CEffect::EFFECT_TEX tex, TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col, int nLife, int nNumParticle);
	static CParticle* CreateModel(TYPE type, D3DXVECTOR3 pos, int nLife, int nNumParticle);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SpawnParticle(void);
	float RandFloat(void);

	void SetTex(const CEffect::EFFECT_TEX tex) { m_tex = tex; }
	void SetType(const TYPE type) { m_type = type; }
	void SetPosition(const D3DXVECTOR3 pos) {};
	D3DXVECTOR3 GetPosition(void) { return D3DXVECTOR3(0, 0, 0); }
	D3DXVECTOR3 GetSize(void) { return D3DXVECTOR3(0, 0, 0); }
private:
	CEffect* m_apEffect[MAX_PARTICLE];
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	D3DXVECTOR3 m_spawnPos;
	D3DXVECTOR3 m_move;
	int m_nLife;
	bool m_bUse;			//使用状態
	int m_nNumParticle;		//パーティクル量
	CEffect::EFFECT_TEX m_tex;
	TYPE m_type;			//タイプ
};

#endif