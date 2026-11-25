//=============================================================================
//
// 敵の作的範囲など[range.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "range.h"

#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "fade.h"
#include "color.h"

#include "scene.h"
#include "game.h"
#include "fail.h"

#include "player.h"
#include "objectX.h"
#include "particle.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CRange::m_nNumAll = 0;
int CRange::m_nIdxTexture[CTarget::TYPE_MAX] = { -1 };

//=============================================================================
//コンストラクタ
CRange::CRange(int nPriority) :CObject(nPriority)
{
	m_pVtxBuff = NULL;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	//m_Type = CEnemy::TYPE_NONE;
	m_type = CTarget::TYPE_NONE;

	m_nTurnCounter = 0;
	m_bTurn = false;
	m_mtxWorld = {};
	nBoom = 0;
	m_nNumAll++;
}

//=============================================================================
// デストラクタ
CRange::~CRange()
{
}

//=============================================================================
//テクスチャの生成
HRESULT CRange::Load(void)
{
	//テクスチャインデックスの取得
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture[CTarget::TYPE_RANGE] = pTexture->Register("data/TEXTURE/range000.png");
	m_nIdxTexture[CTarget::TYPE_FAN] = pTexture->Register("data/TEXTURE/fan_shape.png");

	return S_OK;
}

//=============================================================================
//テクスチャの破棄
void CRange::Unload(void)
{
}

//=============================================================================
//生成処理
CRange* CRange::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXVECTOR3 rot, const CTarget::TYPE type)
{
	//生成
	CRange* pRange = new CRange;

	if (pRange != NULL)
	{
		//各種代入
		pRange->SetRotation(rot);
		pRange->SetType(type);

		//初期化処理
		pRange->Init(pos, size);
	}

	return pRange;
}

//=============================================================================
//初期化処理
HRESULT CRange::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
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

	//頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, 0.0f, m_size.z);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, m_size.z);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, m_size.y, -m_size.z);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, 0.0f, -m_size.z);

	//法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	//カラー
	pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);

	//テクスチャ座標
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();

	return S_OK;
}

//=============================================================================
//終了処理
void CRange::Uninit(void)
{
	//頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	//総数を減らす
	m_nNumAll--;

	//破棄
	CObject::Release();
}

//=============================================================================
//更新処理
void CRange::Update(void)
{
	//objectXのwallを取得
	CObjectX** pWall = CGame::GetObjectXWall();

	//前回の位置を保存
	m_posOld = m_pos;

	//壁との判定処理
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		// TYPE_FAN の位置は親ターゲットが管理するため、壁との押し戻しをしない。
		if (m_type == CTarget::TYPE_FAN || pWall[nCntWall] == NULL)
		{
			continue;
		}

		//当たり判定チェック
		if (pWall[nCntWall]->CollisionEnemy(m_pos, m_size) == true && pWall[nCntWall]->GetType() != CObjectX::TYPE_CEILING)
		{
			//当たり判定が起きている間は、常に壁の外へ押し戻す
			m_pos = pWall[nCntWall]->GetEnemyPos();

#if 0
			//まだクールダウン中でなければ方向転換を行う
			if (m_bTurn == false)
			{
				m_bTurn = true;

				if (m_Type == CEnemy::TYPE_STRAIGHT)
				{
					//180度反転
					m_rot.y += D3DX_PI;
				}
				else if (m_Type == CEnemy::TYPE_AROUND_RIGHT)
				{
					//右に90度回転 (+90度)
					m_rot.y += (D3DX_PI * 0.5f);
				}
				else if (m_Type == CEnemy::TYPE_AROUND_LEFT)
				{
					//左に90度回転 (-90度)
					m_rot.y -= (D3DX_PI * 0.5f);
				}

				//角度の正規化 (-π ～ +π の範囲に確実に納める)
				while (m_rot.y > D3DX_PI)  m_rot.y -= (D3DX_PI * 2.0f);
				while (m_rot.y < -D3DX_PI) m_rot.y += (D3DX_PI * 2.0f);
			}

#endif
		}
	}

	//クールダウンタイマー更新
	if (m_bTurn == true)
	{
		m_nTurnCounter++;

		if (m_nTurnCounter >= 30)
		{
			m_bTurn = false;
			m_nTurnCounter = 0;
		}
	}

	//シーン・フェードおよびプレイヤーとの判定
	CScene* pScene = CManager::GetScene();
	CFade* pFade = CManager::GetFade();

	if (pScene != NULL && pScene->GetMode() == CScene::MODE_GAME)
	{
		CPlayer* pPlayer = CGame::GetPlayer();

		if (pPlayer != NULL)
		{
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();

			float fDispX = playerPos.x - m_pos.x;
			float fDispZ = playerPos.z - m_pos.z;
			float fDistSq = (fDispX * fDispX) + (fDispZ * fDispZ);
			bool bPlayerInRange = (fDistSq <= (m_size.x * m_size.z));

			if (m_type == CTarget::TYPE_FAN)
			{
				// TYPE_FAN はターゲット側の衝突判定と同じ長さ・前方方向で判定する。
				const float fFanLengthSq = m_size.x * m_size.x;
				bPlayerInRange = (fDistSq <= fFanLengthSq);

				if (bPlayerInRange && fDistSq > 0.000001f)
				{
					const float fDistance = sqrtf(fDistSq);
					const float fForwardX = -sinf(m_rot.y);
					const float fForwardZ = -cosf(m_rot.y);
					const float fDot = fForwardX * fDispX + fForwardZ * fDispZ;
					const float fCosHalfAngle = cosf(D3DXToRadian(TARGET_FAN_ANGLE_DEGREES * 0.5f));
					bPlayerInRange = (fDot >= fDistance * fCosHalfAngle);
				}
			}

			if (bPlayerInRange && playerPos.y <= 50.0f)
			{
				pFade->SetFade(CScene::MODE_FAIL);
			}
		}
	}
}

//=============================================================================
//描画処理
void CRange::Draw(void)
{
	//デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//アルファブレンドにする場合の設定
	//pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DXMATRIX mtxRot, mtxTrans;		//計算用マトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	float rotationY = m_rot.y;
	if (m_type == CTarget::TYPE_FAN)
	{
	 rotationY += D3DX_PI;
	}

	D3DXMatrixRotationYawPitchRoll(&mtxRot,
	 rotationY, m_rot.x, m_rot.z);

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
	pDevice->SetTexture(0, CManager::GetTexture()->GetAddres(m_nIdxTexture[m_type]));

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

//=============================================================================
//サイズの変更
void CRange::SetSize(const D3DXVECTOR3 size)
{
	m_size = size;

	VERTEX_3D* pVtx;		//頂点情報へのポインタ

	 //頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標
	pVtx[0].pos = D3DXVECTOR3(-m_size.x, 0.0f, m_size.z);
	pVtx[1].pos = D3DXVECTOR3(m_size.x, m_size.y, m_size.z);
	pVtx[2].pos = D3DXVECTOR3(-m_size.x, m_size.y, -m_size.z);
	pVtx[3].pos = D3DXVECTOR3(m_size.x, 0.0f, -m_size.z);
	
	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}

#if 0
//=============================================================================
// 壁との当たり判定処理
bool CRange::CollisionWall(void)
{
	// 全オブジェクトの中から TYPE_WALL の CObjectX を探して判定する
	for (int nCntObj = 0; nCntObj < CManager::GetNumObject(); nCntObj++)
	{
		CObject* pObj = CManager::GetObject(nCntObj);
		if (pObj == NULL) continue;

		CObjectX* pObjX = dynamic_cast<CObjectX*>(pObj);
		if (pObjX != NULL && pObjX->GetType() == CObjectX::TYPE_WALL)
		{
			// ObjectXのAABBおよび配置情報を取得
			D3DXVECTOR3 vtxMin = pObjX->GetMin();
			D3DXVECTOR3 vtxMax = pObjX->GetMax();
			D3DXVECTOR3 objCenter = pObjX->GetCenter();
			D3DXVECTOR3 objRot = pObjX->GetRotation();

			// CRange の判定半径
			float rangeRadius = m_size.x;

			// 壁の中心からの相対座標
			float relativeX = m_pos.x - objCenter.x;
			float relativeZ = m_pos.z - objCenter.z;

			// 壁の回転の逆向きに座標を回転（ローカル座標系へ変換）
			float localRangeX = relativeX * cosf(-objRot.y) - relativeZ * sinf(-objRot.y);
			float localRangeZ = relativeX * sinf(-objRot.y) + relativeZ * cosf(-objRot.y);

			// AABBの幅の半分
			float halfWidthX = (vtxMax.x - vtxMin.x) * 0.5f;
			float halfWidthZ = (vtxMax.z - vtxMin.z) * 0.5f;

			// 四角形上で最も近い点を計算
			float closestX = max(-halfWidthX, min(localRangeX, halfWidthX));
			float closestZ = max(-halfWidthZ, min(localRangeZ, halfWidthZ));

			// 最近傍点からの距離の二乗
			float diffX = localRangeX - closestX;
			float diffZ = localRangeZ - closestZ;
			float distanceSq = (diffX * diffX) + (diffZ * diffZ);

			// 接触判定
			if (distanceSq <= (rangeRadius * rangeRadius))
			{
				return true; // 壁に衝突
			}
		}
	}
	return false;
}
#endif