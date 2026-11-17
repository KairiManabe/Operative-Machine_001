//=============================================================================
// 
// チュートリアル処理[tutorial.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _TUTORIAL_H_					//このマクロが定義されてなかったら
#define _TUTORIAL_H_					//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "scene.h"
#include "objectX.h"

//マクロ定義-------------------------------------------------------------------
#define NUM_OBJ3D1			(20)	//なんちゃってメッシュの数(仮)
#define VER_OUTER_WALL		(26)		//縦の最大数
#define BESIDE_OUTER_WALL	(10)		//横の最大数

//前方宣言---------------------------------------------------------------------
class CPause;			//ポーズ
class CPlayer;			//プレイヤー
class CBullet;			//弾
class CScore;			//スコア
class CEffect;			//エフェクト
class CParticle;		//パーティクル
class CObject3D;		//3Dオブジェクト
class CMeshField;		//メッシュフィールド
class CEnergy;			//エナジー
class CObjectX;			//オブジェクトXの壁オブジェクト
class CTextureUI;		//テクスチャUI

//ゲームクラス-----------------------------------------------------------------
class CTutorial : public CScene
{
public:
	CTutorial();
	~CTutorial();

	typedef enum
	{
		UI_TUTORIAL = 0,
		UI_BOTTUN,
		UI_SPIDER,
		UI_HUMAN,
		UI_MAX
	}TUTORIAL_UI;

	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CPause* GetPause(void) { return m_pPause; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CBullet* GetBullet(void) { return m_pBullet; }
	static CScore* GetScore(void) { return m_pScore; }
	static CEffect* GetEffect(void) { return m_pEffect; }
	static CParticle* GeParticle(void) { return m_pParticle; }
	static CObject3D** GetObject3D(void) { return &m_pObject3D[0]; }
	static CMeshField* GetMeshField(void) { return m_pMeshField; }
	static CEnergy* GetEnergy(void) { return m_pEnergy; }
	static CObjectX** GetObjectXWall(void) { return &m_apObjxWall[0]; }

	static void SetUI(bool UI) { m_tutorialUI = UI; }
	static bool GetUI(void) { return m_tutorialUI; }
private:
	static CPause* m_pPause;
	static CPlayer* m_pPlayer;
	static CBullet* m_pBullet;
	static CScore* m_pScore;
	static CEffect* m_pEffect;
	static CParticle* m_pParticle;
	static CObject3D* m_pObject3D[256];
	static CMeshField* m_pMeshField;
	static CEnergy* m_pEnergy;
	static CObjectX* m_apObjxWall[MAX_WALL];
	static CTextureUI* m_pTextureUI[UI_MAX];

	static bool m_tutorialUI;
};

#endif
