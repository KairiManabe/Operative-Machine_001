//=============================================================================
//
//マネージャー[manager.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _MANAGER_H_				//このマクロが定義されてなかったら
#define _MANAGER_H_				//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "scene.h"

//前方宣言---------------------------------------------------------------------
class CRenderer;		//レンダラー
class CFade;			//フェード
class CInputKeyboard;	//キーボード入力
class CInputMouse;		//マウス入力
class CInputJoypad;		//ジョイパッド入力
class CDebugproc;		//デバッグ表示
class CModelManager;

class CCamera;			//カメラ
class CLight;			//ライト
class CTexture;			//テクスチャ管理
class CSound;			//サウンド

//マネージャークラス-----------------------------------------------------------
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(int FPS);
	void Draw(void);

	static CScene* GetScene(void) { return m_pScene; }
	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static CFade* GetFade(void) { return m_pFade; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CInputMouse* GetInputMouse(void) { return m_pInputMouse; }
	static CInputJoypad* GetInputJoypad(void) { return m_pInputJoypad; }
	static CDebugproc* GetDebugproc(void) { return m_pDebug; }
	static CModelManager* GetModelManager(void) { return m_pModelManager; }

	static CCamera* GetCamera(void) { return m_pCamera; }
	static CLight* GetLight(void) { return m_pLight; }
	static CTexture* GetTexture(void) { return m_pTexture; }
	static CSound* GetSound(void) { return m_pSound; }
	
	static bool GetPause(void) { return m_bPause; }
	static bool GetFadePause(void) { return m_bFade; }

	static void SetMode(CScene::MODE mode);
	static void SetPause(bool bPause) { m_bPause = bPause; }
	static void SetFadePause(bool bFade) { m_bFade = bFade; }
private:
	static CScene* m_pScene;
	static CRenderer* m_pRenderer;
	static CFade* m_pFade;
	static CInputKeyboard* m_pInputKeyboard;
	static CInputMouse* m_pInputMouse;
	static CInputJoypad* m_pInputJoypad;
	static CDebugproc* m_pDebug;
	static CModelManager* m_pModelManager;

	static CCamera* m_pCamera;
	static CLight* m_pLight;
	static CTexture* m_pTexture;
	static CSound*m_pSound;
	
	static bool m_bPause;
	static bool m_bFade;
};

#endif
