//=============================================================================
// 
// タイトル処理[title.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _TITLE_H_					//このマクロが定義されてなかったら
#define _TITLE_H_					//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "scene.h"

//前方宣言---------------------------------------------------------------------
class CPlayer;			//プレイヤー
class CObjectX;			//Xオブジェクト
class CObject3D;		//3Dオブジェクト
class CTextureUI;		//UI

//タイトルクラス---------------------------------------------------------------
class CTitle : public CScene
{
public:
	CTitle();
	~CTitle();

	typedef enum
	{
		MENU_TUTORIAL = 0,
		MENU_GAME,
		MENU_MAX
	}TITLE_MENU;

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
	static CTextureUI* m_pTextureUI[MENU_MAX];

	D3DXVECTOR3 m_CametaRot;
	TITLE_MENU m_Menu;
	int m_nStickCounter;		//スティックの入力カウンター
};

#endif
