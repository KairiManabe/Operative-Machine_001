//=============================================================================
// 
// シーン処理[scene.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "scene.h"

#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "clear.h"
#include "fail.h"
#include "result.h"

//=============================================================================
//コンストラクタ
CScene::CScene(MODE mode)
{
	m_mode = mode;
}

//=============================================================================
//デストラクタ
CScene::~CScene()
{
}

//=============================================================================
//生成処理
CScene* CScene::Create(MODE mode)
{
	CScene* pScene = NULL;

	//指定されたモードを生成
	switch (mode)
	{
	case MODE_TITLE:
		//タイトル
		pScene = new CTitle;
		break;
	case MODE_TUTORIAL:
		//チュートリアル
		pScene = new CTutorial;
		break;
	case MODE_GAME:
		//ゲーム
		pScene = new CGame;
		break;
	case MODE_CLEAR:
		//クリア
		pScene = new CClear;
		break;
	case MODE_FAIL:
		//失敗		
		pScene = new CFail;
		break;
	case MODE_RESULT:
		//リザルト
		pScene = new CResult;
		break;
	}

	if (pScene != NULL)
	{//シーンの初期化
		pScene->Init();
	}

	return pScene;
}