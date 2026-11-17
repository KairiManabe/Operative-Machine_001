//=============================================================================
// 
// シーン処理[scene.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _SCENE_H_					//このマクロが定義されてなかったら
#define _SCENE_H_					//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"

//シーンクラス-----------------------------------------------------------------
class CScene
{
public:
	typedef enum
	{
		MODE_TITLE = 0,
		MODE_TUTORIAL,
		MODE_GAME,
		MODE_CLEAR,
		MODE_FAIL,
		MODE_RESULT,
		MODE_MAX
	}MODE;

	CScene(MODE mode);
	~CScene();

	static CScene* Create(MODE mode);

	virtual void Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	MODE GetMode(void) { return m_mode; }
private:
	MODE m_mode;
};

#endif