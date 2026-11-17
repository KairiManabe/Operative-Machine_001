//=============================================================================
//
// Xオブジェクト[objectX.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "objectX.h"
#include "fade.h"
#include "player.h"
#include "renderer.h"
#include "manager.h"
#include "scene.h"
#include "game.h"
#include "tutorial.h"
#include "title.h"
#include "number.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"
#include "texture.h"
#include "particle.h"
#include "color.h"

//マクロ定義-------------------------------------------------------------------
#define COLLISION_MODEL_W		(15.0f)
#define COLLISION_MODEL_H		(10.0f)

//静的メンバ変数宣言-----------------------------------------------------------
bool CObjectX::m_bWallClim = false;
int CObjectX::m_wallnum = 0;

//=============================================================================
//コンストラクタ
CObjectX::CObjectX(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	memset(&m_nIdxTexture[0], 0, sizeof(m_nIdxTexture));
	m_pMesh = NULL;
	m_pBuffMat = NULL;
	m_dwNumMat = NULL;
	m_mtxWorld = {};
	m_vtxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vtxMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_enemyPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_pFileName = {};
	m_type = TYPE_WALL;
}

//=============================================================================
//デストラクタ
CObjectX::~CObjectX()
{
}

//=============================================================================
//モデルの生成
CObjectX* CObjectX::Create(TYPE type, const std::string& fileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//プレイヤー
	CObjectX* pObjX = NULL;

	if (pObjX == NULL)
	{
		//弾生成
		pObjX = new CObjectX;

		if (pObjX != NULL)
		{
			//各種代入
			pObjX->m_pos = pos;
			pObjX->m_rot = rot;
			pObjX->m_pFileName = fileName;
			pObjX->m_type = type;

			if (type == TYPE_WALL)
			{
				m_wallnum++;
			}

			pObjX->Init(pObjX->m_pos, pObjX->m_size);
		}
	}
	return pObjX;
}

//=============================================================================
//オブジェクトの初期化
HRESULT CObjectX::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	//テクスチャ管理
	CTexture* pTexture = CManager::GetTexture();

	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	int nNumVtx;			//頂点数
	DWORD dwSizeFVF;		//頂点フォーマットのサイズ
	BYTE* pVtxBuff;			//頂点バッファへのポインタ
	D3DXMATERIAL* pMat;
	D3DXVECTOR3 vtxMax, vtxMin;
	//g_MasicStoneNum = 0;
	//FrameCounter = 0;		//フレームのカウンター
	//TimeMemo = 120;
	//TimeAns = 0;

	vtxMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);
	vtxMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);

	//xファイルの読み込み
	D3DXLoadMeshFromX(m_pFileName.c_str(),
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat,
		NULL,
		&m_dwNumMat,
		&m_pMesh);

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		//テクスチャ管理でやるときはテクスチャファイルがなくても登録する(-1で帰ってくる)
		//if (pMat[nCntMat].pTextureFilename != NULL)←がいらなくなる
		m_nIdxTexture[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
	}
	//頂点数を取得
	nNumVtx = m_pMesh->GetNumVertices();

	//頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	//頂点バッファをロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

		if (vtx.x < vtxMin.x)
		{
			vtxMin.x = vtx.x;
		}
		if (vtx.z < vtxMin.z)
		{
			vtxMin.z = vtx.z;
		}
		if (vtx.y < vtxMin.y)
		{
			vtxMin.y = vtx.y;
		}

		if (vtx.x > vtxMax.x)
		{
			vtxMax.x = vtx.x;
		}
		if (vtx.z > vtxMax.z)
		{
			vtxMax.z = vtx.z;
		}
		if (vtx.y > vtxMax.y)
		{
			vtxMax.y = vtx.y;
		}

		pVtxBuff += dwSizeFVF;		//頂点フォーマットのサイズ分ポインタを進める
	}
	m_vtxMax = vtxMax;
	m_vtxMin = vtxMin;

	//アンロック
	m_pMesh->UnlockVertexBuffer();

	return S_OK;
}

//=============================================================================
//オブジェクトの終了
void CObjectX::Uninit(void)
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

	//破棄
	CObject::Release();
}

//=============================================================================
//オブジェクトの更新
void CObjectX::Update(void)
{
	//キーボード入力の取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	//フェードを取得
	CFade* pFade = CManager::GetFade();

	//シーンの取得
	CScene* pScene = CManager::GetScene();

	//カメラの取得
	CCamera* pCamera = CManager::GetCamera();
	
	D3DXVECTOR3 rotCamera = pCamera->GetRotation();

	//オブジェクトの中心座標を計算する
	m_Center.x = m_pos.x + (m_vtxMax.x + m_vtxMin.x) * 0.5f;
	m_Center.z = m_pos.z + (m_vtxMax.z + m_vtxMin.z) * 0.5f;

	if (m_type == TYPE_GOAL && CollisionPlayer() == true && pScene->GetMode() == CScene::MODE_TUTORIAL)
	{//チュートリアルだったらゲームに移行
		pFade->SetFade(CScene::MODE_GAME);
	}

	if (m_type == TYPE_GOAL && CollisionPlayer() == true && pScene->GetMode() == CScene::MODE_GAME)
	{//ゲームだったらクリア
		pFade->SetFade(CScene::MODE_CLEAR);
	}
	else if (m_type == TYPE_WALL && CollisionPlayer() == true)
	{
		if (pScene->GetMode() == CScene::MODE_TUTORIAL && CTutorial::GetUI() == false)
		{//UIをオンにする
			CTutorial::SetUI(true);
		}
	}
	else if (m_type == TYPE_CEILING&& CollisionPlayer() == true)
	{
	}

	//カメラの当たり判定
	CollisionCamera();
}

//=============================================================================
//オブジェクトの描画
void CObjectX::Draw(void)
{
	//レンダラーを取得
	CRenderer* renderer = CManager::GetRenderer();

	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;					//計算用マトリックス
	D3DMATERIAL9 matDef;							//現在のマテリアル保存用
	D3DXMATERIAL* pMat;								//マテリアルデータへのポインタ

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

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		//マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		//テクスチャの設定
		pDevice->SetTexture(0, CManager::GetTexture()->GetAddres(m_nIdxTexture[nCntMat]));	

		//モデル(パーツ)の描画
		m_pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

#if 0
//=============================================================================
//テクスチャの割り当て
void CObjectX::BindTexture(const LPDIRECT3DTEXTURE9 pTexture)
{
	m_pTexture = pTexture;
}
#endif

//=============================================================================
//ポジション設定
void CObjectX::SetPosition(const D3DXVECTOR3 pos)
{
	//posを代入
	m_pos = pos;
}

//=============================================================================
//プレイヤーとの当たり判定	****(斜めの時当たり判定が逆向きになってるかも)****
bool CObjectX::CollisionPlayer(void)
{
	//シーンの取得
	CScene *pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;

	if (pScene->GetMode() == CScene::MODE_GAME)
	{
		// プレイヤーの取得
		pPlayer = CGame::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	{
		// プレイヤーの取得
		pPlayer = CTutorial::GetPlayer();
	}

	if (pPlayer == NULL)
	{//NULLたったらfalseを返す
		return false;
	}

	//プレイヤーの座標
	D3DXVECTOR3 pos = pPlayer->GetPosition();

	//プレイヤーが壁の高さの範囲内にいるか
	float wallMinY = m_pos.y + m_vtxMin.y;
	float wallMaxY = m_pos.y + m_vtxMax.y;
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
	float halfWidthX = (m_vtxMax.x - m_vtxMin.x) * 0.5f;
	float halfWidthZ = (m_vtxMax.z - m_vtxMin.z) * 0.5f;

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

		//壁登り判定
		if (m_type == TYPE_WALL &&
			pPlayer->GetMode() == CPlayer::MODE_SPIDER &&
			bIsMovingTowardsWall)
		{
			//壁登りフラグを立てて、Y軸上昇速度をセット
			m_bWallClim = true;
			pPlayer->ClimbUp(2.0f);

			//地上判定で床にめり込まないよう、位置を少しだけ上にする
			pos.y += 0.5f;
			pPlayer->SetPosition(pos);
		}

		return true;
	}
	return false;
}

//=============================================================================
//敵との当たり判定	(range.cppで)
bool CObjectX::CollisionEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	if (m_type != TYPE_WALL)
	{
		return false;
	}

	//対象物の半径
	float playerRadius = size.x;	//見えているサイズより少し小さく判定

	//オブジェクトの中心を原点とした、対象物の相対座標
	float relativeX = pos.x - m_Center.x;
	float relativeZ = pos.z - m_Center.z;

	//対象物の座標を、オブジェクトの向きの逆に回転させて計算
	float localPlayerX = relativeX * cosf(-m_rot.y) - relativeZ * sinf(-m_rot.y);
	float localPlayerZ = relativeX * sinf(-m_rot.y) + relativeZ * cosf(-m_rot.y);

	//シンプルに計算したいからローカル空間として、最小・最大幅を定義
	float halfWidthX = (m_vtxMax.x - m_vtxMin.x) * 0.5f;
	float halfWidthZ = (m_vtxMax.z - m_vtxMin.z) * 0.5f;

	//わかりやすく変換。オブジェクトのAABB範囲
	float minX = -halfWidthX;
	float maxX = halfWidthX;
	float minZ = -halfWidthZ;
	float maxZ = halfWidthZ;

	//四角形上で、対象物に一番近い「最近傍点」を求める
	float closestX = max(minX, min(localPlayerX, maxX));
	float closestZ = max(minZ, min(localPlayerZ, maxZ));

	//対象物の最近傍点の距離を計算する
	float diffX = localPlayerX - closestX;
	float diffZ = localPlayerZ - closestZ;
	float distanceSq = (diffX * diffX) + (diffZ * diffZ);

	//当たり判定
	if (distanceSq <= (playerRadius * playerRadius))
	{
		float distance = sqrtf(distanceSq);

		if (distance == 0.0f)
		{
			return false;
		}

		//押し戻しベクトルの計算
		float dirX = diffX / distance;
		float dirZ = diffZ / distance;

		//めり込んでいる長さ
		float overlap = playerRadius - distance;

		//対象物の位置をめり込んだ分だけ押し戻す
		localPlayerX += dirX * overlap;
		localPlayerZ += dirZ * overlap;

		//補正した座標を逆翻訳して戻す
		float origCos = cosf(m_rot.y);
		float origSin = sinf(m_rot.y);

		//本来の向きに回転を戻す
		float worldCorrectedX = localPlayerX * origCos - localPlayerZ * origSin;
		float worldCorrectedZ = localPlayerX * origSin + localPlayerZ * origCos;

		//オブジェクトの中心座標を足して、最終的なプレイヤーの座標を決定
		pos.x = m_Center.x + worldCorrectedX;
		pos.z = m_Center.z + worldCorrectedZ;

		m_enemyPos = pos;

		return true;
	}
	return false;
}

//=============================================================================
//カメラとの当たり判定	(プレイヤーの判定とほぼ同じ)
bool CObjectX::CollisionCamera(void)
{
	if (m_type == TYPE_CEILING)
	{
		return false;
	}

	//カメラの取得posの取得
	CCamera* pCamera = CManager::GetCamera();
	D3DXVECTOR3 pos = pCamera->GetPositionV();

	//カメラの半径
	float cameraRadius = 20.0f;

	//オブジェクトの中心を原点とした、カメラの相対座標(relative→相対座標の英語)
	float relativeX = pos.x - m_Center.x;
	float relativeZ = pos.z - m_Center.z;

	//カメラの座標を、オブジェクトの向き(m_rot.y)の逆に回転させて計算
	float localCameraX = relativeX * cosf(-m_rot.y) - relativeZ * sinf(-m_rot.y);
	float localCameraZ = relativeX * sinf(-m_rot.y) + relativeZ * cosf(-m_rot.y);

	//シンプルに計算したいからローカル空間（回転していないもの）として、最小・最大幅を定義
	float halfWidthX = (m_vtxMax.x - m_vtxMin.x) * 0.5f;
	float halfWidthZ = (m_vtxMax.z - m_vtxMin.z) * 0.5f;

	//わかりやすく変換。オブジェクトのAABB範囲
	float minX = -halfWidthX;
	float maxX = halfWidthX;
	float minZ = -halfWidthZ;
	float maxZ = halfWidthZ;

	//四角形上で、カメラに一番近い「最近傍点」を求める
	float closestX = max(minX, min(localCameraX, maxX));
	float closestZ = max(minZ, min(localCameraZ, maxZ));

	//カメラと最近傍点の距離を計算する
	float diffX = localCameraX - closestX;
	float diffZ = localCameraZ - closestZ;
	float distanceSq = (diffX * diffX) + (diffZ * diffZ);

	//当たり判定とローカル空間での押し戻し
	if (distanceSq <= (cameraRadius * cameraRadius))
	{
		float distance = sqrtf(distanceSq);

		if (distance == 0.0f)
		{//distanceが0
			return false;
		}

		//めり込んでいる長さ
		float overlap = cameraRadius - distance;

		//ローカル空間側で、カメラの位置をめり込んだ分だけ押し戻す(押し戻しベクトルの計算（ローカル空間))
		localCameraX += (diffX / distance) * overlap;
		localCameraZ += (diffZ / distance) * overlap;

		//補正したローカル座標を、元のワールド空間（回転した世界）に逆翻訳して戻す
		float origCos = cosf(m_rot.y);
		float origSin = sinf(m_rot.y);

		//本来の向き（m_rot.y）に回転を戻す
		float worldCorrectedX = localCameraX * origCos - localCameraZ * origSin;
		float worldCorrectedZ = localCameraX * origSin + localCameraZ * origCos;

		//オブジェクトの中心座標を足して、最終的なカメラのワールド座標を決定
		pos.x = m_Center.x + worldCorrectedX;
		pos.z = m_Center.z + worldCorrectedZ;

		//座標をカメラに渡す
		pCamera->SetPositionV(pos);

		return true;
	}
	return false;
}