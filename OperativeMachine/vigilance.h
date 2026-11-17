//=============================================================================
// 
// 警戒モード移行範囲[vigilance.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _VIGILANCE_H_
#define _VIGILANCE_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define VIGILANCE_PRIORITY	(3)		//自分のプライオリティを持っておく

//ボムクラス-------------------------------------------------------------------
class CVigilance :public CObject
{
public:
	CVigilance(int nPriority = VIGILANCE_PRIORITY);
	~CVigilance();

	static CVigilance* Create(D3DXVECTOR3 pos, float fMaxRadius, float fExpandSpeed);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void SetPosition(const D3DXVECTOR3 pos) override { m_pos = pos; }
	D3DXVECTOR3 GetPosition(void) override { return m_pos; }
	D3DXVECTOR3 GetSize(void) override { return D3DXVECTOR3(m_fCurrentRadius, m_fCurrentRadius, m_fCurrentRadius); }
	static bool GetVigilanceNow(void) { return m_bVigilanceEnd; }

	//現在の広がっている半径を取得できる
	float GetRadius(void) const { return m_fCurrentRadius; }

private:
	D3DXVECTOR3 m_pos;
	float m_fCurrentRadius; //現在の半径（0から徐々に広がる）
	float m_fMaxRadius;     //ターゲットがやられてから警戒し始めるターゲットの最大半径
	float m_fExpandSpeed;   //1フレームあたりの広がる速度（5.0f〜10.0f）

	static bool m_bVigilanceEnd;
};
#endif