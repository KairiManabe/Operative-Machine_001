//=============================================================================
//
//3Dオブジェクト[object3D.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "object3D.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "input.h"
#include "texture.h"
#include "color.h"

#include "scene.h"
#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "clear.h"
#include "fail.h"
#include "result.h"

#include "player.h"
#include "number.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CObject3D::m_nIdxTexture = 0;
int CObject3D::m_nNumAll = 0;

//=============================================================================
//コンストラクタ
CObject3D::CObject3D(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	m_mtxWorld = {};
	m_pVtxBuff = NULL;
	m_fHeight = 0.0f;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_type = TYPE_NONE;
	m_nNumAll++;
}

//=============================================================================
//デストラクタ
CObject3D::~CObject3D()
{
}

//=============================================================================
//テクスチャの生成
HRESULT CObject3D::Load(void)
{
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->Register("data/TEXTURE/floor000.jpg");

	////レンダラーを取得
	//CRenderer* renderer = CManager::GetRenderer();

	////デバイス取得
	//LPDIRECT3DDEVICE9 D3DDevice = renderer->GetDevice();

	////テクスチャの読み込み
	//D3DXCreateTextureFromFile(D3DDevice,
	//	"data\\TEXTURE\\ski000.jpg",
	//	&m_pTexture);

	return S_OK;
}

//=============================================================================
//テクスチャの破棄
void CObject3D::Unload(void)
{
}

//=============================================================================
//3Dオブジェクトの生成
CObject3D* CObject3D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size)
{
	//プレイヤー
	CObject3D* pObj3D = NULL;

	if (pObj3D == NULL)
	{
		//弾生成
		pObj3D = new CObject3D;

		if (pObj3D != NULL)
		{
			//各種代入
			pObj3D->m_pos = pos;
			pObj3D->m_rot = rot;
			pObj3D->m_size = size;

			pObj3D->Init(pObj3D->m_pos, pObj3D->m_size);
			pObj3D->m_type = TYPE_FLOOR;
			//pObj3D->BindTexture(m_pTexture);
		}
	}
	return pObj3D;
}

//=============================================================================
//オブジェクトの初期化
HRESULT CObject3D::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	/*m_pos = pos;
	m_size = size;*/

	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx;		//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, 0.0f, m_size.z);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, m_size.z);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, m_size.y, -m_size.z);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, 0.0f, -m_size.z);

	//法線ベクトルの設定
	pVtx[0].nor = GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos);
	pVtx[1].nor = (GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos) + GetNor(pVtx[3].pos, pVtx[1].pos, pVtx[2].pos)) / 2.0f;
	pVtx[2].nor = (GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos) + GetNor(pVtx[3].pos, pVtx[1].pos, pVtx[2].pos)) / 2.0f;
	pVtx[3].nor = GetNor(pVtx[3].pos, pVtx[1].pos, pVtx[2].pos);

	//カラー
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(12.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 12.0f);
	pVtx[3].tex = D3DXVECTOR2(12.0f, 12.0f);

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();

	return S_OK;
}

//=============================================================================
//オブジェクトの終了
void CObject3D::Uninit(void)
{
	//頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	m_nNumAll--;

	//破棄
	CObject::Release();
}

//=============================================================================
//オブジェクトの更新
void CObject3D::Update(void)
{
	CScene* pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;

	//プレイヤーの各シーン取得
	if (pScene->GetMode() == CScene::MODE_GAME && pPlayer == NULL)
	{//ゲームシーン
		pPlayer = CGame::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TITLE && pPlayer == NULL)
	{//タイトルシーン
		pPlayer = CTitle::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL && pPlayer == NULL)
	{//チュートリアルシーン
		pPlayer = CTutorial::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_CLEAR && pPlayer == NULL)
	{//クリアシーン
		pPlayer = CClear::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_FAIL && pPlayer == NULL)
	{//failシーン
		pPlayer = CFail::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_RESULT && pPlayer == NULL)
	{//リザルトシーン
		pPlayer = CResult::GetPlayer();
	}

	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	VERTEX_3D* pVtx;		//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, 0.0f, m_size.z);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, m_size.z);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, m_size.y, -m_size.z);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, 0.0f, -m_size.z);

#if 0
	D3DXVECTOR3 vec1 = {};
	D3DXVECTOR3 vec2 = {};
	D3DXVECTOR3 vec3 = {};
	D3DXVECTOR3 vec4 = {};
	D3DXVECTOR3 norA = {};
	D3DXVECTOR3 norB = {};

	vec1 = pVtx[1].pos - pVtx[0].pos;
	vec2 = pVtx[2].pos - pVtx[0].pos;

	vec3 = pVtx[1].pos - pVtx[3].pos;
	vec4 = pVtx[2].pos - pVtx[3].pos;

	//外積
	D3DXVec3Cross(&norA, &vec1, &vec2);
	D3DXVec3Cross(&norB, &vec3, &vec4);

	//正規化
	D3DXVec3Normalize(&norA, &norA);
	D3DXVec3Normalize(&norB, &norB);
#endif

	//法線ベクトルの設定
	pVtx[0].nor = GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos);
	pVtx[1].nor = (GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos) + GetNor(pVtx[3].pos, pVtx[2].pos, pVtx[1].pos)) / 2.0f;
	pVtx[2].nor = (GetNor(pVtx[0].pos, pVtx[1].pos, pVtx[2].pos) + GetNor(pVtx[3].pos, pVtx[2].pos, pVtx[1].pos)) / 2.0f;
	pVtx[3].nor = GetNor(pVtx[3].pos, pVtx[2].pos, pVtx[1].pos);

	CDebugproc* pDebug = CManager::GetDebugproc();

	//for (int nCntVtx = 0; nCntVtx < 4; nCntVtx++)
	//{
	//	pDebug->PrintDebugProc("3Dポリゴンの座標vtx[%d]<%.1f,%.1f,%.1f>\n", nCntVtx, pVtx[nCntVtx].pos.x, pVtx[nCntVtx].pos.y, pVtx[nCntVtx].pos.z);
	//}

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}

//=============================================================================
//オブジェクトの描画
void CObject3D::Draw(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイスの取得
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

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddres(m_nIdxTexture));

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
//当たり判定
bool CObject3D::CollisionPlayer(void)
{
	CScene* pScene = CManager::GetScene();
	D3DXVECTOR3 pos = {};

	CPlayer* pPlayer = NULL;

	//プレイヤーの各シーン取得
	if (pScene->GetMode() == CScene::MODE_GAME && pPlayer == NULL)
	{//ゲームシーン
		pPlayer = CGame::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TITLE && pPlayer == NULL)
	{//タイトルシーン
		pPlayer = CTitle::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL && pPlayer == NULL)
	{//チュートリアルシーン
		pPlayer = CTutorial::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_CLEAR && pPlayer == NULL)
	{//クリアシーン
		pPlayer = CClear::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_FAIL && pPlayer == NULL)
	{//failシーン
		pPlayer = CFail::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_RESULT && pPlayer == NULL)
	{//リザルトシーン
		pPlayer = CResult::GetPlayer();
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
	D3DXVECTOR3 local[4] =
	{
		D3DXVECTOR3(-m_size.x, 0.0f,     m_size.z),
		D3DXVECTOR3(m_size.x, m_size.y, m_size.z),
		D3DXVECTOR3(-m_size.x, m_size.y,-m_size.z),
		D3DXVECTOR3(m_size.x, 0.0f,    -m_size.z)
	};

	D3DXVECTOR3 vtx[4];

	for (int nCntVtx = 0; nCntVtx < 4; nCntVtx++)
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

	return false;
}

//=============================================================================
//テクスチャの割り当て
void CObject3D::BindTexture(const LPDIRECT3DTEXTURE9 pTexture)
{
	//m_pTexture = pTexture;
}

//=============================================================================
//ポジション設定
void CObject3D::SetPosition(const D3DXVECTOR3 pos)
{
	//posを代入
	m_pos = pos;
}

//=============================================================================
//法線の計算取得
D3DXVECTOR3 CObject3D::GetNor(const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
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
float CObject3D::CalcHeight(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
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
bool CObject3D::PointInTriangle(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
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
