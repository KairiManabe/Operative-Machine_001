//=============================================================================
//
// 敵(階層構造)[enemy.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _ENEMY_H_			//このマクロが定義されてなかったら
#define _ENEMY_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"
#include "model.h"
#include "motion.h"
#include "load.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_F1			(30)			//ポリゴンサイズ
#define SIZE_F2			(400)			//ポリゴンサイズ
#define PLAYER_POSY		(0.0f)
#define PLAYER_HEAD		(100.0f)
#define MAX_ENEMY_MODEL	(15)			//モデル(パーツ)の最大数

#define PLAYER_WALK		(0.35f)			//プレイヤーの速度調整
#define PLAYER_ACTION	(0.1f)			//プレイヤーの仮アクション速度(前に転ぶから)

#define PLAYER_MOVEROT	(-0.3f)			//プレイヤーが進んだ時の傾き

//前方宣言---------------------------------------------------------------------
class CRange;		//索敵範囲

//エネミークラス-------------------------------------------------------------
class CEnemy:public CObject
{
public:
	CEnemy(int nPriority = 3);
	~CEnemy();

	struct MODEL_INFO
	{
		std::string FileName;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		int nIdxParent;
		int nIdxModel;
	};

	typedef enum
	{
		TYPE_NONE = -1,
		TYPE_STRAIGHT,
		TYPE_AROUND_RIGHT,
		TYPE_AROUND_LEFT,
		TYPE_MAX
	}TYPE;

	static CEnemy* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot, TYPE type);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	HRESULT LoadParentModel(const std::string& fileName);
	void LoadParentFile(const std::string& fileName, int nIdxFile, int nMaxModel);
	void LoadOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxParent, int nIdxModel);
	HRESULT LoadMotionInfo(const std::string& fileName);
	void LoadMotion(bool bLoop, int nNumKey, CMotion::KEY_INFO* pKeyInfo, int nMotion);
	void LoadEnableString(char* aStrCpy, char* pStart);

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetType(const TYPE type) { m_type = type; }
	
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetPosOld(void) { return m_posOld; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	float GetCollsionSize(void) { return m_fCollisionSize; }

	CRange* GetRange(void) { return m_pRange; }
private:
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_posOld;
	D3DXVECTOR3 m_move;
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	D3DXVECTOR3 m_rot;		//色
	D3DXMATRIX m_mtxWorld;

	CModel *m_apModel[30];
	bool m_bLand;			//着地状態
	bool m_bAction;			//アクション状態
	CMotion* m_pMotion;		
	int m_nNumModel;		//モデルの総数
	float m_fCollisionSize;	//当たり判定時の半径の大きさ
	float m_fModelSize;		//モデルのサイズ
	CRange* m_pRange;		//足元の索敵範囲用のサークルへのポインタ
	TYPE m_type;			//敵の行動パターンタイプ
	
	static MODEL_INFO m_aModelInfo[30];						//モデルインフォー
	static CMotion::INFO m_aMotionInfo[MOTIONTYPE_MAX];		//モーションインフォ－
	static int m_nNumAll;		//敵の総数
};

#endif