//=============================================================================
//
// モデルのエフェクト[effectmodel.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "effectmodel.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "input.h"
#include "texture.h"
#include "ModelManager.h"
#include "color.h"

#include "tutorial.h"
#include "game.h"

#include "player.h"
#include "number.h"
#include "camera.h"
#include "particle.h"
#include "field.h"
#include "wall.h"

//マクロ定義-------------------------------------------------------------------
#define COLLISION_MODEL_W		(15.0f)
#define COLLISION_MODEL_H		(10.0f)

//静的メンバ変数宣言-----------------------------------------------------------
bool CEffectModel::m_bWallClim = false;
int CEffectModel::m_wallnum = 0;
int CEffectModel::m_nNumAll = 0;

//=============================================================================
//コンストラクタ
CEffectModel::CEffectModel(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	memset(&m_nIdxTexture[0], 0, sizeof(m_nIdxTexture));
	//memset(&m_nIdxModel[0], 0, sizeof(m_nIdxModel));
	m_nIdxModel = 0;
	D3DXMatrixIdentity(&m_mtxWorld);

	m_vtxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vtxMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_moveRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_pFileName = "";
	m_type = EFFECT_NONE;
	m_nFloating = 0;
	m_fInit = 0.0f;
	m_nLife = 0;
	m_pEffModel = NULL;
	m_nNumAll++;		//総数を加算
}

//=============================================================================
//デストラクタ
CEffectModel::~CEffectModel()
{
}

//=============================================================================
//モデルの生成
CEffectModel* CEffectModel::Create(TYPE type, const std::string& fileName, D3DXVECTOR3 pos, D3DXVECTOR3 move, int nLife)
{
	//プレイヤー
	CEffectModel* pObjX = NULL;

	if (pObjX == NULL)
	{
		//弾生成
		pObjX = new CEffectModel;

		if (pObjX != NULL)
		{
			//各種代入
			pObjX->SetPosition(pos);
			pObjX->m_move = move;
			pObjX->m_pFileName = fileName;
			pObjX->m_type = type;
			pObjX->m_nLife = nLife;

			pObjX->SetEffModel(pObjX);
			pObjX->Init(pObjX->m_pos, pObjX->m_size);
		}
	}
	return pObjX;
}

//=============================================================================
//オブジェクトの初期化
HRESULT CEffectModel::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	CModelManager* pModelManager = CManager::GetModelManager();

	const char* pFilePath = NULL;

	if (m_type == EFFECT_BREAK)
	{
		switch (m_nNumAll % 5)
		{
		case 0:
			pFilePath = "data/MODEL/effect/break000.x";
			break;
		case 1:
			pFilePath = "data/MODEL/effect/break001.x";
			break;
		case 2:
			pFilePath = "data/MODEL/effect/break002.x";
			break;
		case 3:
			pFilePath = "data/MODEL/effect/break003.x";
			break;
		case 4:
			pFilePath = "data/MODEL/effect/break004.x";
			break;
		}
	}
	else if (m_type == EFFECT_DETH_HUMAN)
	{
		pFilePath = m_pFileName.c_str();
	}

	//登録（既にロード済みならキャッシュから即座にインデックスが返る）
	m_nIdxModel = pModelManager->Register(pFilePath);

	//読み込んだ共有モデルデータから境界情報等を設定
	CModelManager::MODEL_DATA* pModelData = pModelManager->GetModelData(m_nIdxModel);
	if (pModelData != NULL)
	{
		m_vtxMax = pModelData->vtxMax;
		m_vtxMin = pModelData->vtxMin;
	}

	// Yの初期位置を保存
	m_fInit = m_pos.y;

	return S_OK;
}

//=============================================================================
//オブジェクトの終了
void CEffectModel::Uninit(void)
{
	if (m_pEffModel != NULL)
	{
		m_pEffModel = NULL;
	}

	//総数を減らす
	m_nNumAll--;

	//破棄
	CObject::Release();
}

//=============================================================================
//オブジェクトの更新
void CEffectModel::Update(void)
{
	//各種取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();
	CDebugproc* pDebug = CManager::GetDebugproc();
	CCamera* pCamera = CManager::GetCamera();
	
	D3DXVECTOR3 rotCamera = pCamera->GetRotation();

	CField* pField = NULL;			//ゲーム管理クラスからフィールドを取得
	CScene* pScene = CManager::GetScene();

	if (pScene->GetMode() == CScene::MODE_GAME)
	{
		pField = CGame::GetField();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	{
		//pField = CTutorial::GetField();
		return;
	}

	//オブジェクトの中心座標を計算する
	m_Center.x = m_pos.x + (m_vtxMax.x + m_vtxMin.x) * 0.5f;
	m_Center.z = m_pos.z + (m_vtxMax.z + m_vtxMin.z) * 0.5f;

	if (m_type == EFFECT_BREAK || m_type == EFFECT_DETH_HUMAN)
	{
		for (int nCntField = 0; nCntField < CField::GetNumAll(); nCntField++)
		{//床の上に出現
			if (pField != NULL)
			{
				if (pField->CollisionModelEffect(m_pEffModel) == true && m_pos.y <= pField->GetHeight() + 1.0f && pField->GetType() == CField::TYPE_FLOOR)
				{
					m_pos.y = pField->GetHeight() + 1.0f;
					m_move = { 0.0f,0.0f,0.0f }; 

					/*if (m_fInit - m_pos.y >= 2.5f)
					{
						m_move.y += 2.0f;
						m_fInit = m_pos.y;
					}*/

					if (m_rot.x < 1.57f)
					{//↓Xの角度の計算意味ないかも
						m_rot.x += 0.1f;
						m_rot.z += (float)(rand() % 314 / 100);
						m_rot.y += (float)(rand() % 628 / 100.0f);
					}
					else
					{//落ちきったらXの角度は固定
						m_rot.x = 1.57f;
					}

					if (m_rot.x > 1.57f)
					{//↓Xの角度の計算意味ないかも
						m_rot.x -= 0.1f;
						m_rot.z += (float)(rand() % 314 / 100);
					}
					else
					{//落ちきったらXの角度は固定
						m_rot.x = 1.57f;
					}
				}
				else
				{//空中での回転
					m_moveRot.x += 0.25f;
					m_rot.z += 0.04f;

					m_moveRot.x += (0.0f - m_moveRot.x) * 0.02f;

					m_rot.x = m_moveRot.x;

					//角度の正規化
					if (m_rot.x <= -D3DX_PI)
					{
						m_rot.x += D3DX_PI * 2.0f;
					}
					else if (m_rot.x >= D3DX_PI)
					{
						m_rot.x -= D3DX_PI * 2.0f;
					}

					if (m_rot.z <= -D3DX_PI)
					{
						m_rot.z += D3DX_PI * 2.0f;
					}
					else if (m_rot.z >= D3DX_PI)
					{
						m_rot.z -= D3DX_PI * 2.0f;
					}
				}
			}
		}
	}

	m_move.y -= 0.09f;
	m_pos += m_move;

	//ライフを減らす
	m_nLife--;
	if (m_nLife <= 0)
	{//終了処理
		Uninit();
	}
}

//=============================================================================
//オブジェクトの描画
void CEffectModel::Draw(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	DrawShadow();

	//モデルデータを取得
	CModelManager::MODEL_DATA* pData = CManager::GetModelManager()->GetModelData(m_nIdxModel);

	if (pData == NULL || pData->pMesh == NULL)
	{
		return;
	}

	D3DXMATRIX mtxRot, mtxTrans;					//計算用マトリックス

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
//影の描画
void CEffectModel::DrawShadow(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	float fBias = -0.0001f;
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fBias);

	//光源方向の設定
	D3DXVECTOR4 lightDir(-10.0f, 20.0f, 10.0f, 0.0f);

	//CFieldから平面情報（位置と法線）を取得
	D3DXVECTOR3 planePoint(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 planeNormal(0.0f, 1.0f, 0.0f);

	CField* pField = NULL;			//ゲーム管理クラスからフィールドを取得
	CScene* pScene = CManager::GetScene();

	if (pScene->GetMode() == CScene::MODE_GAME)
	{
		pField = CGame::GetField();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	{
		//pField = CTutorial::GetField();
	}

	if (pField != NULL)
	{
		planePoint = pField->GetPlanePoint();
		planePoint.y += 1.0f;
		planeNormal = pField->GetPlaneNormal();
	}

	//影行列の生成
	D3DXPLANE plane;
	D3DXPlaneFromPointNormal(&plane, &planePoint, &planeNormal);

	//マトリックスシャドウ
	D3DXMATRIX mtxShadow;
	D3DXMatrixShadow(&mtxShadow, &lightDir, &plane);

	//ワールド行列の計算と合成
	D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
	D3DXMatrixIdentity(&mtxWorld);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	mtxWorld = mtxRot * mtxTrans;

	D3DXMATRIX mtxShadowWorld = mtxWorld * mtxShadow;
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadowWorld);

	//チラつき防止設定
	float fSlopeBias = 1.0f;
	float fDepthBias = -0.00001f;
	float fZeroBias = 0.0f;

	pDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *reinterpret_cast<DWORD*>(&fSlopeBias));
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, *reinterpret_cast<DWORD*>(&fDepthBias));

	//減算合成(大事)
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//テクスチャはいらない
	pDevice->SetTexture(0, NULL);

	D3DMATERIAL9 matShadow;
	ZeroMemory(&matShadow, sizeof(matShadow));
	matShadow.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f); //アルファ値0.5で半透明
	pDevice->SetMaterial(&matShadow);

	//メッシュ描画（CModelManager経由）
	CModelManager::MODEL_DATA* pData = CManager::GetModelManager()->GetModelData(m_nIdxModel);
	if (pData != NULL && pData->pMesh != NULL)
	{
		for (int nCntMat = 0; nCntMat < (int)pData->dwNumMat; nCntMat++)
		{
			pData->pMesh->DrawSubset(nCntMat);
		}
	}

	//減算合成をリセット
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//ステートの復元
	pDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *reinterpret_cast<DWORD*>(&fZeroBias));
	pDevice->SetRenderState(D3DRS_DEPTHBIAS, *reinterpret_cast<DWORD*>(&fZeroBias));
}

#if 0
//=============================================================================
//テクスチャの割り当て
void CEffectModel::BindTexture(const LPDIRECT3DTEXTURE9 pTexture)
{
	m_pTexture = pTexture;
}
#endif

//=============================================================================
//ポジション設定
void CEffectModel::SetPosition(const D3DXVECTOR3 pos)
{
	//posを代入
	m_pos = pos;
}