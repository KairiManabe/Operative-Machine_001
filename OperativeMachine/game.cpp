//=============================================================================
// 
// ゲーム処理[game.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "game.h"

#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "color.h"
#include "fade.h"
#include "pause.h"
#include "sound.h"
#include "load.h"

#include "camera.h"
#include "number.h"
#include "timer.h"
#include "score.h"

#include "player.h"
#include "enemy.h"
#include "textureUI.h"
#include "energy.h"
#include "particle.h"
#include "effect.h"
#include "object2D.h"
#include "object3D.h"
#include "objectX.h"
#include "meshfield.h"
#include "billboard.h"
#include "range.h"
#include "particle.h"
#include "objectX.h"
#include "field.h"
#include "wall.h"
#include "target.h"
#include "crosshair.h"
#include "bullet.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_MINI		(D3DXVECTOR3(100.0f, 0.0f, 100.0f))
#define SIZE_NORMAL		(D3DXVECTOR3(1000.0f, 0.0f, 1000.0f))
#define MODEL_WALL		"data/MODEL/object/wall002.x"
#define MODEL_CEILING	"data/MODEL/object/ceiling000.x"

//静的メンバ変数宣言-----------------------------------------------------------
CPause* CGame::m_pPause = NULL;
CPlayer* CGame::m_pPlayer = NULL;
CBullet* CGame::m_pBullet = NULL;
CScore* CGame::m_pScore = NULL;
CTimer* CGame::m_pTimer = NULL;
CEffect* CGame::m_pEffect = NULL;
CParticle* CGame::m_pParticle = NULL;
CObject3D* CGame::m_pObject3D[256] = {};
CMeshField* CGame::m_pMeshField = NULL;
CEnergy* CGame::m_pEnergy = NULL;
CObjectX* CGame::m_apObjxWall[MAX_WALL] = {};
CWall* CGame::m_pWall[10] = {};
CField* CGame::m_pField = NULL;
CTarget* CGame::m_pTarget[30] = {};
CCrosshair* CGame::m_pCrosshair = NULL;

int CGame::m_nCntTraget = 0;

//=============================================================================
//コンストラクタ
CGame::CGame() :CScene(CScene::MODE_GAME)
{
	m_nCntWall = 0;
}

//=============================================================================
//デストラクタ
CGame::~CGame()
{
}

//=============================================================================
//初期化
void CGame::Init(void)
{ 
	//カメラ情報の初期化
	CManager::GetCamera()->SetAll(D3DXVECTOR3(0.0f, CAMERA_ANGLE_PY, -700.0f - CAMERA_ANGLE_PZ),
		D3DXVECTOR3(0.0f, 30.0f, -950.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//各テクスチャの生成
	CObject3D::Load();
	CEffect::Load();
	CBullet::Load();
	CNumber::Load();
	CRange::Load();

	if (m_pPause == NULL)
	{
		m_pPause = new CPause;
		if (m_pPause != NULL)
		{
			m_pPause->Init();
		}
	}

	//BGM再生
	CSound::PlaySound(CSound::SOUND_LABEL_GAMEBGM);

	//UI・タイマー（3分 = 180秒）
	m_pEnergy = CEnergy::Create("data\\TEXTURE\\UI\\energy001.png", D3DXVECTOR3(20.0f, 3.0f, 0.0f), D3DXVECTOR3(MAX_ENERGY, 40.0f, 0.0f), COLOR_NORMAL);
	CTextureUI::Create("data\\TEXTURE\\UI\\timer001.png", D3DXVECTOR3(1020.0f, 0.0f, 0.0f), D3DXVECTOR3(120.0f, 60.0f, 0.0f), COLOR_RED);
	m_pTimer = CTimer::Create(D3DXVECTOR3(1240.0f, 0.0f, 0.0f), D3DXVECTOR3(30.0f, 60.0f, 0.0f), COLOR_NORMAL, 30, 4, 180);

	//クロスヘアUIの生成
	m_pCrosshair = CCrosshair::Create(CENTER_2DPOS, D3DXVECTOR3(20.0f, 20.0f, 0.0f), COLOR_NONE);

	LoadObject("data/SCRIPT/object000.txt");
}

//=============================================================================
//終了
void CGame::Uninit(void)
{
	if (m_pPause != NULL)
	{//終了、NULLにする
		m_pPause->Uninit();
		delete m_pPause;
		m_pPause = NULL;
	}

	//配列数を取得
	int nWallNum = sizeof(m_apObjxWall) / sizeof(m_apObjxWall[0]);

	for (int nCntWall = 0; nCntWall < nWallNum; nCntWall++)
	{//NULLにする
		m_apObjxWall[nCntWall] = NULL;
	}

	//BGMを停止
	CSound::StopSound(CSound::SOUND_LABEL_GAMEBGM);

	//すべて破棄
	CObject::ReleaseAll();

	//各テクスチャの破棄
	CNumber::Unload();
	CBullet::Unload();
	CEffect::Unload();
	CObject3D::Unload();
}

//=============================================================================
//更新
void CGame::Update(void)
{
	//キー入力
	CInputKeyboard* pInputKey = CManager::GetInputKeyboard();

	//フェードを取得
	CFade* pFade = CManager::GetFade();

	if (m_pPause != NULL)
	{//ポーズのアップデート
		m_pPause->Update();
	}
}

//=============================================================================
//描画
void CGame::Draw(void)
{
}

//=============================================================================
//CGame 内の人間減算・配列整理処理
void CGame::SubTarget(CTarget* pTarget)
{
	if (pTarget == NULL) return;

	for (int nCntIdx = 0; nCntIdx < m_nCntTraget; nCntIdx++)
	{
		if (m_pTarget[nCntIdx] == pTarget)
		{
			// 削除対象の場所に配列の「最後尾の要素」を上書きして詰める
			m_pTarget[nCntIdx] = m_pTarget[m_nCntTraget - 1];
			m_pTarget[m_nCntTraget - 1] = NULL; // 旧末尾をNULL化
			m_nCntTraget--;                          // 要素数を減らす
			break;
		}
	}
}

//=============================================================================
//オブジェクトファイルの読み込み
HRESULT CGame::LoadObject(const std::string& fileName)
{
	FILE* pFile = fopen(fileName.c_str(), "r");

	if (pFile == NULL)
	{//読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};		    //文字列読み込み
	static char aStrCpy[MAX_STRING] = {};	    //文字列複製(整理)
	static char* pStart = NULL;				    //文字列開始位置
	static char aObjPath[FILE_MAX] = {};		//モデルのファイル名読み込み
	static char aTexPath[FILE_MAX] = {};		//テクスチャファイル名読み込み
	static char aModelType[FILE_MAX] = {};		//モデルタイプ名読み込み
	int nIdx = 0;								//モデルのインデックス読み込み
	int nParent = 0;							//モデルの親インデックス読み込み

	enum
	{//ローカルタイプ
		OBJECT_TYPE_PLAYER = 0,
		OBJECT_TYPE_FIELD,
		OBJECT_TYPE_WALL,
		OBJECT_TYPE_TARGET,
		OBJECT_TYPE_MODEL
	};

	int type = -1;
	D3DXVECTOR3 pos = { 0.0f,0.0f,0.0f };		//位置読み込み
	D3DXVECTOR3 size = { 0.0f,0.0f,0.0f };		//サイズ読み込み
	D3DXVECTOR3 rot = { 0.0f,0.0f,0.0f };		//向き読み込み
	int nCntFileName = 0;						//ファイル名カウント
	int nCntModel = 0;							//モデル数
	int nLoadedObjects = 0;

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));			//文字列クリア
		(void)fgets(aStr, sizeof(aStr), pFile);	//一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{//LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pFile) != NULL)
		{//読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));				//文字列クリア
		memset(aStrCpy, 0, sizeof(aStrCpy));		//コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pFile);		//一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);	//有効文字だけ抜き取って複製

		if (strstr(aStr, LOAD_NUMMODEL))
		{//NUM_MODELを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%d", &nCntModel);

			break;
		}
	}

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));				//文字列クリア
		memset(aStrCpy, 0, sizeof(aStrCpy));		//コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pFile);		//一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);	//有効文字だけ抜き取って複製

		if (strstr(aStr, LOAD_OBJECT))
		{//onjectsetを読み込んだ
			while (true)
			{
				if (nLoadedObjects >= nCntModel)
				{//最大数より多くなったら
					break;
				}

				memset(aStr, 0, sizeof(aStr));				//文字列クリア
				memset(aStrCpy, 0, sizeof(aStrCpy));		//コピーもクリア
				(void)fgets(aStr, sizeof(aStr), pFile);		//一列読み込み
				LoadEnableString(&aStrCpy[0], &aStr[0]);	//有効文字だけ抜き取って複製

				if (strstr(aStr, LOAD_TYPE))
				{//TYPEを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &type);
					continue;
				}

				if (strstr(aStr, LOAD_MODELKIND))
				{//モデルのタイプ名を読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%s", &aModelType);
				}

				if (strstr(aStr, LOAD_OBJECTFILE))
				{//MODEL_FILENAMEを読み込んだ

					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%s", &aObjPath);
				}

				if (strstr(aStr, LOAD_TEXTUREFILE))
				{//MODEL_FILENAMEを読み込んだ

					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%s", &aTexPath);
				}

				if (strstr(aStr, LOAD_POS))
				{//POSを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);
					continue;
				}

				if (strstr(aStr, LOAD_SIZE))
				{//SIZEを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%f %f %f", &size.x, &size.y, &size.z);
					continue;
				}

				if (strstr(aStr, LOAD_ROT))
				{//ROTを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%f %f %f", &rot.x, &rot.y, &rot.z);

					continue;
				}

				if (strcmp(aStrCpy, LOAD_OBJECTEND) == 0)
				{// END_PARTSSETを読み込んだ
					switch (type)
					{
					case OBJECT_TYPE_PLAYER:
						LoadPlayer(pos, size);
						break;
					case OBJECT_TYPE_FIELD:
						LoadField(aTexPath, pos, size, rot);
						break;
					case OBJECT_TYPE_WALL:
						LoadWall(aTexPath, pos, size, rot);
						break;
					case OBJECT_TYPE_TARGET:
						LoadTarget(aObjPath, pos, rot);
						break;
					}

					nLoadedObjects++;

					//値をクリア
					memset(&pos, NULL, sizeof(D3DXVECTOR3));
					memset(&size, NULL, sizeof(D3DXVECTOR3));
					memset(&rot, NULL, sizeof(D3DXVECTOR3));
					nIdx = 0;
					nParent = 0;
					type = -1;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{//END_SCRIPTを読み込んだ
			//読み込み終了
			fclose(pFile);
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//プレイヤー情報読み込み
void CGame::LoadPlayer(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_pPlayer = CPlayer::Create(pos, size);

	if (m_pPlayer != NULL)
	{
		CCamera* pCamera = CManager::GetCamera();
		D3DXVECTOR3 cameraRot = pCamera->GetRotation();

		// プレイヤーの後ろから見る向きにする
		cameraRot.y = m_pPlayer->GetRotation().y - D3DX_PI;

		pCamera->SetRotation(cameraRot);
	}
}

//=============================================================================
//床情報読み込み
void CGame::LoadField(const char* texPath, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot)
{
	m_pField = CField::Create(texPath, pos, size, rot, CField::TYPE_FLOOR);
}

//=============================================================================
//壁情報読み込み
void CGame::LoadWall(const char* texPath, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot)
{
	m_pWall[m_nCntWall++] = CWall::Create(texPath, pos, size, rot);
}

//=============================================================================
//ターゲット情報読み込み
void CGame::LoadTarget(const char* ModelPath, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	m_pTarget[m_nCntTraget++] = CTarget::Create(CTarget::TYPE_NORMAL, ModelPath, pos, rot);
}

//=============================================================================
//objectX情報読み込み
void CGame::LoadObjectX(const char* modelType, const char* objPath, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	if (modelType == NULL || objPath == NULL)
	{
		return;
	}

	CObjectX::TYPE objectType = CObjectX::TYPE_WALL;
	if (strstr(modelType, "ceiling") != NULL)
	{
		objectType = CObjectX::TYPE_CEILING;
	}
	else if (strstr(modelType, "goal") != NULL)
	{
		objectType = CObjectX::TYPE_GOAL;
	}

	const int capacity = sizeof(m_apObjxWall) / sizeof(m_apObjxWall[0]);
	for (int index = 0; index < capacity; ++index)
	{
		if (m_apObjxWall[index] == NULL)
		{
			m_apObjxWall[index] = CObjectX::Create(objectType, objPath, pos, rot);
			return;
		}
	}
}
//=============================================================================
//有効文字抜き出し処理
void CGame::LoadEnableString(char* aStrCpy, char* pStart)
{
	while (true)
	{
		if (*pStart == '\n')
		{
			pStart++;
			continue;
		}
		else if (*pStart == '\t')
		{
			pStart++;
			continue;
		}
		else if (*pStart == 0x32)
		{
			pStart++;
			continue;
		}
		else if (*pStart == '#' || *pStart == '\0')
		{
			break;
		}
		else
		{
			strncat(aStrCpy, pStart, 1);
			pStart++;
		}
	}
}