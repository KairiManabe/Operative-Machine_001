//=============================================================================
//
// ターゲット[target.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "target.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "ModelManager.h"
#include "input.h"
#include "sound.h"
#include "color.h"
#include "texture.h"

#include "tutorial.h"
#include "game.h"

#include "camera.h"
#include "field.h"
#include "number.h"
#include "player.h"
#include "particle.h"
#include "vigilance.h"

//マクロ定義-------------------------------------------------------------------
#define COLLISION_MODEL_W		(15.0f)
#define COLLISION_MODEL_H		(10.0f)

//静的メンバ変数宣言-----------------------------------------------------------
int CTarget::m_wallnum = 0;

//=============================================================================
//コンストラクタ
CTarget::CTarget(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	memset(&m_nIdxTexture[0], 0, sizeof(m_nIdxTexture));
	m_pMesh = NULL;
	m_pBuffMat = NULL;
	m_dwNumMat = NULL;
	memset(m_mtxWorld, NULL, sizeof(m_mtxWorld));
	m_vtxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vtxMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_enemyPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_pFileName = "";
	m_type = TYPE_NONE;
	m_nFloating = 0;
	m_fInit = 0.0f;
	m_bCollisionOld = false;
	m_nCollisionCounter = 0;
	m_bEnable = true;
	m_nIdxModel = 0;
	m_nWalkTimer = 0;
}

//=============================================================================
//デストラクタ
CTarget::~CTarget()
{
}

//=============================================================================
//モデルの生成
CTarget* CTarget::Create(TYPE type, const std::string& fileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	//プレイヤー
	CTarget* pObjX = NULL;

	if (pObjX == NULL)
	{
		//弾生成
		pObjX = new CTarget;

		if (pObjX != NULL)
		{
			//各種代入
			pObjX->SetPosition(pos);
			pObjX->m_rot = rot;
			pObjX->m_pFileName = fileName;
			pObjX->m_type = type;

			pObjX->Init(pObjX->m_pos, pObjX->m_size);
		}
	}
	return pObjX;
}

//=============================================================================
//オブジェクトの初期化
HRESULT CTarget::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	//モデルマネージャーの取得
	CModelManager* pModelManager = CManager::GetModelManager();

	//登録（既にロード済みならキャッシュから即座にインデックスが返る）
	m_nIdxModel = pModelManager->Register(m_pFileName.c_str());

	//読み込んだ共有モデルデータから境界情報等を設定
	CModelManager::MODEL_DATA* pModelData = pModelManager->GetModelData(m_nIdxModel);
	if (pModelData != NULL)
	{
		m_vtxMax = pModelData->vtxMax;
		m_vtxMin = pModelData->vtxMin;
	}

	//Yの初期位置を保存
	m_fInit = m_pos.y;

	return S_OK;
}

//=============================================================================
//オブジェクトの終了
void CTarget::Uninit(void)
{
	if (!m_bEnable)
	{
		return; //すでに無効化されていればスキップ
	}

	m_bEnable = false; //フラグをオフにする

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
void CTarget::Update(void)
{
	if (!m_bEnable)
	{
		return;
	}

	// 爆発範囲との判定
	if (CollisionVigilanceRange() == true)
	{
		DethHuman();	//人間の死亡
		return;
	}

	// オブジェクトの中心座標を計算する
	m_Center.x = m_pos.x + (m_vtxMax.x + m_vtxMin.x) * 0.5f;
	m_Center.z = m_pos.z + (m_vtxMax.z + m_vtxMin.z) * 0.5f;

	//プレイヤーの取得
	CScene* pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;

	if (pScene->GetMode() == CScene::MODE_GAME)
	{//ゲームのプレイヤー取得
		pPlayer = CGame::GetPlayer();
	}
	else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	{//チュートリアルのプレイヤー取得
		pPlayer = CTutorial::GetPlayer();
	}

	if (this != NULL)
	{
		m_nWalkTimer++;

		//進行方向の先読み座標
		float fCheckDist = 20.0f;
		D3DXVECTOR3 nextPos = m_pos;
		nextPos.x -= sinf(m_rot.y) * fCheckDist;
		nextPos.z -= cosf(m_rot.y) * fCheckDist;

		//-----------------------------------------------------------------
		//ステージの上限設定(humanが行ける範囲)
		bool bHitStageEdge = false;
		float fStageLimit = 0.0f;

		if (pScene->GetMode() == CScene::MODE_GAME)
		{//ゲーム中の時
			fStageLimit = 800.0f; //壁の直前で手前ターンさせるため少し余裕を持たせる
		}
		else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
		{//チュートリアル中
			fStageLimit = 400.0f; //壁の直前で手前ターンさせるため少し余裕を持たせる
		}

		if (nextPos.x > fStageLimit || nextPos.x < -fStageLimit ||
			nextPos.z > fStageLimit || nextPos.z < -fStageLimit)
		{
			bHitStageEdge = true;
		}

		//-----------------------------------------------------------------
		//箱・爆弾の障害物チェック
		bool bHitObstacle = false;

		//-----------------------------------------------------------------
		//方向転換と前進処理
		if (m_nWalkTimer >= 360 || bHitObstacle || bHitStageEdge)
		{
			m_nWalkTimer = 0;

			// 180度反転 ＋ ±45度のランダムな揺らぎ
			float fRandomOffset = ((float)(rand() % 90) - 45.0f) * (D3DX_PI / 180.0f);
			m_rot.y += (D3DX_PI + fRandomOffset);

			// 角度の正規化
			if (m_rot.y > D3DX_PI) m_rot.y -= D3DX_PI * 2.0f;
			else if (m_rot.y < -D3DX_PI) m_rot.y += D3DX_PI * 2.0f;
		}
		else
		{
			//障害物・ステージ端・壁モデルがなければ前進
			float fSpeed = 1.2f;
			m_pos.x -= sinf(m_rot.y) * fSpeed;
			m_pos.z -= cosf(m_rot.y) * fSpeed;
		}
	}

	if (CollisionPlayer() == true && m_bCollisionOld != true)
	{
		DethHuman();
	}
}

//=============================================================================
//オブジェクトの描画
void CTarget::Draw(void)
{
	DrawShadow();

	//モデルデータを取得
	CModelManager::MODEL_DATA* pData = CManager::GetModelManager()->GetModelData(m_nIdxModel);

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
//木箱破壊処理
void CTarget::BreakBox(void)
{
	//プレイヤーの取得
	CScene* pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;

	//if (pScene->GetMode() == CScene::MODE_GAME)
	//{
	//	pPlayer = CGame::GetPlayer();

	//	//箱の破壊数を増やす
	//	pPlayer->AddBreakBox();
	//	CGame::SubBox(this);	//ゲーム内で記録している箱の数を減らす
	//}
	//else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	//{
	//	pPlayer = CTutorial::GetPlayer();

	//	//箱の破壊数を増やす
	//	pPlayer->AddBreakBox();
	//	CTutorial::SubBox(this);	//チュートリアル内で記録している箱の数を減らす
	//}

	////パーティクルを出して消す
	//CParticle::CreateModel(CParticle::TYPE_BREAKBOX, m_pos, 15, 2);

	////SE再生
	//CSound::PlaySound_(CSound::SOUND_LABEL_BOXBREAK);

	//終了処理
	Uninit();
}

//=============================================================================
//爆弾破壊処理
void CTarget::BreakBombBox(void)
{
	//既に無効化されている場合はスキップ
	if (!m_bEnable)
	{
		return;
	}

	//自身の終了処理
	Uninit();

	//カウント処理-----------------------------------------
	//プレイヤーの取得
	CScene* pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;

	//if (pScene->GetMode() == CScene::MODE_GAME)
	//{
	//	pPlayer = CGame::GetPlayer();
	//	CGame::SubBox(this);		//ゲーム内で記録している箱の数を減らす
	//}
	//else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
	//{
	//	pPlayer = CTutorial::GetPlayer();
	//	CTutorial::SubBox(this);	//チュートリアル内で記録している箱の数を減らす
	//}
	//if (pPlayer != NULL)
	//{
	//	pPlayer->AddBreakBox();
	//}

	//爆風領域を生成（パーティクルやSEはCVigilance側に任せる）
	CVigilance* pVigilance = CVigilance::Create(m_pos, 400.0f, 8.0f);
}

//=============================================================================
//爆風判定処理
bool CTarget::CollisionVigilanceRange(void)
{
	//CBombが登録されているPriorityのリストの先頭を取得
	CObject* pObj = CObject::GetTop(VIGILANCE_PRIORITY);

	while (pObj != NULL)
	{
		//TYPE_BOMB→タイプ判定で識別してキャストする
		if (pObj->GetType() == CObject::TYPE_TARGET)
		{
			CVigilance* pVigilance = static_cast<CVigilance*>(pObj);

			if (pVigilance != NULL)
			{
				//距離計算
				D3DXVECTOR3 diff = m_pos - pVigilance->GetPosition();
				float fDistSq = D3DXVec3LengthSq(&diff);

				float fRadius = pVigilance->GetRadius();
				float fRadiusSq = fRadius * fRadius;

				if (fDistSq <= fRadiusSq)
				{
					return true; //爆発の範囲内に入っている
				}
			}
		}

		pObj = pObj->GetNext(); //次のオブジェクトへ
	}

	return false;
}

//=============================================================================
//人間の死亡時処理
void CTarget::DethHuman(void)
{
	//既に無効化されている場合はスキップ
	if (!m_bEnable)
	{
		return;
	}

	//自身の終了処理
	Uninit();

	//カウント処理-----------------------------------------
	//プレイヤーの取得
	CScene* pScene = CManager::GetScene();
	CPlayer* pPlayer = NULL;

	if (pScene->GetMode() != CScene::MODE_TITLE)
	{//タイトルの時はここらへんの処理なし
		if (pScene->GetMode() == CScene::MODE_GAME)
		{
			pPlayer = CGame::GetPlayer();
			CGame::SubTarget(this);			//ゲーム内で記録している人間の数を減らす
		}
		else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
		{
			pPlayer = CTutorial::GetPlayer();
			//CTutorial::SubTarget(this);		//チュートリアル内で記録している人間の数を減らす
		}

		if (pPlayer != NULL)
		{
			//pPlayer->AddDethHuman();

			//人間が爆発に巻き込まれたらコンボリセット
			//pPlayer->PenaltyDethHuman();
		}
	}

	/*if (m_type == TYPE_HUMAN1)
	{*/
	//パーティクル
	CParticle::CreateModel(CParticle::TYPE_DETH_HUMAN2, m_pos, 0, 1);

	//SE再生
	//CSound::PlaySound_(CSound::SOUND_LABEL_HUMAN_DETH);
	//}
	//else if (m_type == TYPE_HUMAN2)
	//{
	//	//パーティクル
	//	CParticle::CreateModel(CParticle::TYPE_DETH_HUMAN2, m_pos, 0, 1);

	//	//SE再生
	//	CSound::PlaySound_(CSound::SOUND_LABEL_HUMAN_DETH);
	//}
}

//=============================================================================
//弾との当たり判定
bool CTarget::CollisionBullet(const D3DXVECTOR3& start, const D3DXVECTOR3& end, float radius) const
{
    if (!m_bEnable || radius < 0.0f)
    {
        return false;
    }

    D3DXMATRIX rotation, translation, world, inverseWorld;
    D3DXMatrixRotationYawPitchRoll(&rotation, m_rot.y, m_rot.x, m_rot.z);
    D3DXMatrixTranslation(&translation, m_pos.x, m_pos.y, m_pos.z);
    D3DXMatrixMultiply(&world, &rotation, &translation);

    if (D3DXMatrixInverse(&inverseWorld, NULL, &world) == NULL)
    {
        return false;
    }

    D3DXVECTOR3 localStart, localEnd;
    D3DXVec3TransformCoord(&localStart, &start, &inverseWorld);
    D3DXVec3TransformCoord(&localEnd, &end, &inverseWorld);

    const float minBounds[3] = { m_vtxMin.x - radius, m_vtxMin.y - radius, m_vtxMin.z - radius };
    const float maxBounds[3] = { m_vtxMax.x + radius, m_vtxMax.y + radius, m_vtxMax.z + radius };
    const float startValues[3] = { localStart.x, localStart.y, localStart.z };
    const float endValues[3] = { localEnd.x, localEnd.y, localEnd.z };
    float enter = 0.0f;
    float leave = 1.0f;

    for (int axis = 0; axis < 3; ++axis)
    {
        const float delta = endValues[axis] - startValues[axis];
        if (fabsf(delta) < 0.000001f)
        {
            if (startValues[axis] < minBounds[axis] || startValues[axis] > maxBounds[axis])
            {
                return false;
            }
            continue;
        }

        float t1 = (minBounds[axis] - startValues[axis]) / delta;
        float t2 = (maxBounds[axis] - startValues[axis]) / delta;
        if (t1 > t2)
        {
            const float temp = t1;
            t1 = t2;
            t2 = temp;
        }

        if (t1 > enter) enter = t1;
        if (t2 < leave) leave = t2;
        if (enter > leave)
        {
            return false;
        }
    }

    return true;
}

//=============================================================================
//影の描画
void CTarget::DrawShadow(void)
{
	if (!m_bEnable)
	{
		return;
	}

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//光源方向の設定
	D3DXVECTOR4 lightDir(-10.0f, 20.0f, 10.0f, 0.0f);

	//CFieldから平面情報（位置と法線）を取得
	D3DXVECTOR3 planePoint(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 planeNormal(0.0f, 1.0f, 0.0f);

	CField* pField = CGame::GetField(); //ゲーム管理クラスからフィールドを取得

	if (pField != NULL)
	{
		planePoint = pField->GetPlanePoint();
		planePoint.y += 1.0f;
		planeNormal = pField->GetPlaneNormal();
	}

	//影行列の生成(※webサイト:〇×の深度バッファシャドウ。忘れたら見る)
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
	float fDepthBias = -0.0001f;
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

//=============================================================================
//プレイヤーとの当たり判定
bool CTarget::CollisionPlayer(void)
{
	//プレイヤーとシーンの取得
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

	//プレイヤーが高さの範囲内にいるか
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
		//押し戻しベクトルの計算(diffX or Y / distance)
		localPlayerX += diffX / distance * overlap;
		localPlayerZ += diffZ / distance * overlap;

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
//カメラとの当たり判定	(プレイヤーの判定とほぼ同じ)
bool CTarget::CollisionCamera(void)
{
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