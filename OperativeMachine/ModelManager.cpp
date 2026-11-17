//=============================================================================
// 
// モデル管理クラス[ModelManager.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "ModelManager.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CModelManager::m_nNumAll = 0;

//=============================================================================
//コンストラクタ
CModelManager::CModelManager()
{//初期化
	memset(&m_aModelData[0], 0, sizeof(m_aModelData));
}

//=============================================================================
//デストラクタ
CModelManager::~CModelManager()
{
}

//=============================================================================
//コンストラクタ
void CModelManager::Unload(void)
{//すべてのモデル・バッファの破棄
	for (int nCnt = 0; nCnt < MAX_MODELNUM; nCnt++)
	{
		if (m_aModelData[nCnt].pMesh != NULL)
		{
			m_aModelData[nCnt].pMesh->Release();
			m_aModelData[nCnt].pMesh = NULL;
		}

		if (m_aModelData[nCnt].pBuffMat != NULL)
		{
			m_aModelData[nCnt].pBuffMat->Release();
			m_aModelData[nCnt].pBuffMat = NULL;
		}
	}

	//0にする
	m_nNumAll = 0;
}

//=============================================================================
//モデルの登録処理
int CModelManager::Register(const char* pFilename)
{
	//レンダラーの取得・デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//テクスチャ管理クラスの取得
	CTexture* pTexture = CManager::GetTexture();

	if (pFilename == NULL) return -1;

	//すでに同じファイル名のモデルが読み込まれているかチェック
	for (int nCntIdx = 0; nCntIdx < MAX_MODELNUM; nCntIdx++)
	{
		if (m_aModelData[nCntIdx].pMesh != NULL)
		{
			if (strcmp(m_aModelData[nCntIdx].aFilename, pFilename) == 0)
			{
				return nCntIdx; //登録されているインデックスを返す
			}
		}
	}

	//空きスロットを探して新規読み込み
	for (int nCntIdx = 0; nCntIdx < MAX_MODELNUM; nCntIdx++)
	{
		if (m_aModelData[nCntIdx].pMesh == NULL)
		{
			MODEL_DATA* pData = &m_aModelData[nCntIdx];

			// Xファイルの読み込み
			if (FAILED(D3DXLoadMeshFromX(pFilename,
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				&pData->pBuffMat,
				NULL,
				&pData->dwNumMat,
				&pData->pMesh)))
			{
				return -1; //読み込み失敗
			}

			D3DXMATERIAL* pMat = (D3DXMATERIAL*)pData->pBuffMat->GetBufferPointer();
			for (int nCntMat = 0; nCntMat < (int)pData->dwNumMat; nCntMat++)
			{//テクスチャの登録
				pData->nIdxTexture[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
			}

			//頂点情報・バウンディングボックス（Min/Max）の計算
			int nNumVtx = pData->pMesh->GetNumVertices();

			//頂点フォーマットのサイズを取得
			DWORD dwSizeFVF = D3DXGetFVFVertexSize(pData->pMesh->GetFVF());
			BYTE* pVtxBuff = NULL;

			pData->vtxMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);
			pData->vtxMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);

			//頂点バッファをロック
			pData->pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

			for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
			{
				D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

				if (vtx.x < pData->vtxMin.x)
				{
					pData->vtxMin.x = vtx.x;
				}
				if (vtx.y < pData->vtxMin.y)
				{
					pData->vtxMin.y = vtx.y;
				}
				if (vtx.z < pData->vtxMin.z)
				{
					pData->vtxMin.z = vtx.z;
				}

				if (vtx.x > pData->vtxMax.x)
				{
					pData->vtxMax.x = vtx.x;
				}
				if (vtx.y > pData->vtxMax.y) 
				{
					pData->vtxMax.y = vtx.y;
				}
				if (vtx.z > pData->vtxMax.z)
				{
					pData->vtxMax.z = vtx.z;
				}

				pVtxBuff += dwSizeFVF;
			}
			pData->pMesh->UnlockVertexBuffer();

			//ファイル名を保存
			strncpy_s(pData->aFilename, sizeof(pData->aFilename), pFilename, _TRUNCATE);

			//総数を増やす
			m_nNumAll++;
			return nCntIdx;
		}
	}

	return -1;
}

//=============================================================================
//データの取得
CModelManager::MODEL_DATA* CModelManager::GetModelData(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODELNUM)
	{//0より下かMAX_MODELNUMより多いときNULLを返す
		return NULL;
	}

	//nIdx版の配列を返す
	return &m_aModelData[nIdx];
}