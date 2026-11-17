//=============================================================================
//
// 3Dオブジェクト壁[wall.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "wall.h"

#include "manager.h"   
#include "renderer.h"
#include "debugproc.h"
#include "input.h"
#include "color.h"
#include "sound.h"
#include "texture.h"

#include "tutorial.h"
#include "game.h"

#include "player.h"
#include "particle.h"
#include "number.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CWall::m_nIdxTexture = 0;
int CWall::m_nNumAll = 0;

//=============================================================================
//コンストラクタ
CWall::CWall(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	m_mtxWorld = {};
	m_pVtxBuff = NULL;
	m_PushOut = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_MaxPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_MinPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_nNumAll++;
}

//=============================================================================
//デストラクタ
CWall::~CWall()
{
}

//=============================================================================
//3Dオブジェクトの生成
CWall* CWall::Create(std::string fileName, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot)
{
	//プレイヤー
	CWall* pWall = NULL;

	if (pWall == NULL)
	{
		//弾生成
		pWall = new CWall;

		if (pWall != NULL)
		{
			//各種代入
			pWall->SetRotation(rot);

			CTexture* pTexture = CManager::GetTexture();
			m_nIdxTexture = pTexture->Register(fileName.c_str());

			pWall->Init(pos, size);
			//pWall->BindTexture(m_pTexture);
		}
	}
	return pWall;
}

//=============================================================================
//オブジェクトの初期化
HRESULT CWall::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_pos = pos;
	m_size = size;

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
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, 0.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, 0.0f, 0.0f);

	//法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

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

	float fThinZ = 5.0f; // 壁の厚み（Z方向）
	m_MinPos = D3DXVECTOR3(-m_size.x, 0.0f, -fThinZ);
	m_MaxPos = D3DXVECTOR3(m_size.x, m_size.y, fThinZ);

	return S_OK;
}

//=============================================================================
//オブジェクトの終了
void CWall::Uninit(void)
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
void CWall::Update(void)
{
	//プレイヤーの取得
	CScene* pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;

	if (pScene->GetMode() == CScene::MODE_GAME)
	{
		pPlayer = CGame::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	{
		pPlayer = CTutorial::GetPlayer();
	}

	//キーボードの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	VERTEX_3D* pVtx;		//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, m_size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, 0.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, 0.0f, 0.0f);

	//法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	CDebugproc* pDebug = CManager::GetDebugproc();

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();

	//中心座標を計算する
	m_Center.x = m_pos.x + (m_MaxPos.x + m_MinPos.x) * 0.5f;
	m_Center.z = m_pos.z + (m_MaxPos.z + m_MinPos.z) * 0.5f;

	if (CollisionPlayer() == true)
	{//壁に当たっていたら
	}
}

//=============================================================================
//オブジェクトの描画
void CWall::Draw(void)
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

	//掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);

	//掛け合わせる
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
//プレイヤーとの当たり判定	****(斜めの時当たり判定が逆向きになってるかも)****
bool CWall::CollisionPlayer(void)
{
	//プレイヤーの取得
	CScene* pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;

	if (pScene->GetMode() == CScene::MODE_GAME)
	{
		pPlayer = CGame::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	{
		pPlayer = CTutorial::GetPlayer();
	}

	if (pPlayer == NULL)
	{//NULLたったらfalseを返す
		return false;
	}

	//プレイヤーの座標
	D3DXVECTOR3 pos = pPlayer->GetPosition();

	//プレイヤーが壁の高さの範囲内にいるか
	float wallMinY = m_pos.y + m_MinPos.y;
	float wallMaxY = m_pos.y + m_MaxPos.y;
	if (pos.y < wallMinY || pos.y > wallMaxY)
	{
		return false;
	}

	//プレイヤーの半径
	float playerRadius = pPlayer->GetCollsionSize();

	//オブジェクトの中心を原点とした、プレイヤーの相対座標
	float relativeX = pos.x - m_Center.x;
	float relativeZ = pos.z - m_Center.z;

	//プレイヤーの座標を、オブジェクトの向きの逆に回転させて計算
	float localPlayerX = relativeX * cosf(-m_rot.y) - relativeZ * sinf(-m_rot.y);
	float localPlayerZ = relativeX * sinf(-m_rot.y) + relativeZ * cosf(-m_rot.y);

	//最小・最大幅
	float halfWidthX = (m_MaxPos.x - m_MinPos.x) * 0.5f;
	float halfWidthZ = (m_MaxPos.z - m_MinPos.z) * 0.5f;

	float minX = -halfWidthX;
	float maxX = halfWidthX;
	float minZ = -halfWidthZ;
	float maxZ = halfWidthZ;

	//四角形上で、プレイヤーに一番近い「最近傍点」を求める
	float closestX = max(minX, min(localPlayerX, maxX));
	float closestZ = max(minZ, min(localPlayerZ, maxZ));

	//プレイヤーと最近傍点の距離を計算する
	float diffX = localPlayerX - closestX;
	float diffZ = localPlayerZ - closestZ;
	float distanceSq = (diffX * diffX) + (diffZ * diffZ);

	//当たり判定
	if (distanceSq <= (playerRadius * playerRadius))
	{
		float distance = sqrtf(distanceSq);

		if (distance == 0.0f)
		{
			diffX = 0.0f;
			diffZ = 1.0f;
			distance = 1.0f;
		}

		//押し戻しベクトルの計算
		float dirX = diffX / distance;
		float dirZ = diffZ / distance;

		//めり込んでいる長さ
		float overlap = playerRadius - distance;

		//-----------------------------------------------------------------
		//壁に向かって進んでいるかチェック
		D3DXVECTOR3 playerMove = pPlayer->GetMove();

		//XZ平面(水平方向)での移動量
		float moveSpeedXZSq = playerMove.x * playerMove.x + playerMove.z * playerMove.z;
		bool bIsMovingTowardsWall = false;

		//水平方向に少しでも移動していれば判定を行う
		if (moveSpeedXZSq > 0.0001f)
		{
			//プレイヤーの移動ベクトルを壁のローカル空間に変換
			float localMoveX = playerMove.x * cosf(-m_rot.y) - playerMove.z * sinf(-m_rot.y);
			float localMoveZ = playerMove.x * sinf(-m_rot.y) + playerMove.z * cosf(-m_rot.y);

			//壁の最近傍点に向かうベクトルと移動ベクトルの内積
			float dot = (diffX * localMoveX) + (diffZ * localMoveZ);

			//内積がマイナス→壁に向かって進んでいる
			if (dot < 0.0f)
			{
				bIsMovingTowardsWall = true;
			}
		}

		//壁の表面(XZ軸)への押し戻し
		localPlayerX += dirX * overlap;
		localPlayerZ += dirZ * overlap;

		float origCos = cosf(m_rot.y);
		float origSin = sinf(m_rot.y);

		float worldCorrectedX = localPlayerX * origCos - localPlayerZ * origSin;
		float worldCorrectedZ = localPlayerX * origSin + localPlayerZ * origCos;

		pos.x = m_Center.x + worldCorrectedX;
		pos.z = m_Center.z + worldCorrectedZ;

		//押し戻した位置を確定
		pPlayer->SetPosition(pos);

		return true;
	}
	return false;
}

//=============================================================================
//ポジション設定
void CWall::SetPosition(const D3DXVECTOR3 pos)
{
	//posを代入
	m_pos = pos;
}

//=============================================================================
//法線の計算取得
D3DXVECTOR3 CWall::GetNor(const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
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
D3DXVECTOR3 CWall::CalcPush(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
{
	D3DXVECTOR3 vec1 = B - A;
	D3DXVECTOR3 vec2 = C - A;
	D3DXVECTOR3 nor;

	//２つのベクトルから法線を求める
	D3DXVec3Cross(&nor, &vec1, &vec2);

	//正規化
	D3DXVec3Normalize(&nor, &nor);

	D3DXVECTOR3 pushout;

	//押し出し座標を求める
	pushout.x = ((player.x - A.x) * nor.x + (player.z - A.z) * nor.z + A.y) / nor.y * -1;
	pushout.z = ((player.x - A.x) * nor.x + (player.z - A.z) * nor.z + A.y) / nor.y * -1;

	return pushout;
}

//=============================================================================
//三角形の判定[A:求めたい法線のvtx]
bool CWall::PointInTriangle(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C)
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
