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
#include "range.h"

//マクロ定義-------------------------------------------------------------------
#define COLLISION_MODEL_W		(15.0f)
#define COLLISION_MODEL_H		(15.0f)

//静的メンバ変数宣言-----------------------------------------------------------
int CTarget::m_wallnum = 0;
CTarget::MODEL_INFO CTarget::m_aModelInfo[30];
CMotion::INFO CTarget::m_aMotionInfo[MOTIONTYPE_MAX];

//=============================================================================
//コンストラクタ
CTarget::CTarget(int nPriority) :CObject(nPriority)
{
	memset(&m_nIdxTexture[0], 0, sizeof(m_nIdxTexture));
	m_pMesh = NULL;
	m_pBuffMat = NULL;
	m_dwNumMat = 0;
	memset(m_mtxWorld, 0, sizeof(m_mtxWorld));
	m_vtxMin = D3DXVECTOR3(-COLLISION_MODEL_W, 0.0f, -COLLISION_MODEL_W);
	m_vtxMax = D3DXVECTOR3(COLLISION_MODEL_W, COLLISION_MODEL_H * 2.0f, COLLISION_MODEL_W);
	m_Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_enemyPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DCOLOR_RGBA(255, 255, 255, 255);
	m_type = TYPE_NONE;
	m_nFloating = 0;
	m_fInit = 0.0f;
	m_bCollisionOld = false;
	m_nCollisionCounter = 0;
	m_bEnable = true;
	m_nWalkTimer = 0;
	m_pMotion = NULL;
	memset(&m_apModel[0], NULL, sizeof(m_apModel));
	m_nNumModel = 0;

	m_pRange = NULL;
}

//=============================================================================
//デストラクタ
CTarget::~CTarget()
{
}

//=============================================================================
//生成
CTarget* CTarget::Create(TYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CTarget* pTarget = new CTarget;
	if (pTarget != NULL)
	{
		pTarget->SetPosition(pos);
		pTarget->m_rot = rot;
		pTarget->m_type = type;
		pTarget->Init(pTarget->m_pos, pTarget->m_size);
	}
	return pTarget;
}

//=============================================================================
//初期化
HRESULT CTarget::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_fInit = m_pos.y;
	m_pMotion = new CMotion;
	if (m_pMotion == NULL)
	{
		return E_FAIL;
	}

	LoadParentModel("data/SCRIPT/drone.txt");
	LoadMotionInfo("data/Motion/motion_drone.txt");

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		m_apModel[nCntModel] = CModel::Create(m_aModelInfo[nCntModel].FileName, m_aModelInfo[nCntModel].pos, m_aModelInfo[nCntModel].rot);

		if (m_apModel[nCntModel] == NULL)
		{
			continue;
		}

		int nParent = m_aModelInfo[nCntModel].nIdxParent;
		if (nParent < 0 || nParent >= nCntModel || m_apModel[nParent] == NULL)
		{
			m_apModel[nCntModel]->SetParent(NULL);
		}
		else
		{
			m_apModel[nCntModel]->SetParent(m_apModel[nParent]);
		}
	}
	m_pMotion->SetModel(m_apModel, m_nNumModel);

	for (int nCntInfo = 0; nCntInfo < MOTIONTYPE_MAX; nCntInfo++)
	{
		m_pMotion->SetInfo(m_aMotionInfo[nCntInfo]);
	}

	m_pMotion->Set(MOTIONTYPE_NEUTRAL, 15);

	m_pRange = CRange::Create(m_pos, D3DXVECTOR3(100.0f, 0.0f, 100.0f), m_rot, m_type);
	if (m_pRange != NULL && m_type == TYPE_FAN)
	{
		// Place the fan range ahead immediately, before the first update.
		D3DXVECTOR3 rangePos = m_pos;
		rangePos.x -= sinf(m_rot.y) * TARGET_FAN_FORWARD_OFFSET;
		rangePos.z -= cosf(m_rot.y) * TARGET_FAN_FORWARD_OFFSET;
		m_pRange->SetPosition(rangePos);
	}

	return S_OK;
}

//=============================================================================
//終了
void CTarget::Uninit(void)
{
	if (!m_bEnable)
	{
		return;
	}

	m_bEnable = false;

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL)
		{
			m_apModel[nCntModel]->Uninit();
			m_apModel[nCntModel]->SetParent(NULL);
			delete m_apModel[nCntModel];
			m_apModel[nCntModel] = NULL;
		}
	}
	m_nNumModel = 0;

	if (m_pMotion != NULL)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat->Release();
		m_pBuffMat = NULL;
	}
	if (m_pRange != NULL)
	{
		m_pRange->Uninit();
		m_pRange = NULL;
	}

	CObject::Release();
}

//=============================================================================
//更新
void CTarget::Update(void)
{
	if (!m_bEnable)
	{
		return;
	}

	if (m_pMotion != NULL)
	{
		m_pMotion->Update();
		if (m_type != TYPE_NONE) m_pMotion->Set(MOTIONTYPE_MOVE, 3);
	}

	if (CollisionVigilanceRange())
	{
		DethTarget();
		return;
	}

	m_Center.x = m_pos.x + (m_vtxMax.x + m_vtxMin.x) * 0.5f;
	m_Center.z = m_pos.z + (m_vtxMax.z + m_vtxMin.z) * 0.5f;

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

	if (this != NULL)
	{
		m_nWalkTimer++;
		const float fCheckDist = 20.0f;
		D3DXVECTOR3 nextPos = m_pos;
		nextPos.x -= sinf(m_rot.y) * fCheckDist;
		nextPos.z -= cosf(m_rot.y) * fCheckDist;

		bool bHitStageEdge = false;
		float fStageLimit = 0.0f;
		if (pScene->GetMode() == CScene::MODE_GAME) fStageLimit = 800.0f;
		else if (pScene->GetMode() == CScene::MODE_TUTORIAL) fStageLimit = 400.0f;

		if (nextPos.x > fStageLimit || nextPos.x < -fStageLimit ||
			nextPos.z > fStageLimit || nextPos.z < -fStageLimit)
		{
			bHitStageEdge = true;
		}

		bool bHitObstacle = false;
		if (m_nWalkTimer >= 360 || bHitObstacle || bHitStageEdge)
		{
			m_nWalkTimer = 0;
			const float fRandomOffset = ((float)(rand() % 90) - 45.0f) * (D3DX_PI / 180.0f);
			m_rot.y += D3DX_PI + fRandomOffset;
			if (m_rot.y > D3DX_PI) m_rot.y -= D3DX_PI * 2.0f;
			else if (m_rot.y < -D3DX_PI) m_rot.y += D3DX_PI * 2.0f;
		}
		else
		{
			// Keep the existing target movement disabled.
		}
	}

	if (m_pRange != NULL)
	{
		// Fan display and collision are both placed ahead in the target's forward direction.
		D3DXVECTOR3 rangePos = m_pos;
		if (m_type == TYPE_FAN)
		{
			rangePos.x -= sinf(m_rot.y) * TARGET_FAN_FORWARD_OFFSET;
			rangePos.z -= cosf(m_rot.y) * TARGET_FAN_FORWARD_OFFSET;
		}
		m_pRange->SetPosition(rangePos);
		m_pRange->SetRotation(D3DXVECTOR3(m_rot.x, m_rot.y, m_rot.z));
	}

	if (CollisionPlayer() == true && m_bCollisionOld != true)
	{
		DethTarget();
		return;
	}
}

//=============================================================================
//描画
void CTarget::Draw(void)
{
	DrawShadow();

	D3DXMATRIX mtxRot, mtxTrans;
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	D3DXMatrixIdentity(&m_mtxWorld);

	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL) m_apModel[nCntModel]->Draw();
	}
}

//=============================================================================
//
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
					return true;	//爆発の範囲内に入っている
				}
			}
		}

		pObj = pObj->GetNext();		//次のオブジェクトへ
	}

	return false;
}

//=============================================================================
//ターゲットの死亡時処理
void CTarget::DethTarget(void)
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
			CGame::SubTarget(this);			//ゲーム内で記録しているターゲットの数を減らす
		}
		else if (pScene->GetMode() == CScene::MODE_TUTORIAL)
		{
			pPlayer = CTutorial::GetPlayer();
			//CTutorial::SubTarget(this);		//チュートリアル内で記録しているターゲットの数を減らす
		}

		if (pPlayer != NULL)
		{
			//pPlayer->AddDethHuman();

			//ターゲットが爆発に巻き込まれたらコンボリセット
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
HRESULT CTarget::LoadParentModel(const std::string& fileName)
{
	FILE* pFile = fopen(fileName.c_str(), "r");

	if (pFile == NULL)
	{//読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};		    //文字列読み込み
	static char aStrCpy[MAX_STRING] = {};	    //文字列複製(整理)
	static char* pStart = NULL;				    //文字列開始位置
	static char aModelPath[FILE_MAX] = {};		//モデルのファイル名読み込み
	int nIdx = 0;								//モデルのインデックス読み込み
	int nParent = 0;							//モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};						//位置読み込み
	D3DXVECTOR3 rot = {};						//向き読み込み
	int nCntParts = 0;							//パーツ数カウント
	int nCntFileName = 0;						//ファイル名カウント
	int nCntModel = 0;							//モデル数

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));			//文字列クリア
		(void)fgets(aStr, sizeof(aStr), pFile);	//一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{//LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pFile) != NULL)
		{//読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, 0, sizeof(aStr));				//文字列クリア
		memset(aStrCpy, 0, sizeof(aStrCpy));		//コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pFile);		//一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);	//有効文字だけ抜き取って複製

		if (strstr(aStr, LOAD_NUMMODEL))
		{//NUM_MODELを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%d", &nCntModel);
		}

		if (strstr(aStr, LOAD_MODEL))
		{//MODEL_FILENAMEを読み込んだ

			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%s", &aModelPath);

			LoadParentFile(aModelPath, nCntFileName, nCntModel);
			nCntFileName++;
		}

		if (strstr(aStr, LOAD_PLAYER))
		{//CHARACTERSETを読み込んだ
			while (true)
			{
				memset(aStr, 0, sizeof(aStr));				//文字列クリア
				memset(aStrCpy, 0, sizeof(aStrCpy));		//コピーもクリア
				(void)fgets(aStr, sizeof(aStr), pFile);		//一列読み込み
				LoadEnableString(&aStrCpy[0], &aStr[0]);	//有効文字だけ抜き取って複製

				if (strcmp(aStrCpy, LOAD_PARTS) == 0)
				{//PARTSSETを読み込んだ
					while (true)
					{
						memset(aStr, 0, sizeof(aStr));
						memset(aStrCpy, 0, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_INDEX))
						{//INDEXを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nIdx);
							continue;
						}

						if (strstr(aStr, LOAD_PARENT))
						{//PARENTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nParent);
							continue;
						}

						if (strstr(aStr, LOAD_POS))
						{//POSを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);
							continue;
						}

						if (strstr(aStr, LOAD_ROT))
						{//ROTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &rot.x, &rot.y, &rot.z);
							continue;
						}

						if (strcmp(aStrCpy, LOAD_ENDPARTS) == 0)
						{//END_PARTSSETを読み込んだ
							LoadOffSet(pos, rot, nParent, nIdx);
							nCntParts++;

							//値をクリア
							memset(&pos, NULL, sizeof(D3DXVECTOR3));
							memset(&rot, NULL, sizeof(D3DXVECTOR3));
							nIdx = 0;
							nParent = 0;
							break;
						}
					}
				}

				if (strcmp(aStrCpy, LOAD_ENDPLAYER) == 0)
				{//END_CHARACTERSETを読み込んだ
					nCntParts = 0;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pFile);
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//階層構造のモデルファイルをロード
void CTarget::LoadParentFile(const std::string& fileName, int nIdxFile, int nMaxModel)
{
	if (nIdxFile < nMaxModel)
	{
		m_aModelInfo[nIdxFile].FileName = fileName;
	}
}

//=============================================================================
//オフセット読み込み処理
void CTarget::LoadOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxParent, int nIdxModel)
{
	CDebugproc* pDebug = CManager::GetDebugproc();

	int nIdx = m_nNumModel;

	m_aModelInfo[nIdx].nIdxModel = nIdxModel;
	m_aModelInfo[nIdx].nIdxParent = nIdxParent;
	m_aModelInfo[nIdx].pos = pos;
	m_aModelInfo[nIdx].rot = rot;

	m_nNumModel++;
}

//=============================================================================
//モーション情報読み込み処理
HRESULT CTarget::LoadMotionInfo(const std::string& fileName)
{
	FILE* pMotionFile = fopen(fileName.c_str(), "r");

	if (pMotionFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	static char aStr[MAX_STRING] = {};				// 文字列読み込み
	static char aStrCpy[MAX_STRING] = {};			// 文字列複製(整理)
	static char* pStart = NULL;						// 文字列開始位置
	static char aModelPath[FILE_MAX] = {};			// モデルのファイル名読み込み
	int nIdx = 0;									// モデルのインデックス読み込み
	int nParent = 0;								// モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};							// 位置読み込み
	D3DXVECTOR3 rot = {};							// 向き読み込み
	int nNumModel = 0;								// モデル数読み込み
	int nCntMotion = 0;								// モーション数カウント
	int bLoop = false;								// ループするかどうか読み込み
	int nNumKey = 0;								// キー数読み込み
	static CMotion::KEY_INFO KeyInfo[MAX_KEY] = {};	// キー情報読み込み
	int nCntKey = 0;								// キー数カウント
	int nCntParts = 0;								// パーツ数カウント

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pMotionFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pMotionFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));
		memset(aStrCpy, NULL, sizeof(aStrCpy));
		(void)fgets(aStr, sizeof(aStr), pMotionFile);
		LoadEnableString(&aStrCpy[0], &aStr[0]);

		if (strcmp(aStrCpy, LOAD_MOTIONINFO) == 0)
		{// MOTIONSETを読み込んだ
			while (true)
			{
				memset(aStr, NULL, sizeof(aStr));
				memset(aStrCpy, NULL, sizeof(aStrCpy));
				(void)fgets(aStr, sizeof(aStr), pMotionFile);
				LoadEnableString(&aStrCpy[0], &aStr[0]);

				if (strstr(aStr, LOAD_MOTIONLOOP))
				{// LOOPを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &bLoop);
					continue;
				}

				if (strstr(aStr, LOAD_NUMKEY))
				{// NUM_KEYを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &nNumKey);
					continue;
				}

				if (strcmp(aStrCpy, LOAD_KEYSET) == 0)
				{// KEYSETを読み込んだ
					while (true)
					{
						memset(aStr, NULL, sizeof(aStr));
						memset(aStrCpy, NULL, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pMotionFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_FRAME))
						{// FRAMEを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &KeyInfo[nCntKey].nFrame);
							continue;
						}

						if (strcmp(aStrCpy, LOAD_KEY) == 0)
						{// KEYを読み込んだ
							while (true)
							{
								memset(aStr, NULL, sizeof(aStr));
								memset(aStrCpy, NULL, sizeof(aStrCpy));
								(void)fgets(aStr, sizeof(aStr), pMotionFile);
								LoadEnableString(&aStrCpy[0], &aStr[0]);

								if (strstr(aStr, LOAD_POS))
								{// POSを読み込んだ
									if ((pStart = strchr(aStr, '=')) == NULL)
									{
										continue;
									}

									(void)sscanf(pStart + 1, "%f %f %f", &KeyInfo[nCntKey].aKey[nCntParts].fPosX, &KeyInfo[nCntKey].aKey[nCntParts].fPosY, &KeyInfo[nCntKey].aKey[nCntParts].fPosZ);
									continue;
								}

								if (strstr(aStr, LOAD_ROT))
								{// ROTを読み込んだ
									if ((pStart = strchr(aStr, '=')) == NULL)
									{
										continue;
									}

									(void)sscanf(pStart + 1, "%f %f %f", &KeyInfo[nCntKey].aKey[nCntParts].fRotX, &KeyInfo[nCntKey].aKey[nCntParts].fRotY, &KeyInfo[nCntKey].aKey[nCntParts].fRotZ);
									continue;
								}

								if (strcmp(aStrCpy, LOAD_ENDKEY) == 0)
								{// END_KEYを読み込んだ
									nCntParts++;
									break;
								}
							}
						}

						if (strcmp(aStrCpy, LOAD_ENDKEYSET) == 0)
						{// END_KEYSET
							nCntKey++;
							nCntParts = 0;
							break;
						}
					}
				}

				if (strcmp(aStrCpy, LOAD_ENDMOTIONINFO) == 0)
				{// END_MOTIONSETを読み込んだ
					LoadMotion(bLoop, nNumKey, &KeyInfo[0], nCntMotion);
					nCntMotion++;

					// 各種変数初期化
					memset(&KeyInfo, NULL, sizeof(CMotion::KEY_INFO));
					bLoop = false;
					nNumKey = 0;
					nCntKey = 0;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pMotionFile);
			break;
		}
	}
	return S_OK;
}

//=============================================================================
//	モーション読み込み処理
void CTarget::LoadMotion(bool bLoop, int nNumKey, CMotion::KEY_INFO* pKeyInfo, int nMotion)
{
	if (nMotion < MOTIONTYPE_MAX)
	{
		m_aMotionInfo[nMotion].bLoop = bLoop;
		m_aMotionInfo[nMotion].nNumKey = nNumKey;

		for (int nCntKey = 0; nCntKey < m_aMotionInfo[nMotion].nNumKey; nCntKey++)
		{
			m_aMotionInfo[nMotion].aKeyInfo[nCntKey] = pKeyInfo[nCntKey];
		}
	}
}

//=============================================================================
//有効文字抜き出し処理
void CTarget::LoadEnableString(char* aStrCpy, char* pStart)
{
	while (true)
	{
		if (*pStart == '\n')
		{
			pStart++;
			continue;
		}
		else if (*pStart == '\t')
		{
			pStart++;
			continue;
		}
		else if (*pStart == 0x32)
		{
			pStart++;
			continue;
		}
		else if (*pStart == '#' || *pStart == '\0')
		{
			break;
		}
		else
		{
			strncat(aStrCpy, pStart, 1);
			pStart++;
		}
	}
}

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

		for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] != NULL) m_apModel[nCntModel]->Draw();
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
	if (m_type == TYPE_FAN)
	{
		// Fan collision follows the ground-level cutoff used by the range trigger.
		if (pos.y > 50.0f) return false;
		return CollisionFan(pos);
	}

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
// 点がターゲット前方の扇形内にあるかを判定する。
bool CTarget::CollisionFan(const D3DXVECTOR3& point) const
{
	const D3DXVECTOR3 fanPos = (m_pRange != NULL) ? m_pRange->GetPosition() : m_pos;
	const float fanLength = (m_pRange != NULL) ? m_pRange->GetSize().x : 100.0f;

	// 扇形の中心からプレイヤーまでのXZ方向ベクトルを求める。
	const float dx = point.x - fanPos.x;
	const float dz = point.z - fanPos.z;
	const float distanceSq = dx * dx + dz * dz;

	// 扇形の長さを超えた点は範囲外とする。
	if (distanceSq > fanLength * fanLength)
	{
		return false;
	}

	// 中心点では方向を定義できないため、範囲内として扱う。
	if (distanceSq < 0.000001f)
	{
		return true;
	}

	const float distance = sqrtf(distanceSq);

	// このプロジェクトでは前方を -sin(yaw), -cos(yaw) とする。
	const float forwardX = -sinf(m_rot.y);
	const float forwardZ = -cosf(m_rot.y);

	// 内積でターゲットの前方とプレイヤー方向の角度を比べる。
	const float dot = forwardX * dx + forwardZ * dz;

	// TARGET_FAN_ANGLE_DEGREES は扇全体の角度なので、半分の角度で比較する。
	const float cosHalfAngle = cosf(D3DXToRadian(TARGET_FAN_ANGLE_DEGREES * 0.5f));

	// 内積が境界値を下回れば扇の角度外とする。
	return dot >= distance * cosHalfAngle;
}


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