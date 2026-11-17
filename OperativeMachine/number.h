//=============================================================================
// 
// ナンバークラス[number.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _NUMBER_H_
#define _NUMBER_H_

#include "main.h"

//マクロ定義
#define NUMBER_MAX			(10)
#define SIZE_TIMER_W		(70)		//スコアの幅
#define SIZE_TIMER_H		(70)		//スコアの高さ
#define SIZE_SCORE_W		(55)		//スコアの高さ
#define SIZE_SCORE_H		(55)		//スコアの幅
#define MAX_TIMER			(2)			//テクスチャの最大数
#define SECOND				(60)		//一秒
#define CLEARTIME_SECOND	(3)		//クリアしたときのスコア集計

class CNumber
{
public:
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_TIMER,
		TYPE_SCORE,
		TYPE_MAX
	}TYPE;

	CNumber();
	~CNumber();

	static HRESULT Load(void);
	static void Unload(void);
	static CNumber* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, int nInterval, int nIdx);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(D3DXVECTOR3 size, int nInterval) { m_size = size; m_nInterval = nInterval; }
	void SetNumber(int nNumber);
	void AddCounter(int nCounter);
	void SetColor(D3DXCOLOR col) { m_col = col; }
private:
	static int m_nIdxTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	int m_nInterval;		//間隔
	TYPE m_type;
	int m_nIdx;
	int m_nNumber;
};

#endif