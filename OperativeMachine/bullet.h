//===================================================================
//
// 弾の出力処理[bullet.h]
// 
// Author : 眞鍋快陸
// 
//===================================================================
#ifndef _BULLET_H_			//このマクロが定義されてなかったら
#define _BULLET_H_			//2重インクルード防止のマクロを定義

//インクルードファイル-----------------------------------------------
#include "main.h"
#include "billboard.h"

//プレイヤークラス---------------------------------------------------
class CBullet :public CBillboard
{
public:
	CBullet(void);
	~CBullet();

	static HRESULT Load(void);
	static void Unload(void);
	static CBullet* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 move, const int nLife);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	bool CollisionHuman(const D3DXVECTOR3& start, const D3DXVECTOR3& end, float radius);
private:
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_move;
	D3DXVECTOR3 m_size;		//サイズ
	int m_nLife;			//体力

	static int m_nIdxTexture;
};

#endif
