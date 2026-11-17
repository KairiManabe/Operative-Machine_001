//=============================================================================
//
// 3Dプレイヤー(階層構造)[player.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _PLAYER_H_			//このマクロが定義されてなかったら
#define _PLAYER_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"
#include "model.h"
#include "motion.h"
#include "load.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_F1			(30)				//ポリゴンサイズ
#define SIZE_F2			(400)				//ポリゴンサイズ
#define PLAYER_POSY		(0.0f)
#define PLAYER_HEAD		(100.0f)
#define MAX_PLAYER_MODEL	(40)			//モデル(パーツ)の最大数

#define PLAYER_WALK		(0.35f)				//プレイヤーの速度調整
#define PLAYER_ACTION	(0.1f)				//プレイヤーの仮アクション速度(前に転ぶから)

#define PLAYER_MOVEROT	(-0.3f)				//プレイヤーが進んだ時の傾き

#define PLAYER_PRIORITY	(2)		//自分のプライオリティを持っておく

//プレイヤークラス-------------------------------------------------------------
class CPlayer:public CObject
{
public:
	CPlayer(int nPriority = PLAYER_PRIORITY);
	~CPlayer();

	typedef enum
	{
		MODE_HUMAN = 0,		//人型
		MODE_SPIDER,		//クモ型
		MODE_CANNON			//銃型
	}MODE;

	struct MODEL_INFO
	{
		std::string FileName;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		int nIdxParent;
		int nIdxModel;
	};

	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void DrawShadow(void);

	HRESULT LoadParentModel(const std::string& fileName);
	void LoadParentFile(const std::string& fileName, int nIdxFile, int nMaxModel);
	void LoadOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxParent, int nIdxModel);
	HRESULT LoadMotionInfo(const std::string& fileName);
	void LoadMotion(bool bLoop, int nNumKey, CMotion::KEY_INFO* pKeyInfo, int nMotion);
	void LoadEnableString(char* aStrCpy, char* pStart);

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetMode(const MODE mode);

	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetPosOld(void) { return m_posOld; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	float GetCollsionSize(void) { return m_fCollisionSize; }
	static MODE GetMode(void) { return m_Mode; }
	static bool GetChangeNow(void) { return m_bChangeMode; }

	void SetClimbingWall(bool bClimb) { m_bClimbingWall = bClimb; }
	bool IsClimbingWall(void) const { return m_bClimbingWall; }

	void ClimbUp(float speed);
	void RotateTarget(float& currentRotY, float targetRotY, float speed);
private:
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_posOld;
	D3DXVECTOR3 m_move;
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	D3DXVECTOR3 m_rot;		//色
	D3DXMATRIX m_mtxWorld;

	CModel *m_apModel[50];
	bool m_bLand;			//着地状態
	bool m_bAction;			//アクション状態
	CMotion* m_pMotion;		
	int m_nNumModel;		//モデルの総数
	float m_fCollisionSize;	//当たり判定時の半径の大きさ
	float m_fModelChange;	//モデルを変型(交換するときの演出時間)
	int m_nIdxModel;		//モデルインデックスは配列である必要はない

	static MODEL_INFO m_aModelInfo[50];						//モデルインフォー
	static CMotion::INFO m_aMotionInfo[MOTIONTYPE_MAX];		//モーションインフォ－
	static MODE m_Mode;				//プレイヤーのモード
	static bool m_bChangeMode;		//モード変形中

	bool m_bClimbingWall;   // 壁登り中フラグ

	float m_targetRotY;		// 移動目標のY軸回転角度
};

#endif