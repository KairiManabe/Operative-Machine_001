//=============================================================================
// 
// モデル管理クラス[ModelManager.h]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "main.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_MODELNUM (100) //保持できる最大モデル数

//モデル管理クラス-------------------------------------------------------------
class CModelManager
{
public:
	//管理するモデルの情報構造体
	struct MODEL_DATA
	{
		LPD3DXMESH pMesh;          //メッシュポインタ
		LPD3DXBUFFER pBuffMat;     //マテリアルバッファ
		DWORD dwNumMat;            //マテリアル数
		int nIdxTexture[32];       //テクスチャインデックス一覧
		D3DXVECTOR3 vtxMax;        //バウンディングボックス最大
		D3DXVECTOR3 vtxMin;        //バウンディングボックス最小
		char aFilename[256];       //ファイル名
	};

	CModelManager();
	~CModelManager();

	void Unload(void);

	//モデルの登録・読み込み（あるなら登録済みのインデックスを返す）
	int Register(const char* pFilename);

	//モデルデータの取得
	MODEL_DATA* GetModelData(int nIdx);

private:
	MODEL_DATA m_aModelData[MAX_MODELNUM];
	static int m_nNumAll;
};