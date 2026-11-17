//=============================================================================
// 
// フェード処理[fade.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _FADE_H_ 
#define _FADE_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "scene.h"

//シーンクラス-----------------------------------------------------------------
class CFade
{
public:
	CFade();
	~CFade();

	typedef enum
	{
		FADE_NONE = 0,
		FADE_IN,
		FADE_OUT,
		FADE_MAX
	}FADE;

	void Init(CScene::MODE modeNext);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void SetFade(CScene::MODE modeNext);
	FADE GetFadeNow(void) { return m_fade; }
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	FADE m_fade;					//フェードの状態
	static CScene::MODE m_modeNext;	//次の画面
	D3DXCOLOR m_colorFade;			//ポリゴンの色
};

#endif

