//=============================================================================
// 
// ゲーム処理[game.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _GAME_H_					//このマクロが定義されてなかったら
#define _GAME_H_					//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "scene.h"
#include "objectX.h"

//マクロ定義-------------------------------------------------------------------
#define NUM_OBJ3D			(70)
#define VER_OUTER_WALL		(26)		//縦の最大数
#define BESIDE_OUTER_WALL	(10)		//横の最大数

//前方宣言---------------------------------------------------------------------
class CPause;			//ポーズ
class CPlayer;			//プレイヤー
class CBullet;			//弾
class CScore;			//スコア
class CTimer;			//タイマー
class CEffect;			//エフェクト
class CParticle;		//パーティクル
class CObject3D;		//3Dオブジェクト
class CMeshField;		//メッシュフィールド
class CEnergy;			//エナジー
class CObjectX;			//オブジェクトXの壁オブジェクト
class CWall;
class CField;
class CTarget;			//ターゲット
class CCrosshair;		//クロスヘア

//ゲームクラス-----------------------------------------------------------------
class CGame : public CScene
{
public:
	CGame();
	~CGame();

	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CPause* GetPause(void) { return m_pPause; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CBullet* GetBullet(void) { return m_pBullet; }
	static CScore* GetScore(void) { return m_pScore; }
	static CTimer* GetTimer(void) { return m_pTimer; }
	static CEffect* GetEffect(void) { return m_pEffect; }
	static CParticle* GeParticle(void) { return m_pParticle; }
	static CObject3D** GetObject3D(void) { return &m_pObject3D[0]; }
	static CMeshField* GetMeshField(void) { return m_pMeshField; }
	static CEnergy* GetEnergy(void) { return m_pEnergy; }
	static CObjectX** GetObjectXWall(void) { return &m_apObjxWall[0]; }
	static CField* GetField(void) { return m_pField; }
	static CWall** GetWall(void) { return &m_pWall[0]; }
	static CTarget** GetTarget(void) { return &m_pTarget[0]; }
	static CCrosshair* GetCrosshair(void) { return m_pCrosshair; }

	static void SubTarget(CTarget* pTarget);
	static int GetNumTarget(void) { return m_nCntTraget; }

	HRESULT LoadObject(const std::string& fileName);
	void LoadEnableString(char* aStrCpy, char* pStart);
	void LoadPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void LoadField(const char* texPath, const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXVECTOR3 rot);
	void LoadWall(const char* texPath, const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXVECTOR3 rot);
	void LoadTarget(const char* ModelPath, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
	void LoadObjectX(const char* modelType, const char* objPath, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
private:
	static CPause* m_pPause;
	static CPlayer* m_pPlayer;
	static CBullet* m_pBullet;
	static CScore* m_pScore;
	static CTimer* m_pTimer;
	static CEffect* m_pEffect;
	static CParticle* m_pParticle;
	static CObject3D* m_pObject3D[256];
	static CMeshField* m_pMeshField;
	static CEnergy* m_pEnergy;
	static CObjectX* m_apObjxWall[MAX_WALL];
	static CField* m_pField;
	static CWall* m_pWall[10];
	static CTarget* m_pTarget[30];
	static CCrosshair* m_pCrosshair;

	int m_nCntWall;
	static int m_nCntTraget;
};

#endif
