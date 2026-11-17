//=============================================================================
// 
// スコア[score.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _SCORE_H_
#define _SCORE_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define NUM_SCORE			(8)			//スコアの桁数

//前方宣言---------------------------------------------------------------------
class CNumber;

//タイマークラス---------------------------------------------------------------
class CScore :public CObject
{
public:
	CScore(int nPriority = 5);
	~CScore();

	static CScore* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col,int nInterval, int nNum);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(const D3DXVECTOR3 size, int nInterval) { m_size = size; m_nInterval = nInterval; }
	void SetColor(D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return D3DXVECTOR3(0, 0, 0); }
	D3DXVECTOR3 GetSize(void) { return D3DXVECTOR3(0, 0, 0); }
	void AddScore(int nScore);
private:
	CNumber* m_apNumber[NUM_SCORE];
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	int m_nInterval;		//数字の数
	int m_nNumber;			//数字の数
	int m_nScore;			//カウンター
};

#endif