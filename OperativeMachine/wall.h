//=============================================================================
//
// 3Dオブジェクト壁[wall.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _WALL_H_			//このマクロが定義されてなかったら
#define _WALL_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_F1			(30)				//ポリゴンサイズ
#define SIZE_F2			(400)				//ポリゴンサイズ

//オブジェクト2Dクラス---------------------------------------------------------
class CWall :public CObject
{
public:
	CWall(int nPriority = 3);
	~CWall();

	static CWall* Create(std::string fileName, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 pos);
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetColor(const D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetNor(const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	D3DXVECTOR3 GetPush(void) { return m_PushOut; }
	bool CollisionPlayer(void);
	D3DXVECTOR3 CalcPush(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	bool PointInTriangle(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	
	static int GetNumAll(void) { return m_nNumAll; }
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_size;		//サイズ	
	D3DXVECTOR3 m_MaxPos;	//最大座標
	D3DXVECTOR3 m_MinPos;	//最小座標
	D3DXVECTOR3 m_Center;	//中心座標
	D3DXCOLOR m_col;		//色
	D3DXMATRIX m_mtxWorld;	//ワールドマトリックス			
	D3DXVECTOR3 m_PushOut;	//押し出し座標
	static int m_nIdxTexture;
	static int m_nNumAll;
};

#endif