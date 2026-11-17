//=============================================================================
// 
// ポーズ処理[pauzse.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _PAUSE_H_ 
#define _PAUSE_H_ 

//インクルードファイル---------------------------------------------------------
#include "main.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_MENU	(3)

//ポーズクラス-----------------------------------------------------------------
class CPause
{
public:
	CPause();
	~CPause();

	//ポーズメニュー
	typedef enum
	{
		P_CONTINUE = 0,		//ゲームに戻る
		P_RETRY,			//ゲームをやり直す
		P_QUIT,				//タイトルに戻る
		P_MAX
	}PAUSE_MENU;

	//プロトタイプ宣言
	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	PAUSE_MENU GetPauseState(void){ return m_PauseMenu; }
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	PAUSE_MENU m_PauseMenu;
	int m_nIdxTexture[MAX_MENU];
	int m_nStickCounter;		//スティックの入力カウンター
};

#endif

