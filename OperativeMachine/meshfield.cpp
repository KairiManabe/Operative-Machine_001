//=============================================================================
// 
// メッシュフィールド[meshfield.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "meshfield.h"
#include "manager.h"
#include "scene.h"
#include "game.h"
#include "title.h"
#include "tutorial.h"
#include "clear.h"
#include "color.h"
#include "renderer.h"
#include "player.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CMeshField::m_nIdxTexture = 0;

//=============================================================================
//コンストラクタ
CMeshField::CMeshField(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	m_pVtxBuff = NULL;
	m_fHeight = 0.0f;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
}

//=============================================================================
//デストラクタ
CMeshField::~CMeshField()
{
}

//=============================================================================
//テクスチャの生成
HRESULT CMeshField::Load(void)
{
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/renga000.jpg");

	return S_OK;
}

//=============================================================================
//テクスチャの破棄
void CMeshField::Unload(void)
{
}

//=============================================================================
//3Dオブジェクトの生成
CMeshField* CMeshField::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, int nDivisionDepth, int nDivisionWidth)
{
	CMeshField* pMeshField = NULL;

	if (pMeshField == NULL)
	{
		//弾生成
		pMeshField = new CMeshField;

		if (pMeshField != NULL)
		{
			//各種代入
			pMeshField->m_pos = pos;
			pMeshField->m_size = size;

			pMeshField->Init(pMeshField->m_pos, pMeshField->m_size);
			//pObj3D->BindTexture(m_pTexture);
		}
	}
	return pMeshField;
}

//=============================================================================
//プレイヤーの初期化処理
HRESULT CMeshField::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_pos = pos;
	m_size = size;

	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 9,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx;		//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**) &pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, 0.0f, m_size.z);
	pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, m_size.z);
	pVtx[2].pos = D3DXVECTOR3(m_size.x, 0.0f, m_size.z);

	pVtx[3].pos = D3DXVECTOR3(-m_size.x, 0.0f, 0.0f);
	pVtx[4].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(m_size.x, 0.0f, 0.0f);

	pVtx[6].pos = D3DXVECTOR3(-m_size.x, 0.0f, -m_size.z);
	pVtx[7].pos = D3DXVECTOR3(0.0f, 0.0f,-m_size.z);
	pVtx[8].pos = D3DXVECTOR3(m_size.x, 0.0f, -m_size.z);

	for (int nCntMeshField = 0; nCntMeshField < 9; nCntMeshField++)
	{
		//法線ベクトルの設定
		pVtx[nCntMeshField].nor = GetNor(pVtx[nCntMeshField].pos, pVtx[nCntMeshField + 1].pos, pVtx[nCntMeshField + 2].pos);

		//カラー
		pVtx[nCntMeshField].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	}

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(2.0f, 0.0f);
	pVtx[3].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[4].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVtx[5].tex = D3DXVECTOR2(2.0f, 1.0f);
	pVtx[6].tex = D3DXVECTOR2(0.0f, 2.0f);
	pVtx[7].tex = D3DXVECTOR2(1.0f, 2.0f);
	pVtx[8].tex = D3DXVECTOR2(2.0f, 2.0f);

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();

	//インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * 14,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	WORD* pIdx;

	//インデックスバッファをロックし、頂点データの設定
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	pIdx[0] = 3;
	pIdx[1] = 0;
	pIdx[2] = 4;
	pIdx[3] = 1;
	pIdx[4] = 5;
	pIdx[5] = 2;
	pIdx[6] = 2;
	pIdx[7] = 6;
	pIdx[8] = 6;
	pIdx[9] = 3;
	pIdx[10] = 7;
	pIdx[11] = 4;
	pIdx[12] = 8;
	pIdx[13] = 5;

	//インデックスバッファのアンロック
	m_pIdxBuff->Unlock();

return S_OK;
}

//=============================================================================
//プレイヤーの描画
void CMeshField::Draw(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	CTexture* pTexture = CManager::GetTexture();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;		//計算用マトリックス

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

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	//インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddres(m_nIdxTexture));

	//ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
		0,
		0,
		9,
		0,
		12);
}

//=============================================================================
//プレイヤーの終了処理
void CMeshField::Uninit(void)
{
	//頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	//インデックスバッファの破棄
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}

	//破棄
	CObject::Release();
}

//=============================================================================
//プレイヤーの更新処理
void CMeshField::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_T) == true)
	{
		m_size.y += 1.0f;
	}
	if (pInputKeyboard->GetPress(DIK_G) == true)
	{
		m_size.y -= 1.0f;
	}

	if (pInputKeyboard->GetPress(DIK_U) == true)
	{
		m_rot.y += 0.05f;

		//修正
		if (m_rot.y <= -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2;
		}
		else if (m_rot.y >= D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2;
		}
	}
	if (pInputKeyboard->GetPress(DIK_I) == true)
	{
		m_rot.y -= 0.05f;

		//修正
		if (m_rot.y <= -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2;
		}
		else if (m_rot.y >= D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2;
		}
	}

	VERTEX_3D* pVtx;		//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, 0.0f, m_size.z);
	pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, m_size.z);
	pVtx[2].pos = D3DXVECTOR3(m_size.x, 0.0f, m_size.z);

	pVtx[3].pos = D3DXVECTOR3(-m_size.x, 0.0f, 0.0f);
	pVtx[4].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(m_size.x, 0.0f, 0.0f);

	pVtx[6].pos = D3DXVECTOR3(-m_size.x, 0.0f, -m_size.z);
	pVtx[7].pos = D3DXVECTOR3(0.0f, 0.0f, -m_size.z);
	pVtx[8].pos = D3DXVECTOR3(m_size.x, 0.0f, -m_size.z);

	//法線ベクトルの設定
	pVtx[0].nor = GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos);
	pVtx[1].nor = (GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos) + GetNor(pVtx[3].pos, pVtx[2].pos, pVtx[1].pos)) / 2.0f;
	pVtx[2].nor = (GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos) + GetNor(pVtx[3].pos, pVtx[2].pos, pVtx[1].pos)) / 2.0f;

	pVtx[3].nor = (GetNor(pVtx[3].pos, pVtx[2].pos, pVtx[1].pos) + GetNor(pVtx[3].pos, pVtx[4].pos, pVtx[5].pos)) / 2.0f;
	pVtx[4].nor = (GetNor(pVtx[3].pos, pVtx[4].pos, pVtx[5].pos) + GetNor(pVtx[6].pos, pVtx[5].pos, pVtx[4].pos)) / 2.0f;
	pVtx[5].nor = (GetNor(pVtx[3].pos, pVtx[4].pos, pVtx[5].pos) + GetNor(pVtx[6].pos, pVtx[5].pos, pVtx[4].pos)) / 2.0f;

	pVtx[6].nor = (GetNor(pVtx[6].pos, pVtx[5].pos, pVtx[4].pos) + GetNor(pVtx[6].pos, pVtx[7].pos, pVtx[8].pos)) / 2.0f;
	pVtx[7].nor = GetNor(pVtx[3].pos, pVtx[2].pos, pVtx[1].pos);
	pVtx[8].nor = GetNor(pVtx[3].pos, pVtx[2].pos, pVtx[1].pos);

	CDebugproc* pDebug = CManager::GetDebugproc();

	for (int nCntVtx = 0; nCntVtx < 4; nCntVtx++)
	{
		pDebug->PrintDebugProc("3Dポリゴンの座標vtx[%d]<X:%.1f , Y:%.1f , Z:%.1f>\n", nCntVtx, pVtx[nCntVtx].pos.x, pVtx[nCntVtx].pos.y, pVtx[nCntVtx].pos.z);
	}

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
//当たり判定
bool CMeshField::CollisionPlayer(void)
{
	CScene* pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;
	D3DXVECTOR3 pos = {};

	if (pScene->GetMode() == CScene::MODE_GAME)
	{
		pPlayer = CGame::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TITLE)
	{
		pPlayer = CTitle::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL && pPlayer == NULL)
	{
		pPlayer = CTutorial::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_CLEAR)
	{
		pPlayer = CClear::GetPlayer();
	}

	if (pPlayer != NULL)
	{
		pos = pPlayer->GetPosition();
	}

	D3DXMATRIX mtxRot, mtxTrans;		//計算用マトリックス

	D3DXMatrixIdentity(&m_mtxWorld);

	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_rot.y, m_rot.x, m_rot.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ローカル座標の頂点
	D3DXVECTOR3 local[9] =
	{
		D3DXVECTOR3(-m_size.x, 0.0f, m_size.z),
		D3DXVECTOR3(0.0f, 0.0f, m_size.z),
		D3DXVECTOR3(m_size.x, 0.0f, m_size.z),

		D3DXVECTOR3(-m_size.x, 0.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3(m_size.x, 0.0f, 0.0f),

		D3DXVECTOR3(-m_size.x, 0.0f, -m_size.z),
		D3DXVECTOR3(0.0f, 0.0f,-m_size.z),
		D3DXVECTOR3(m_size.x, 0.0f, -m_size.z)
	};

	D3DXVECTOR3 vtx[9];

	for (int nCntVtx = 0; nCntVtx < 9; nCntVtx++)
	{//ワールド座標に変換
		D3DXVec3TransformCoord(&vtx[nCntVtx], &local[nCntVtx], &m_mtxWorld);
	}

	// 三角形 A（0,1,2）に点があるか
	if (PointInTriangle(pos, vtx[0], vtx[1], vtx[2]))
	{
		m_fHeight = CalcHeight(pos, vtx[0], vtx[1], vtx[2]);
		return true;
	}

	// 三角形 B（1,3,2）に点があるか
	if (PointInTriangle(pos, vtx[3], vtx[2], vtx[1]))
	{
		m_fHeight = CalcHeight(pos, vtx[3], vtx[2], vtx[1]);
		return true;
	}

	// 三角形 B（1,3,2）に点があるか
	if (PointInTriangle(pos, vtx[3], vtx[4], vtx[5]))
	{
		m_fHeight = CalcHeight(pos, vtx[3], vtx[4], vtx[5]);
		return true;
	}

	// 三角形 B（1,3,2）に点があるか
	if (PointInTriangle(pos, vtx[6], vtx[5], vtx[4]))
	{
		m_fHeight = CalcHeight(pos, vtx[6], vtx[5], vtx[4]);
		return true;
	}

	// 三角形 B（1,3,2）に点があるか
	if (PointInTriangle(pos, vtx[6], vtx[7], vtx[8]))
	{
		m_fHeight = CalcHeight(pos, vtx[6], vtx[7], vtx[8]);
		return true;
	}

	return false;
}

//=============================================================================
//テクスチャの割り当て
void CMeshField::BindTexture(const LPDIRECT3DTEXTURE9 pTexture)
{
	//m_pTexture = pTexture;
}

//=============================================================================
//ポジション設定
void CMeshField::SetPosition(const D3DXVECTOR3 pos)
{
	//posを代入
	m_pos = pos;
}

//=============================================================================
//法線の計算取得
D3DXVECTOR3 CMeshField::GetNor(const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
{
	D3DXVECTOR3 vec1 = B - A;
	D3DXVECTOR3 vec2 = C - A;
	D3DXVECTOR3 nor;

	//２つのベクトルから法線を求める
	D3DXVec3Cross(&nor, &vec1, &vec2);

	//正規化
	D3DXVec3Normalize(&nor, &nor);

	return nor;
}

//=============================================================================
//高さを算出する処理[A:求めたい法線のvtx]
float CMeshField::CalcHeight(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
{
	D3DXVECTOR3 vec1 = B - A;
	D3DXVECTOR3 vec2 = C - A;
	D3DXVECTOR3 nor;

	//２つのベクトルから法線を求める
	D3DXVec3Cross(&nor, &vec1, &vec2);

	//正規化
	D3DXVec3Normalize(&nor, &nor);

	//高さを求める
	float height = ((player.x - A.x) * nor.x + (player.z - A.z) * nor.z + A.y) / nor.y * -1;

	return height;
}

//=============================================================================
//三角形の判定[A:求めたい法線のvtx]
bool CMeshField::PointInTriangle(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
{//プレイヤーを点Pとして三角形の内側にあるか
	D3DXVECTOR3 vec0 = C - A;
	D3DXVECTOR3 vec1 = B - A;
	D3DXVECTOR3 vec2 = player - A;		//プレイヤーの点とのベクトル

	//内積を求める
	float dot00 = D3DXVec3Dot(&vec0, &vec0);
	float dot01 = D3DXVec3Dot(&vec0, &vec1);
	float dot02 = D3DXVec3Dot(&vec0, &vec2);
	float dot11 = D3DXVec3Dot(&vec1, &vec1);
	float dot12 = D3DXVec3Dot(&vec1, &vec2);

	//重心座標を計算
	float fDenominator = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float fTriangleA = (dot11 * dot02 - dot01 * dot12) * fDenominator;
	float fTriangleB = (dot00 * dot12 - dot01 * dot02) * fDenominator;

	//条件式で返す
	return (fTriangleA >= 0) && (fTriangleB >= 0) && (fTriangleA + fTriangleB <= 1);
}
