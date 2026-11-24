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
#include "model.h"
#include "motion.h"
#include "load.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_TARGET		(100)
#define TARGET_PRIORITY	(4)		//自分のプライオリティを持っておく

//前方宣言---------------------------------------------------------------------
class CRange;		//敵の索敵範囲

//オブジェクトXクラス----------------------------------------------------------
class CTarget :public CObject
{
public:
	struct MODEL_INFO
	{
		std::string FileName;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		int nIdxParent;
		int nIdxModel;
	};
	CTarget(int nPriority = TARGET_PRIORITY);
	~CTarget();

	typedef enum
	{
		TYPE_NONE = -1,
		TYPE_RANGE,		//円形
		TYPE_FAN,		//扇形
		TYPE_MAX,		
	}TYPE;

	static CTarget* Create(TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BreakBox(void);
	void BreakBombBox(void);
	void DethTarget(void);
	bool CollisionBullet(const D3DXVECTOR3& start, const D3DXVECTOR3& end, float radius) const;

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
void SetCollisionBounds(const D3DXVECTOR3& minBounds, const D3DXVECTOR3& maxBounds) { m_vtxMin = minBounds; m_vtxMax = maxBounds; }
	void SetColor(const D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	LPD3DXMESH GetMesh(void) const { return m_pMesh; }
	D3DXMATRIX GetWorldMatrix(void) const { return m_mtxWorld; }
	TYPE GetType(void) const { return m_type; }
	CRange* GetRange(void) { return m_pRange; }

	void DrawShadow(void);

	bool IsEnable(void) const { return m_bEnable; }
	void SetEnable(bool bEnable) { m_bEnable = bEnable; }

	bool CollisionPlayer(void);
	bool CollisionCamera(void);
	bool CollisionVigilanceRange(void);

	HRESULT LoadParentModel(const std::string& fileName);
	void LoadParentFile(const std::string& fileName, int nIdxFile, int nMaxModel);
	void LoadOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxParent, int nIdxModel);
	HRESULT LoadMotionInfo(const std::string& fileName);
	void LoadMotion(bool bLoop, int nNumKey, CMotion::KEY_INFO* pKeyInfo, int nMotion);
	void LoadEnableString(char* aStrCpy, char* pStart);		//警戒態勢範囲との接触判定関数
private:
	int m_nIdxTexture[100];
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
	CModel* m_apModel[30];
	CMotion* m_pMotion;
	int m_nNumModel;
	int m_nWalkTimer;
	static MODEL_INFO m_aModelInfo[30];
	static CMotion::INFO m_aMotionInfo[MOTIONTYPE_MAX];

	CRange* m_pRange;
};

#endif