//=============================================================================
// 
// モーション[motion.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _MOTION_H_ 
#define _MOTION_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"

//前方宣言---------------------------------------------------------------------
class CModel;

//マクロ定義-------------------------------------------------------------------
#define MAX_KEY				(16)	// キーの最大数
#define MAX_MODEL			(50)

//モーションタイプ-------------------------------------------------------------
typedef enum
{
	MOTIONTYPE_NONE = -1,
	MOTIONTYPE_NEUTRAL,
	MOTIONTYPE_MOVE,
	MOTIONTYPE_JUMP,
	MOTIONTYPE_ACTION,
	MOTIONTYPE_LAND,
	MOTIONTYPE_TRANSFORM,
	MOTIONTYPE_MAX
}MOTIONTYPE;

//モーションクラス-------------------------------------------------------------
class CMotion
{
public:
	CMotion();
	~CMotion();

	//キー要素の構造体
	typedef struct
	{
		float fPosX;		//位置X
		float fPosY;		//位置Y
		float fPosZ;		//位置Z
		float fRotX;		//向きX
		float fRotY;		//向きY
		float fRotZ;		//向きZ
	}KEY;

	//キー情報の構造体
	typedef struct
	{
		int nFrame;			//再生フレーム
		KEY aKey[128];		//各パーツのキー要素
	}KEY_INFO;

	//モーション情報の構造体
	typedef struct
	{
		bool bLoop;					//ループするかどうか
		int nNumKey;				//キーの総数
		KEY_INFO aKeyInfo[128];		//キー情報(キーの最大数)
	}INFO;

	void SetInfo(INFO info);
	void SetModel(CModel** ppModel, int nNumModel);
	void Update(void);
	int GetType(void) { return m_nType; }
	bool IsFinish(void);
	int GetKey(void) { return m_nKey; }
	float Normalize(float fAngle);
	void Set(int nType, int nBlendFrame = 10);
private:
	INFO m_aInfo[50];		//モーション情報
	int m_nNumAll;			//モーションの総数
	CModel** m_ppModel;		//モデルへのポインタ
	int m_nNumModel;		//モデルの総数
	int m_nType;
	bool m_bLoop;
	int m_nNumKey;
	int m_nKey;
	int m_nCounter;
	static bool m_bFinish;
	D3DXVECTOR3 m_offsetPos[40];
	D3DXVECTOR3 m_offsetRot[40];
	bool m_bBlending;							  //ブレンド中かどうかのフラグ
	int m_nBlendCounter;						  //ブレンド経過フレームカウンタ
	int m_nBlendFrame;							  //ブレンドにかける総フレーム数
	D3DXVECTOR3 m_aBlendStartPos[MAX_MODEL];      //ブレンド開始時の各モデル位置
	D3DXVECTOR3 m_aBlendStartRot[MAX_MODEL];      //ブレンド開始時の各モデル回転
};

#endif