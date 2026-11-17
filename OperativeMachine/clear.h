//=============================================================================
// 
// タイトル処理[clear.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _CLEAR_H_					//このマクロが定義されてなかったら
#define _CLEAR_H_					//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "scene.h"

//前方宣言---------------------------------------------------------------------
class CPlayer;			//プレイヤー
class CObjectX;			//Xオブジェクト
class CObject3D;		//3Dオブジェクト

//タイトルクラス---------------------------------------------------------------
class CClear : public CScene
{
public:
	CClear();
	~CClear();

	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CObjectX* GetObjectX(void) { return m_pObjectX; }
	static CObject3D** GetObject3D(void) { return &m_pObject3D[0]; }
private:
	static CPlayer* m_pPlayer;
	static CObjectX* m_pObjectX;
	static CObject3D* m_pObject3D[256];
};

#endif
