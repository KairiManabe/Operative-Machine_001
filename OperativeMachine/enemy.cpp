//=============================================================================
//
// 敵(階層構造)[enemy.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "enemy.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "texture.h"
#include "input.h"

#include "scene.h"
#include "title.h"
#include "game.h"
#include "clear.h"
#include "fail.h"
#include "result.h"

#include "camera.h"
#include "range.h"
#include "player.h"
#include "bullet.h"
#include "model.h"
#include "meshfield.h"
#include "objectX.h"
#include "motion.h"

//マクロ定義-------------------------------------------------------------------
#define NO_USE
//#undef NO_USE

#define OFSET_HUMAN		"data/SCRIPT/player.txt"
#define OFSET_SPIDER	"data/SCRIPT/player.txt"
#define OFSET_CANNON	"data/SCRIPT/player.txt"
#define MOTION_HUMAN	"data/Motion/motion.txt"
#define MOTION_SPIDER	"data/Motion/motion.txt"
#define MOTION_CANNON	"data/Motion/motion.txt"

//静的メンバ変数宣言-----------------------------------------------------------
CEnemy::MODEL_INFO CEnemy::m_aModelInfo[30];
CMotion::INFO CEnemy::m_aMotionInfo[MOTIONTYPE_MAX];
int CEnemy::m_nNumAll = 0;

//=============================================================================
//コンストラクタ
CEnemy::CEnemy(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	memset(&m_apModel[0], NULL, sizeof(m_apModel));
	for (int nModelInfo = 0; nModelInfo < sizeof(m_aModelInfo) / sizeof(m_aModelInfo[0]); ++nModelInfo)
	{
		m_aModelInfo[nModelInfo] = MODEL_INFO{};
	}
	memset(&m_aMotionInfo[0], NULL, sizeof(m_aMotionInfo));

	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 1.57f, 0.0f);
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);

	m_bLand = false;
	m_bAction = false;
	m_pMotion = NULL;
	m_nNumModel = 0;
	m_fModelSize = 1.0f;		//デフォルトが1.0倍
	m_fCollisionSize = 25.0f;	//当たり判定のサイズ
	m_pRange = NULL;
	m_type = TYPE_NONE;
	m_nNumAll++;
}

//=============================================================================
//デストラクタ
CEnemy::~CEnemy()
{
}

//=============================================================================
//生成処理
CEnemy* CEnemy::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot, TYPE type)
{
	//テクスチャクラスの取得
	CTexture* pTexture = CManager::GetTexture();

	//プレイヤー
	CEnemy* pEnemy = NULL;

	if (pEnemy == NULL)
	{
		//プレイヤー生成
		pEnemy = new CEnemy;

		if (pEnemy != NULL)
		{
			//各種代入
			pEnemy->m_pos = pos;
			pEnemy->m_size = size;
			pEnemy->SetRotation(rot);
			pEnemy->SetType(type);		//敵の行動パターンタイプ

			//プレイヤー初期化
			pEnemy->Init(pEnemy->m_pos, pEnemy->m_size);
		}
	}
	return pEnemy;
}

//=============================================================================
//プレイヤーの初期化
HRESULT CEnemy::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	if (m_pMotion == NULL)
	{//モーションを生成
		m_pMotion = new CMotion;
	}
	else
	{
		return E_FAIL;
	}

	//階層構造のプレイヤーモデルファイル名を取得
	LoadParentModel("data/SCRIPT/drone.txt");
	LoadMotionInfo("data/Motion/motion_drone.txt");

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		//モデルの生成[0]体
		m_apModel[nCntModel] = CModel::Create(m_aModelInfo[nCntModel].FileName, m_aModelInfo[nCntModel].pos, m_aModelInfo[nCntModel].rot);

		if (m_aModelInfo[nCntModel].nIdxParent == -1)
		{
			//親モデルの設定
			m_apModel[nCntModel]->SetParent(NULL);
		}
		else
		{
			//親モデルの設定
			m_apModel[nCntModel]->SetParent(m_apModel[m_aModelInfo[nCntModel].nIdxParent]);
		}
	}

	if (m_pMotion != NULL)
	{//モーションに必要なモデルの設定
		m_pMotion->SetModel(m_apModel, m_nNumModel);
	}

	for (int nCntInfo = 0; nCntInfo < MOTIONTYPE_MAX; nCntInfo++)
	{//読み込んだモーションを代入
		m_pMotion->SetInfo(m_aMotionInfo[nCntInfo]);
	}

	//ニュートラルモーション
	m_pMotion->Set(MOTIONTYPE_NEUTRAL,15);

	//種類の設定
	CObject::SetType(TYPE_ENEMY);

	//敵の足元にサークルを生成する
	if (m_pRange == NULL)
	{
		m_pRange = CRange::Create(m_pos, D3DXVECTOR3(100.0f, 1.0f, 100.0f), m_rot, m_type);
	}

	return S_OK;
}

//=============================================================================
//プレイヤーの終了
void CEnemy::Uninit(void)
{
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL)
		{//モデルの破棄
			m_apModel[nCntModel]->Uninit();
			delete m_apModel[nCntModel];
			memset(m_apModel, NULL, sizeof(m_apModel));
		}
	}

	if (m_pMotion != NULL)
	{//モーションの破棄
		delete m_pMotion;
		m_pMotion = NULL;
	}

	if (m_pRange != NULL)
	{//索敵範囲の終了処理
		m_pRange->Uninit();
		m_pRange = NULL;
	}

	//総数を減らす
	m_nNumAll--;

	//破棄
	CObject::Release();
}

//=============================================================================
//プレイヤーの描画
void CEnemy::Draw(void)
{
	D3DXMATRIX mtxRot, mtxTrans;		//計算用マトリックス

	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_rot.y, m_rot.x, m_rot.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//モデルの描画(全パーツ)
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL)
		{
			m_apModel[nCntModel]->Draw();
		}
	}
}

//=============================================================================
//敵の更新
void CEnemy::Update(void)
{
	m_posOld = m_pos;
	
	//pos,rotをrangeから取得
	m_rot = m_pRange->GetRotation();
	m_pos.x = m_pRange->GetPosition().x;
	m_pos.z = m_pRange->GetPosition().z;

	//デバッグ表示の取得
	CDebugproc* pDebug = CManager::GetDebugproc();

	//プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	if (pPlayer != NULL)
	{
		switch (pPlayer->GetMode())
		{//プレイヤーのモードによって索敵範囲のサイズ変更
		case CPlayer::MODE_HUMAN:
			m_pRange->SetSize(D3DXVECTOR3(100.0f, 1.0f, 100.0f));
			break;

		case CPlayer::MODE_SPIDER:
			m_pRange->SetSize(D3DXVECTOR3(60.0f, 1.0f, 60.0f));
			break;

		case CPlayer::MODE_CANNON:
			m_pRange->SetSize(D3DXVECTOR3(100.0f, 1.0f, 100.0f));
			break;
		}
	}

	//モーションの更新処理
	m_pMotion->Update();

	if (m_type != CEnemy::TYPE_NONE)
	{
		m_move.x -= 0.2f * (sinf(m_rot.y));
		m_move.z -= 0.2f * (cosf(m_rot.y));

		m_pMotion->Set(MOTIONTYPE_MOVE, 3);

		//位置を更新
		m_pos.x += m_move.x;
		m_pos.z += m_move.z;
	}

	//移動量を更新
	m_move.x += (0.0f - m_move.x) * 0.12f;
	m_move.z += (0.0f - m_move.z) * 0.12f;

	//サークルの位置を敵の現在の位置(足元)に追従
	if (m_pRange != NULL)
	{
		//必要に応じてY軸のオフセットを加算
		D3DXVECTOR3 rangePos = m_pos;
		rangePos.y -= m_pos.y;

		if (m_pRange != NULL)
		{//敵の位置にrangeを持ってくる
			m_pRange->SetPosition(rangePos);
		}
	}
}

//=============================================================================
//階層構造モデル読み込み処理
HRESULT CEnemy::LoadParentModel(const std::string& fileName)
{
	FILE* pFile = fopen(fileName.c_str(), "r");

	if (pFile == NULL)
	{//読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};		    //文字列読み込み
	static char aStrCpy[MAX_STRING] = {};	    //文字列複製(整理)
	static char* pStart = NULL;				    //文字列開始位置
	static char aModelPath[FILE_MAX] = {};		//モデルのファイル名読み込み
	int nIdx = 0;								//モデルのインデックス読み込み
	int nParent = 0;							//モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};						//位置読み込み
	D3DXVECTOR3 rot = {};						//向き読み込み
	int nCntParts = 0;							//パーツ数カウント
	int nCntFileName = 0;						//ファイル名カウント
	int nCntModel = 0;							//モデル数

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
		}

		if (strstr(aStr, LOAD_MODEL))
		{//MODEL_FILENAMEを読み込んだ

			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%s", &aModelPath);

			LoadParentFile(aModelPath, nCntFileName, nCntModel);
			nCntFileName++;
		}

		if (strstr(aStr, LOAD_PLAYER))
		{//CHARACTERSETを読み込んだ
			while (true)
			{
				memset(aStr, 0, sizeof(aStr));				//文字列クリア
				memset(aStrCpy, 0, sizeof(aStrCpy));		//コピーもクリア
				(void)fgets(aStr, sizeof(aStr), pFile);		//一列読み込み
				LoadEnableString(&aStrCpy[0], &aStr[0]);	//有効文字だけ抜き取って複製

				if (strcmp(aStrCpy, LOAD_PARTS) == 0)
				{//PARTSSETを読み込んだ
					while (true)
					{
						memset(aStr, 0, sizeof(aStr));
						memset(aStrCpy, 0, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_INDEX))
						{//INDEXを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nIdx);
							continue;
						}

						if (strstr(aStr, LOAD_PARENT))
						{//PARENTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nParent);
							continue;
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

						if (strstr(aStr, LOAD_ROT))
						{//ROTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &rot.x, &rot.y, &rot.z);
							continue;
						}

						if (strcmp(aStrCpy, LOAD_ENDPARTS) == 0)
						{//END_PARTSSETを読み込んだ
							LoadOffSet(pos, rot, nParent, nIdx);
							nCntParts++;

							//値をクリア
							memset(&pos, NULL, sizeof(D3DXVECTOR3));
							memset(&rot, NULL, sizeof(D3DXVECTOR3));
							nIdx = 0;
							nParent = 0;
							break;
						}
					}
				}

				if (strcmp(aStrCpy, LOAD_ENDPLAYER) == 0)
				{//END_CHARACTERSETを読み込んだ
					nCntParts = 0;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pFile);
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//階層構造のモデルファイルをロード
void CEnemy::LoadParentFile(const std::string& fileName, int nIdxFile, int nMaxModel)
{
	if (nIdxFile < nMaxModel)
	{
		m_aModelInfo[nIdxFile].FileName = fileName;
	}
}

//=============================================================================
//オフセット読み込み処理
void CEnemy::LoadOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxParent, int nIdxModel)
{
	CDebugproc* pDebug = CManager::GetDebugproc();

	int nIdx = m_nNumModel;

	m_aModelInfo[nIdx].nIdxModel = nIdxModel;
	m_aModelInfo[nIdx].nIdxParent = nIdxParent;
	m_aModelInfo[nIdx].pos = pos;
	m_aModelInfo[nIdx].rot = rot;

	m_nNumModel++;
}

//=============================================================================
//モーション情報読み込み処理
HRESULT CEnemy::LoadMotionInfo(const std::string& fileName)
{
	FILE* pMotionFile = fopen(fileName.c_str(), "r");

	if (pMotionFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};				// 文字列読み込み
	static char aStrCpy[MAX_STRING] = {};			// 文字列複製(整理)
	static char* pStart = NULL;						// 文字列開始位置
	static char aModelPath[FILE_MAX] = {};			// モデルのファイル名読み込み
	int nIdx = 0;									// モデルのインデックス読み込み
	int nParent = 0;								// モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};							// 位置読み込み
	D3DXVECTOR3 rot = {};							// 向き読み込み
	int nNumModel = 0;								// モデル数読み込み
	int nCntMotion = 0;								// モーション数カウント
	int bLoop = false;								// ループするかどうか読み込み
	int nNumKey = 0;								// キー数読み込み
	static CMotion::KEY_INFO KeyInfo[MAX_KEY] = {};	// キー情報読み込み
	int nCntKey = 0;								// キー数カウント
	int nCntParts = 0;								// パーツ数カウント

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pMotionFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pMotionFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));
		memset(aStrCpy, NULL, sizeof(aStrCpy));
		(void)fgets(aStr, sizeof(aStr), pMotionFile);
		LoadEnableString(&aStrCpy[0], &aStr[0]);

		if (strcmp(aStrCpy, LOAD_MOTIONINFO) == 0)
		{// MOTIONSETを読み込んだ
			while (true)
			{
				memset(aStr, NULL, sizeof(aStr));
				memset(aStrCpy, NULL, sizeof(aStrCpy));
				(void)fgets(aStr, sizeof(aStr), pMotionFile);
				LoadEnableString(&aStrCpy[0], &aStr[0]);

				if (strstr(aStr, LOAD_MOTIONLOOP))
				{// LOOPを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &bLoop);
					continue;
				}

				if (strstr(aStr, LOAD_NUMKEY))
				{// NUM_KEYを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &nNumKey);
					continue;
				}

				if (strcmp(aStrCpy, LOAD_KEYSET) == 0)
				{// KEYSETを読み込んだ
					while (true)
					{
						memset(aStr, NULL, sizeof(aStr));
						memset(aStrCpy, NULL, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pMotionFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_FRAME))
						{// FRAMEを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &KeyInfo[nCntKey].nFrame);
							continue;
						}

						if (strcmp(aStrCpy, LOAD_KEY) == 0)
						{// KEYを読み込んだ
							while (true)
							{
								memset(aStr, NULL, sizeof(aStr));
								memset(aStrCpy, NULL, sizeof(aStrCpy));
								(void)fgets(aStr, sizeof(aStr), pMotionFile);
								LoadEnableString(&aStrCpy[0], &aStr[0]);

								if (strstr(aStr, LOAD_POS))
								{// POSを読み込んだ
									if ((pStart = strchr(aStr, '=')) == NULL)
									{
										continue;
									}

									(void)sscanf(pStart + 1, "%f %f %f", &KeyInfo[nCntKey].aKey[nCntParts].fPosX, &KeyInfo[nCntKey].aKey[nCntParts].fPosY, &KeyInfo[nCntKey].aKey[nCntParts].fPosZ);
									continue;
								}

								if (strstr(aStr, LOAD_ROT))
								{// ROTを読み込んだ
									if ((pStart = strchr(aStr, '=')) == NULL)
									{
										continue;
									}

									(void)sscanf(pStart + 1, "%f %f %f", &KeyInfo[nCntKey].aKey[nCntParts].fRotX, &KeyInfo[nCntKey].aKey[nCntParts].fRotY, &KeyInfo[nCntKey].aKey[nCntParts].fRotZ);
									continue;
								}

								if (strcmp(aStrCpy, LOAD_ENDKEY) == 0)
								{// END_KEYを読み込んだ
									nCntParts++;
									break;
								}
							}
						}

						if (strcmp(aStrCpy, LOAD_ENDKEYSET) == 0)
						{// END_KEYSET
							nCntKey++;
							nCntParts = 0;
							break;
						}
					}
				}

				if (strcmp(aStrCpy, LOAD_ENDMOTIONINFO) == 0)
				{// END_MOTIONSETを読み込んだ
					LoadMotion(bLoop, nNumKey, &KeyInfo[0], nCntMotion);
					nCntMotion++;

					// 各種変数初期化
					memset(&KeyInfo, NULL, sizeof(CMotion::KEY_INFO));
					bLoop = false;
					nNumKey = 0;
					nCntKey = 0;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pMotionFile);
			break;
		}
	}
	return S_OK;
}

//=============================================================================
//	モーション読み込み処理
void CEnemy::LoadMotion(bool bLoop, int nNumKey, CMotion::KEY_INFO* pKeyInfo, int nMotion)
{
	if (nMotion < MOTIONTYPE_MAX)
	{
		m_aMotionInfo[nMotion].bLoop = bLoop;
		m_aMotionInfo[nMotion].nNumKey = nNumKey;

		for (int nCntKey = 0; nCntKey < m_aMotionInfo[nMotion].nNumKey; nCntKey++)
		{
			m_aMotionInfo[nMotion].aKeyInfo[nCntKey] = pKeyInfo[nCntKey];
		}
	}
}

//=============================================================================
//有効文字抜き出し処理
void CEnemy::LoadEnableString(char* aStrCpy, char* pStart)
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