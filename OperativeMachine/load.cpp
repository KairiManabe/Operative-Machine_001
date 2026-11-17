//=============================================================================
// 
// ファイル読み込み[load.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "load.h"
#include "manager.h"
#include "player.h"
#include "debugproc.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_STRING			(256)						// 読み込める文字列の最大
#define MAX_MAGICOBJECTFILE	(10)						// 魔法オブジェクトファイルの最大
#define LOAD_START			"SCRIPT"					// 読み込み
#define LOAD_END			"END_SCRIPT"				// 終了
#define LOAD_OBJECT			"OBJECTSCRIPT"				// オブジェクトスクリプト読み込み
#define LOAD_MAGICOBJECT	"MAGICOBJECTSCRIPT"			// 魔法オブジェクトスクリプト読み込み
#define LOAD_PARENTMODEL	"PARENTSCRIPT"				// 階層構造モデルスクリプト読み込み
#define LOAD_PLAYER			"CHARACTERSET"				// プレイヤー情報読み込み
#define LOAD_ENDPLAYER		"END_CHARACTERSET"			// プレイヤー情報読み込み終了
#define LOAD_PARTS			"PARTSSET"					// パーツ情報読み込み
#define LOAD_ENDPARTS		"END_PARTSSET"				// パーツ情報読み込み終了
#define LOAD_MOTION			"MOTIONSCRIPT"				// モーションスクリプト読み込み
#define LOAD_MOTIONINFO		"MOTIONSET"					// モーション情報読み込み
#define LOAD_ENDMOTIONINFO	"END_MOTIONSET"				// モーション情報読み込み終了
#define LOAD_NUMMODEL		"NUM_MODEL"					// モデル数読み込み
#define LOAD_MODEL			"MODEL_FILENAME"			// モデルファイル名読み込み
#define LOAD_CUSTOMMESHFILE	"CUSTOMMESH_FILENAME"		// カスタムメッシュファイル名読み込み
#define LOAD_ITEM			"ITEM_FILENAME"				// アイテムファイル名読み込み
#define LOAD_MOTIONLOOP		"LOOP"						// ループモーションか読み込み
#define LOAD_NUMKEY			"NUM_KEY"					// キー数読み込み
#define LOAD_KEYSET			"KEYSET"					// キー情報読み込み
#define LOAD_ENDKEYSET		"END_KEYSET"				// キー情報読み込み終了
#define LOAD_FRAME			"FRAME"						// フレーム数読み込み
#define LOAD_KEY			"KEY"						// キー読み込み
#define LOAD_ENDKEY			"END_KEY"					// キー読み込み終了
#define LOAD_MODE			"MODE"						// モード読み込み
#define LOAD_POS			"POS"						// 位置読み込み
#define LOAD_ROT			"ROT"						// 向き読み込み
#define LOAD_WIDTH			"WIDTH"						// 幅読み込み
#define LOAD_HEIGHT			"HEIGHT"					// 高さ読み込み
#define LOAD_DEPTH			"DEPTH"						// 奥行き読み込み
#define LOAD_TYPE			"TYPE"						// 種類読み込み
#define LOAD_PARENTTYPE		"PARENTTYPE"				// 階層構造モデルの種類読み込み
#define LOAD_MOTIONTYPE		"MOTIONTYPE"				// モーションデータの種類読み込み
#define LOAD_INDEX			"INDEX"						// インデックス読み込み
#define LOAD_PARENT			"PARENT"					// 親インデックス読み込み
#define LOAD_MODELINFO		"MODELSET"					// モデル情報読み込み
#define LOAD_ENDMODELINFO	"END_MODELSET"				// モデル情報読み込み終了

//=============================================================================
//コンストラクタ
CLoad::CLoad()
{
	//値をクリア(初期化)
	memset(&m_aInfo[0], 0, sizeof(m_aInfo));
	memset(&m_ModelInfo[0], 0, sizeof(m_ModelInfo));
	m_nNumModel = 0;
	m_nNumMotion = 0;
	m_nNumParentModel = 0;
	m_nNumModel = 0;						// モデル数初期化
	m_nNumMotion = 0;						// モーション数初期化
	m_nNumParentModel = 0;					// 階層構造モデル数初期化
}

//=============================================================================
//デストラクタ
CLoad::~CLoad()
{
}

//=============================================================================
//スクリプトの読み込み処理
HRESULT CLoad::LoadScript(const std::string& fileName)
{
	// 受け取ったパスからファイルを開く
	FILE* pScriptFile = fopen(fileName.c_str(), "r");

	if (pScriptFile == NULL)
	{// スクリプト読み込み失敗
		return E_FAIL;
	}

	char aStr[MAX_STRING] = {};				// 文字列格納
	char aScriptPath[FILENAME_MAX] = {};	// スクリプトファイルパス
	char aTexPath[FILENAME_MAX] = {};		// テクスチャファイルパス
	char* pStart = NULL;					// 文字列の開始位置
	int type = 0;							// 種類

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));				// 文字列初期化
		fgets(aStr, sizeof(aStr), pScriptFile);		// 一行読み取り

		if (strstr(aStr, LOAD_START) != NULL)
		{// 読み込み開始
			break;
		}

		if (feof(pScriptFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));						// 文字列初期化
		(void)fgets(aStr, sizeof(aStr), pScriptFile);		// 一行読み取り

		if (strstr(aStr, LOAD_MOTION) != NULL)
		{// MOTIONSCRIPTを読み込んだ
			pStart = strchr(aStr, '=');

			(void)sscanf(pStart + 1, "%s %d", aScriptPath, &type);

			LoadMotionInfo(aScriptPath);
		}

		if (strstr(aStr, LOAD_PARENTMODEL) != NULL)
		{// PARENTSCRIPTを読み込んだ
			pStart = strchr(aStr, '=');

			(void)sscanf(pStart + 1, "%s %d", aScriptPath, &type);

			LoadParentModel(aScriptPath);
		}

		if (strstr(aStr, LOAD_END))
		{// END_SCRIPTを読み込んだ
			fclose(pScriptFile);
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//階層構造モデル読み込み処理
HRESULT CLoad::LoadParentModel(const std::string& fileName)
{
	CLoad* pLoad = CManager::GetLoad();

	FILE* pFile = fopen(fileName.c_str(), "r");

	if (pFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};		    // 文字列読み込み
	static char aStrCpy[MAX_STRING] = {};	    // 文字列複製(整理)
	static char* pStart = NULL;				    // 文字列開始位置
	static char aModelPath[FILENAME_MAX] = {};	// モデルのファイル名読み込み
	int nIdx = 0;								// モデルのインデックス読み込み
	int nParent = 0;							// モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};						// 位置読み込み
	D3DXVECTOR3 rot = {};						// 向き読み込み
	int nCntParts = 0;							// パーツ数カウント
	int nCntFileName = 0;						// ファイル名カウント
	int nCntModel = 0;							// モデル数

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));			// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));				// 文字列クリア
		memset(aStrCpy, 0, sizeof(aStrCpy));		// コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pFile);		// 一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);	// 有効文字だけ抜き取って複製

		if (strstr(aStr, LOAD_NUMMODEL))
		{// NUM_MODELを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%d", &nCntModel);
		}

		if (strstr(aStr, LOAD_MODEL))
		{// MODEL_FILENAMEを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%s", &aModelPath);

			LoadParentFile(aModelPath, nCntFileName, nCntModel);
			nCntFileName++;
		}

		if (strstr(aStr, LOAD_PLAYER))
		{// CHARACTERSETを読み込んだ
			while (true)
			{
				memset(aStr, 0, sizeof(aStr));				// 文字列クリア
				memset(aStrCpy, 0, sizeof(aStrCpy));		// コピーもクリア
				(void)fgets(aStr, sizeof(aStr), pFile);		// 一列読み込み
				LoadEnableString(&aStrCpy[0], &aStr[0]);	// 有効文字だけ抜き取って複製

				if (strcmp(aStrCpy, LOAD_PARTS) == 0)
				{// PARTSSETを読み込んだ
					while (true)
					{
						memset(aStr, 0, sizeof(aStr));
						memset(aStrCpy, 0, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_INDEX))
						{// INDEXを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nIdx);
							continue;
						}

						if (strstr(aStr, LOAD_PARENT))
						{// PARENTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nParent);
							continue;
						}

						if (strstr(aStr, LOAD_POS))
						{// POSを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);
							continue;
						}

						if (strstr(aStr, LOAD_ROT))
						{// ROTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &rot.x, &rot.y, &rot.z);
							continue;
						}

						if (strcmp(aStrCpy, LOAD_ENDPARTS) == 0)
						{// END_PARTSSETを読み込んだ
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
				{// END_CHARACTERSETを読み込んだ
					nCntParts = 0;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pFile);
			pLoad->m_nNumParentModel++;
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//モーション情報読み込み処理
HRESULT CLoad::LoadMotionInfo(const std::string& fileName)
{
	CLoad* pLoad = CManager::GetLoad();
	FILE* pMotionFile = fopen(fileName.c_str(), "r");

	if (pMotionFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};					// 文字列読み込み
	static char aStrCpy[MAX_STRING] = {};				// 文字列複製(整理)
	static char* pStart = NULL;						// 文字列開始位置
	static char aModelPath[FILENAME_MAX] = {};			// モデルのファイル名読み込み
	int nIdx = 0;								// モデルのインデックス読み込み
	int nParent = 0;							// モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};						// 位置読み込み
	D3DXVECTOR3 rot = {};						// 向き読み込み
	int nNumModel = 0;							// モデル数読み込み
	int nCntMotion = 0;							// モーション数カウント
	int bLoop = false;							// ループするかどうか読み込み
	int nNumKey = 0;							// キー数読み込み
	static CMotion::KEY_INFO KeyInfo[MAX_KEY] = {};	// キー情報読み込み
	int nCntKey = 0;							// キー数カウント
	int nCntParts = 0;							// パーツ数カウント

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
					LoadMotion(bLoop, nNumKey, &KeyInfo[0], nCntMotion, pLoad->m_nNumMotion);
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
			pLoad->m_nNumMotion++;
			break;
		}
	}
	return S_OK;
}

//=============================================================================
//有効文字抜き出し処理
void CLoad::LoadEnableString(char* aStrCpy, char* pStart)
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

//=============================================================================
//階層構造のモデルファイルをロード
void CLoad::LoadParentFile(const std::string& fileName,int nIdxFile, int nMaxModel)
{
	CLoad* pLoad = CManager::GetLoad();

	if (pLoad != NULL)
	{
		if (nIdxFile < nMaxModel)
		{
			pLoad->m_ModelInfo[nIdxFile].FileName = fileName;
		}
	}
}

//=============================================================================
//オフセット読み込み処理
void CLoad::LoadOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxParent, int nIdxModel)
{
	CDebugproc* pDebug = CManager::GetDebugproc();
	CLoad* pLoad = CManager::GetLoad();

	if (pLoad != NULL)
	{
		int nIdx = pLoad->m_nNumModel;

		pLoad->m_ModelInfo[nIdx].nIdxModel = nIdxModel;			//ここと
		pLoad->m_ModelInfo[nIdx].nIdxParent = nIdxParent;		//ここでInfoのデータ分一個分外部ファイルが抜ける
		pLoad->m_ModelInfo[nIdx].pos = pos;
		pLoad->m_ModelInfo[nIdx].rot = rot;

		pLoad->m_nNumModel++;
	}
}

//=============================================================================
//	モーション読み込み処理
void CLoad::LoadMotion(bool bLoop, int nNumKey, CMotion::KEY_INFO* pKeyInfo, int nMotion, int nNumMotion)
{
	CLoad* pLoad = CManager::GetLoad();

	pLoad->m_aInfo[nMotion].bLoop = bLoop;
	pLoad->m_aInfo[nMotion].nNumKey = nNumKey;

	for (int nCntKey = 0; nCntKey < pLoad->m_aInfo[nMotion].nNumKey; nCntKey++)
	{
		pLoad->m_aInfo[nMotion].aKeyInfo[nCntKey] = pKeyInfo[nCntKey];
	}
}
