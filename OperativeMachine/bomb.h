//=============================================================================
// 
// 爆弾管理[bomb.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _BOMB_H_
#define _BOMB_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//ボムクラス-------------------------------------------------------------------
class CBomb :public CObject
{
public:
	CBomb(int nPriority = 3);
	~CBomb();

	static CBomb* Create(D3DXVECTOR3 pos, float fMaxRadius, float fExpandSpeed);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void SetPosition(const D3DXVECTOR3 pos) override { m_pos = pos; }
	D3DXVECTOR3 GetPosition(void) override { return m_pos; }
	D3DXVECTOR3 GetSize(void) override { return D3DXVECTOR3(m_fCurrentRadius, m_fCurrentRadius, m_fCurrentRadius); }
	static bool GetBombNow(void) { return m_bBombEnd; }

	//現在の広がっている半径を取得できる
	float GetRadius(void) const { return m_fCurrentRadius; }

private:
	D3DXVECTOR3 m_pos;
	float m_fCurrentRadius; //現在の爆発半径（0から徐々に広がる）
	float m_fMaxRadius;     //爆発の最大半径
	float m_fExpandSpeed;   //1フレームあたりの広がる速度（例: 5.0f〜10.0f）

	static bool m_bBombEnd;
};
#endif