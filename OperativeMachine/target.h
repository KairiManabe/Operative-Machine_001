//=============================================================================
//
// 的(ドローンorロボットの予定or電子機器→精密機器のような？)[target.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _TARGET_H_			//このマクロが定義されてなかったら
#define _TARGET_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_TARGET		(100)
#define TARGET_PRIORITY	(4)		//自分のプライオリティを持っておく

//前方宣言---------------------------------------------------------------------
class CRange;		//敵の索敵範囲

//オブジェクトXクラス----------------------------------------------------------
class CTarget :public CObject
{
public:
	CTarget(int nPriority = TARGET_PRIORITY);
	~CTarget();

	typedef enum
	{
		TYPE_NONE = -1,
		TYPE_NORMAL,
		TYPE_MAX,		
	}TYPE;

	static CTarget* Create(TYPE type, const std::string& fileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BreakBox(void);
	void BreakBombBox(void);
	void DethHuman(void);
	bool CollisionBullet(const D3DXVECTOR3& start, const D3DXVECTOR3& end, float radius) const;

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetColor(const D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	LPD3DXMESH GetMesh(void) const { return m_pMesh; }
	D3DXMATRIX GetWorldMatrix(void) const { return m_mtxWorld; }
	TYPE GetType(void) const { return m_type; }

	void DrawShadow(void);

	bool IsEnable(void) const { return m_bEnable; }
	void SetEnable(bool bEnable) { m_bEnable = bEnable; }

	bool CollisionPlayer(void);
	bool CollisionCamera(void);
	bool CollisionVigilanceRange(void);		//警戒態勢範囲との接触判定関数
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

	static int m_wallnum;
	int m_nFloating;			//浮遊上下切り替え用
	float m_fInit;				//Yの初期位置
	bool m_bCollisionOld;		//当たっていたか
	int m_nCollisionCounter;	//コリジョン用カウンター
	bool m_bEnable;				//有効フラグ
	int m_nIdxModel;
	int m_nWalkTimer;
};

#endif