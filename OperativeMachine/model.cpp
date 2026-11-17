//=============================================================================
// 
// モデルクラス[model.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルード-----------------------------------------------------------------
#include "model.h"

#include "manager.h"
#include "renderer.h"
#include "ModelManager.h"
#include "debugproc.h"
#include "texture.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CModel::m_nParentModel = 0;

//=============================================================================
//コンストラクタ
CModel::CModel()
{
	//値をクリア(初期化)
	m_pMesh = NULL;
	m_pBuffMat = NULL;
	m_dwNumMat = NULL;
	m_pParent = NULL;
	m_FileName = {};
	m_mtxWorld = {};
	m_vtxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vtxMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	memset(&m_nIdxTexture[0], 0, sizeof(m_nIdxTexture));
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_nIdxModel = 0;
	m_fSize = 1.0f;
}

//=============================================================================
//デストラクタ
CModel::~CModel()
{
}

//=============================================================================
//モデルの生成
CModel* CModel::Create(const std::string& fileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//プレイヤー
	CModel* pModel = NULL;

	if (pModel == NULL)
	{
		//弾生成
		pModel = new CModel;

		if (pModel != NULL)
		{
			//各種代入
			//pModel->m_pos = pos;
			pModel->m_offsetPos = pos;

			//pModel->m_rot = rot;
			pModel->m_offsetRot = rot;

			pModel->Init(fileName);
		}
	}
	return pModel;
}

//=============================================================================
//オブジェクトの初期化
void CModel::Init(const std::string& fileName)
{
	CModelManager* pModelManager = CManager::GetModelManager();
	m_FileName = fileName;

	//登録（既にロード済みならキャッシュから即座にインデックスが返る）
	m_nIdxModel = pModelManager->Register(m_FileName.c_str());

	//読み込んだ共有モデルデータから境界情報等を設定
	CModelManager::MODEL_DATA* pModelData = pModelManager->GetModelData(m_nIdxModel);
	if (pModelData != NULL)
	{
		m_vtxMax = pModelData->vtxMax;
		m_vtxMin = pModelData->vtxMin;
	}
}

//=============================================================================
//オブジェクトの終了
void CModel::Uninit(void)
{
	//メッシュの破棄
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	//マテリアルの破棄
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat->Release();
		m_pBuffMat = NULL;
	}
}

//=============================================================================
//オブジェクトの更新
void CModel::Update(void)
{
	//修正
	if (m_rot.y <= -D3DX_PI)
	{
		m_rot.y += (D3DX_PI * 2);
	}
	else if (m_rot.y >= D3DX_PI)
	{
		m_rot.y -= (D3DX_PI * 2);
	}

	if (m_rot.x <= -D3DX_PI)
	{
		m_rot.x += D3DX_PI;
	}
	else if (m_rot.x >= D3DX_PI)
	{
		m_rot.x -= (D3DX_PI * 2);
	}

	if (m_rot.z <= -D3DX_PI)
	{
		m_rot.z += (D3DX_PI * 2);
	}
	else if (m_rot.z >= D3DX_PI)
	{
		m_rot.z -= (D3DX_PI * 2);
	}
}

//=============================================================================
//オブジェクトの描画
void CModel::Draw(void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//モデルデータを取得
	CModelManager::MODEL_DATA* pData = CManager::GetModelManager()->GetModelData(m_nIdxModel);

	if (pData == NULL || pData->pMesh == NULL)
	{
		return;
	}

	D3DXMATRIX mtxRot, mtxTrans, mtxSize;			//計算用マトリックス
	D3DXVECTOR3 pos, rot;

	pos = (m_pos) + m_offsetPos;
	//pos = m_pos + m_offsetPos;
	rot = m_rot + m_offsetRot;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//モデルのサイズ
#if 0
	D3DXMatrixScaling(&mtxSize,
		m_fSize, m_fSize, m_fSize);
	
	//サイズを反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);
#endif

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		rot.y, rot.x, rot.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		pos.x, pos.y, pos.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//親のマトリックスと掛け合わせる
	D3DXMATRIX mtxParent;

	if (m_pParent != NULL)
	{//親が存在する
		//親モデルのマトリックスを取得
		mtxParent = m_pParent->GetMtxWorld();

		m_nParentModel++;
	}
	else
	{//親が存在しない
		//現在(最新)のマトリックスを取得する[= プレイヤーのマトリックス]
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	//親のマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//=========================================================================
	//モデル管理クラスを使う場合
	D3DXMATERIAL* pMat = (D3DXMATERIAL*)pData->pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)pData->dwNumMat; nCntMat++)
	{
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);
		pDevice->SetTexture(0, CManager::GetTexture()->GetAddres(pData->nIdxTexture[nCntMat]));
		pData->pMesh->DrawSubset(nCntMat);
	}
}

//=============================================================================
//シャドーマトリックスの描画
void CModel::DrawShadow(const D3DXMATRIX& shadowMatrix)
{
	CRenderer* renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;
	D3DXVECTOR3 pos = m_pos + m_offsetPos;
	D3DXVECTOR3 rot = m_rot + m_offsetRot;

	D3DXMatrixIdentity(&m_mtxWorld);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_pParent != NULL)
	{
		mtxParent = m_pParent->GetMtxWorld();
	}
	else
	{
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);
	D3DXMATRIX shadowWorld;
	D3DXMatrixMultiply(&shadowWorld, &m_mtxWorld, &shadowMatrix);
	pDevice->SetTransform(D3DTS_WORLD, &shadowWorld);

	//メッシュ描画（CModelManager経由）
	CModelManager::MODEL_DATA* pData = CManager::GetModelManager()->GetModelData(m_nIdxModel);
	for (int nCntMat = 0; nCntMat < (int)pData->dwNumMat; ++nCntMat)
	{
		pData->pMesh->DrawSubset(nCntMat);
	}
}

//=============================================================================
//親子関係の設定
void CModel::SetParent(CModel* pModel)
{
	m_pParent = pModel;
}