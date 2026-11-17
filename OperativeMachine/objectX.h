//=============================================================================
//
// Xオブジェクト[objectX.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _OBJECTX_H_			//このマクロが定義されてなかったら
#define _OBJECTX_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_WALL	(1500)

//前方宣言---------------------------------------------------------------------
class CRange;		//敵の索敵範囲

//オブジェクトXクラス----------------------------------------------------------
class CObjectX :public CObject
{
public:
	CObjectX(int nPriority = 3);
	~CObjectX();

	typedef enum
	{
		TYPE_WALL = 0,
		TYPE_CEILING,	//天井
		TYPE_GOAL,		//仮
	}TYPE;

	static CObjectX* Create(TYPE type, const std::string& fileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 pos);
	void SetColor(D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetEnemyPos(void) { return m_enemyPos; }
	LPD3DXMESH GetMesh(void) const { return m_pMesh; }
	D3DXMATRIX GetWorldMatrix(void) const { return m_mtxWorld; }
	TYPE GetType(void) const { return m_type; }

	bool CollisionPlayer(void);
	bool CollisionEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	//bool CollisionGoal(void);
	bool CollisionCamera(void);

	static bool GetWallClim(void) { return m_bWallClim; }
private:
	int m_nIdxTexture[100];
	std::string m_pFileName;
	LPD3DXMESH m_pMesh;
	LPD3DXBUFFER m_pBuffMat;
	DWORD m_dwNumMat;
	D3DXMATRIX m_mtxWorld;
	D3DXVECTOR3 m_vtxMin, m_vtxMax;		//モデルの最小値、最大値
	D3DXVECTOR3 m_Center;	//オブジェクトの中心座標
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_move;		//位置
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	TYPE m_type;			//オブジェクトのタイプ
	D3DXVECTOR3 m_enemyPos;	//敵の押し出し用メモ

	static bool m_bWallClim;//壁登り状態
	static int m_wallnum;//壁登り状態
};

#endif