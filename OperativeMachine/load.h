//=============================================================================
// 
// ファイル読み込み[load.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _LOAD_H_					//このマクロが定義されてなかったら
#define _LOAD_H_					//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_MOTION			(20)
#define MAX_STRING			(256)						// 読み込める文字列の最大
#define FILE_MAX			(256)
#define LOAD_START			"SCRIPT"					// 読み込み
#define LOAD_END			"END_SCRIPT"				// 終了
#define LOAD_OBJECT			"OBJECTSET"					//オブジェクトスクリプト読み込み
#define LOAD_MAGICOBJECT	"MAGICOBJECTSCRIPT"			// 魔法オブジェクトスクリプト読み込み
#define LOAD_PARENTMODEL	"PARENTSCRIPT"				// 階層構造モデルスクリプト読み込み
#define LOAD_PLAYER			"CHARACTERSET"				//プレイヤー情報読み込み
#define LOAD_ENDPLAYER		"END_CHARACTERSET"			//プレイヤー情報読み込み終了
#define LOAD_PARTS			"PARTSSET"					//パーツ情報読み込み
#define LOAD_ENDPARTS		"END_PARTSSET"				//パーツ情報読み込み終了
#define LOAD_MOTION			"MOTIONSCRIPT"				//モーションスクリプト読み込み
#define LOAD_MOTIONINFO		"MOTIONSET"					//モーション情報読み込み
#define LOAD_ENDMOTIONINFO	"END_MOTIONSET"				//モーション情報読み込み終了
#define LOAD_NUMMODEL		"NUM_MODEL"					//モデル数読み込み
#define LOAD_MODEL			"MODEL_FILENAME"			//モデルファイル名読み込み
#define LOAD_ITEM			"ITEM_FILENAME"				//アイテムファイル名読み込み
#define LOAD_MOTIONLOOP		"LOOP"						//ループモーションか読み込み
#define LOAD_NUMKEY			"NUM_KEY"					//キー数読み込み
#define LOAD_KEYSET			"KEYSET"					//キー情報読み込み
#define LOAD_ENDKEYSET		"END_KEYSET"				//キー情報読み込み終了
#define LOAD_FRAME			"FRAME"						//フレーム数読み込み
#define LOAD_KEY			"KEY"						//キー読み込み
#define LOAD_ENDKEY			"END_KEY"					//キー読み込み終了
#define LOAD_MODE			"MODE"						//モード読み込み
#define LOAD_POS			"POS"						//位置読み込み
#define LOAD_SIZE			"SIZE"						//位置読み込み
#define LOAD_ROT			"ROT"						//向き読み込み
#define LOAD_WIDTH			"WIDTH"						//幅読み込み
#define LOAD_HEIGHT			"HEIGHT"					//高さ読み込み
#define LOAD_DEPTH			"DEPTH"						//奥行き読み込み
#define LOAD_TYPE			"TYPE"						//種類読み込み
#define LOAD_PARENTTYPE		"PARENTTYPE"				//階層構造モデルの種類読み込み
#define LOAD_MOTIONTYPE		"MOTIONTYPE"				//モーションデータの種類読み込み
#define LOAD_INDEX			"INDEX"						//インデックス読み込み
#define LOAD_PARENT			"PARENT"					//親インデックス読み込み
#define LOAD_MODELINFO		"MODELSET"					//モデル情報読み込み
#define LOAD_ENDMODELINFO	"END_MODELSET"				//モデル情報読み込み終了
#define LOAD_PARENTMODEL	"PARENTSCRIPT"				//階層構造モデルスクリプト読み込み
#define LOAD_OBJECTEND		"END_SET"					//オブジェクト情報読み込み
#define LOAD_NUMMODEL		"NUM_MODEL"					//モデル数読み込み
#define LOAD_OBJECTFILE		"OBJECT_FILENAME"			//objectファイル名読み込み
#define LOAD_TEXTUREFILE	"TEXTURE_FILENAME"			//textureファイル名読み込み
#define LOAD_MODELKIND		"MODEL_KIND"				//モデルのタイプ名取得

#if 0
//ロードクラス-----------------------------------------------------------------
class CLoad
{
public:
	CLoad();
	~CLoad();

	struct MODEL_INFO
	{
		std::string FileName;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		int nIdxParent;
		int nIdxModel;
	};

	static HRESULT LoadScript(const std::string& fileName);
	static HRESULT LoadParentModel(const std::string& fileName);
	static HRESULT LoadMotionInfo(const std::string& fileName);
	static void LoadEnableString(char* aStrCpy, char* pStart);
	static void LoadParentFile(const std::string& fileName, int nIdxFile,int nMaxModel);
	static void LoadOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxParent, int nIdxModel);
	static void LoadMotion(bool bLoop, int nNumKey, CMotion::KEY_INFO* pKeyInfo, int nMotion, int nNumMotion);
	MODEL_INFO GetModelInfo(const int nIdx) { return m_ModelInfo[nIdx]; }
	CMotion::INFO GetMotionInfo(const int nIdx) { return m_aInfo[nIdx]; }
	int GetNumModel(void) { return m_nNumModel; }
private:
	MODEL_INFO m_ModelInfo[30];
	CMotion::INFO m_aInfo[MAX_MOTION];
	int m_nNumModel;
	int m_nNumMotion;
	int m_nNumParentModel;
};
#endif
#endif