//=============================================================================
//
// 敵の作的範囲など[range.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _RANGE_H_			//このマクロが定義されてなかったら
#define _RANGE_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"
//#include "enemy.h"
#include "target.h"

//マクロ定義-------------------------------------------------------------------


//エネミークラス-------------------------------------------------------------
class CRange :public CObject
{
public:
	CRange(int nPriority = 3);
	~CRange();

	static HRESULT Load(void);
	static void Unload(void);

	static CRange* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXVECTOR3 rot, const CTarget::TYPE type);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetSize(const D3DXVECTOR3 size);
	void SetType(const CTarget::TYPE type) { m_type = type; }

	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetPosOld(void) { return m_posOld; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) { return m_size; }

	bool CollisionFan(const D3DXVECTOR3& point, CTarget* pTarget) const;
	// 壁との当たり判定
	//bool CollisionWall(void);
private:
	static LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_posOld;
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	D3DXVECTOR3 m_rot;		//向き
	D3DXMATRIX m_mtxWorld;
	//CEnemy::TYPE m_Type;
	int m_nTurnCounter;		//方向転換のクールダウン(方向転換後にクールダウンがないと動かなくなる)	
	bool m_bTurn;			//壁に当たったときの方向転換状態
	int nBoom;

	CTarget::TYPE m_type;

	static int m_nIdxTexture[CTarget::TYPE_MAX];
	static int m_nNumAll;	//敵の総数
};

#endif
