//=============================================================================
// 
// タイマー[timer.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _TIMRE_H_
#define _TIMRE_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define NUM_TIMER			(4)			//タイマーの桁数

//前方宣言---------------------------------------------------------------------
class CNumber;

//タイマークラス---------------------------------------------------------------
class CTimer:public CObject
{
public:
	CTimer(int nPriority = 5);
	~CTimer();

	static CTimer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col, int nInterval, int nNum, int nTimer);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	int GetTimer(void) { return m_nTimer; }
	void SetPosition(const D3DXVECTOR3 pos) {};
	D3DXVECTOR3 GetPosition(void) { return D3DXVECTOR3(0, 0, 0); }
	D3DXVECTOR3 GetSize(void) { return D3DXVECTOR3(0, 0, 0); }
private:
	CNumber* m_apNumber[NUM_TIMER];
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	int m_nInterval;		//間隔
	int m_nNumber;			//数字の数
	int m_nTimer;			//タイマー
	int m_nCounter;			//カウンター
};

#endif