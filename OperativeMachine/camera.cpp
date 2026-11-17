//=============================================================================
// 
// カメラ[camera.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "camera.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "input.h"

#include "scene.h"
#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "clear.h"
#include "fail.h"
#include "result.h"

#include "player.h"
#include "objectX.h"

//マクロ定義 ------------------------------------------------------------------
#define CAMERA_HUMAN_DIST    (CAMERA_ANGLE_PZ)  //人型時の距離
#define CAMERA_HUMAN_AT_Y    (70.0f)            //人型時の注視点高さ

#define CAMERA_SPIDER_DIST   (100.0f)           //クモ型時の距離
#define CAMERA_SPIDER_AT_Y   (15.0f)            //クモ型時の注視点高さ

#define CAMERA_CANNON_DIST   (80.0f)            //銃型時の距離
#define CAMERA_CANNON_AT_Y   (15.0f)            //銃型時の注視点高さ

//=============================================================================
//コンストラクタ
CCamera::CCamera()
{
	m_posV = D3DXVECTOR3(-CAMERA_ANGLE_PZ, CAMERA_ANGLE_PY, 0.0f);
	m_posR = D3DXVECTOR3(0.0f, 80.0f, 0.0f);
	m_posU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_angle = D3DXVECTOR3(CAMERA_ANGLE_PZ, CAMERA_ANGLE_PY, CAMERA_ANGLE_PZ);
	m_posVDest = D3DXVECTOR3(0.0f, CAMERA_ANGLE_PY, -CAMERA_ANGLE_PZ);
	m_posRDest = D3DXVECTOR3(0.0f, 70.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_counter = STAND_BY;
	m_fastKeyPress = 0;
}

//=============================================================================
//デストラクタ
CCamera::~CCamera()
{

}

//=============================================================================
//初期化
void CCamera::Init(void)
{
}

//=============================================================================
//終了
void CCamera::Uninit(void)
{

}

//=============================================================================
//更新
void CCamera::Update(void)
{
	CDebugproc* pDebug = CManager::GetDebugproc();
	CInputKeyboard* pInputKey = CManager::GetInputKeyboard();
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();

	CScene* pScene = CManager::GetScene();
	D3DXVECTOR3 posPlayer = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 rotPlayer = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	CPlayer* pPlayer = NULL;

	//プレイヤーの各シーン取得
	if (pScene != NULL)
	{
		if (pScene->GetMode() == CScene::MODE_GAME && pPlayer == NULL)
		{
			pPlayer = CGame::GetPlayer();
		}
		else if (pScene->GetMode() == CScene::MODE_TITLE && pPlayer == NULL)
		{
			pPlayer = CTitle::GetPlayer();
		}
		else if (pScene->GetMode() == CScene::MODE_TUTORIAL && pPlayer == NULL)
		{
			pPlayer = CTutorial::GetPlayer();
		}
		else if (pScene->GetMode() == CScene::MODE_CLEAR && pPlayer == NULL)
		{
			pPlayer = CClear::GetPlayer();
		}
		else if (pScene->GetMode() == CScene::MODE_FAIL && pPlayer == NULL)
		{
			pPlayer = CFail::GetPlayer();
		}
		else if (pScene->GetMode() == CScene::MODE_RESULT && pPlayer == NULL)
		{
			pPlayer = CResult::GetPlayer();
		}
	}

	//形態に応じたカメラパラメータ目標値
	float targetDist = CAMERA_HUMAN_DIST;
	float targetAtY = CAMERA_HUMAN_AT_Y;
	bool bIsClimbing = false;

	if (pPlayer != NULL)
	{
		//プレイヤーのpos, rotを取得(GetPosition/GetRotation)
		posPlayer = pPlayer->GetPosition();
		rotPlayer = pPlayer->GetRotation();

		//壁登り中かチェック
		bIsClimbing = pPlayer->IsClimbingWall();

		//プレイヤーの型に応じたカメラ処理
		switch (pPlayer->GetMode())
		{
		case CPlayer::MODE_HUMAN:
			targetDist = CAMERA_HUMAN_DIST;
			targetAtY = CAMERA_HUMAN_AT_Y;
			break;

		case CPlayer::MODE_SPIDER:
			targetDist = CAMERA_SPIDER_DIST;
			targetAtY = CAMERA_SPIDER_AT_Y;
			break;

		case CPlayer::MODE_CANNON:
			targetDist = CAMERA_CANNON_DIST;
			targetAtY = CAMERA_CANNON_AT_Y;
			m_rot.x = -14.0f;
			break;
		}
	}

	//形態切替時の滑らかな補間処理(m_angle.zとm_angle.yに段階的に設定)
	m_angle.z += (targetDist - m_angle.z) * 0.1f;
	m_angle.y += (targetAtY - m_angle.y) * 0.1f;

	//クモ型の場合はカメラ位置も少し低くする
	float fHeightOffset = (pPlayer && (pPlayer->GetMode() == CPlayer::MODE_SPIDER || pPlayer->GetMode() == CPlayer::MODE_CANNON)) ? 10.0f : 30.0f;

	//キーボード操作-------------------------
	if (pInputKey->GetPress(DIK_UP) == true)
	{
		m_rot.x += 2.0f; //Rキーでカメラ位置を低く 
		if (m_rot.x > 100.0f) m_rot.x = 100.0f;		//下限制限
	}
	if (pInputKey->GetPress(DIK_DOWN) == true)
	{
		m_rot.x -= 2.0f; // Fキーでカメラ位置を高く
		if (m_rot.x < -50.0f) m_rot.x = -70.0f;		//上限制限
	}

	//元の座標計算構造のまま、m_rot.x を高さ(Y)のオフセットとして直接反映
	m_posVDest.x = m_posR.x - sinf(m_rot.y) * m_angle.z;
	m_posVDest.z = m_posR.z - cosf(m_rot.y) * m_angle.z;
	m_posVDest.y = posPlayer.y + m_angle.y + fHeightOffset + m_rot.x; //m_rot.xを加算

	m_posRDest.x = posPlayer.x;
	m_posRDest.z = posPlayer.z;
	m_posRDest.y = posPlayer.y + (bIsClimbing ? 10.0f : m_angle.y);

	//キー入力操作-------------------------------------------------------------
	if (pInputKey->GetTrigger(DIK_BACKSPACE) == true)
	{
		m_posV = D3DXVECTOR3(-m_angle.z, m_angle.y, 0.0f);
		m_posR = D3DXVECTOR3(0.0f, 80.0f, 0.0f);
		m_posU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_posVDest = D3DXVECTOR3(0.0f, m_angle.y, -m_angle.z);
		m_posRDest = D3DXVECTOR3(0.0f, 70.0f, 0.0f);
		m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		m_counter = STAND_BY;
		m_fastKeyPress = 0;

		//プレイヤーの後ろから見る向きにする
		m_rot.y = pPlayer->GetRotation().y - D3DX_PI;
	}

	//視点をプレイヤーの向きにリセット (Rキーとの重複回避のためTキーへ変更)
	if (pInputKey->GetPress(DIK_T) == true)
	{
		m_rot.y = rotPlayer.y - D3DX_PI;
		m_rot.x = 0.0f; // 上下オフセットもリセット
	}

	int RValueH, RValueV;

	//アナログスティック右の処理-----------------------------------------------
	if (pInputJoypad->GetStickRight(&RValueH, &RValueV))
	{
		float rotSpeed = 0.0f;
		if (RValueH >= 20000.0f)
		{
			rotSpeed = 0.03f;
		}
		else if (RValueH >= 10000.0f)
		{
			rotSpeed = 0.015f;
		}
		else if (RValueH <= -20000.0f)
		{
			rotSpeed = -0.03f;
		}
		else if (RValueH <= -10000.0f)
		{
			rotSpeed = -0.015f;
		}

		//右スティックで上下始点操作-------------
		if (RValueV >= 5000.0f)
		{
			m_rot.x -= 3.0f;

			if (m_rot.x < -70.0f)
			{
				m_rot.x = -70.0f; // 上限制限
			}
		}
		else if (RValueV <= -5000.0f)
		{
			m_rot.x += 3.0f; 

			if (m_rot.x > 100.0f)
			{
				m_rot.x = 100.0f; // 下限制限
			}
		}

		if (rotSpeed != 0.0f)
		{
			m_rot.y += rotSpeed;

			//角度の正規化 (-PI ~ PI)
			if (m_rot.y <= -D3DX_PI)
			{
				m_rot.y += D3DX_PI * 2.0f;
			}
			else if (m_rot.y >= D3DX_PI)
			{
				m_rot.y -= D3DX_PI * 2.0f;
			}

			float offsetDist = (pInputKey->GetPress(DIK_W) || pInputKey->GetPress(DIK_A) ||
				pInputKey->GetPress(DIK_S) || pInputKey->GetPress(DIK_D)) ? 20.0f : 0.0f;

			m_posV.x = m_posR.x + sinf(m_rot.y + D3DX_PI) * (m_angle.z + offsetDist);
			m_posV.z = m_posR.z + cosf(m_rot.y + D3DX_PI) * (m_angle.z + offsetDist);
		}
	}

	//キーボード操作回転-------------------------------------------------------
	if (pInputKey->GetPress(DIK_RIGHT) == true || pInputKey->GetPress(DIK_RSHIFT) == true)
	{
		m_rot.y += 0.03f;

		if (m_rot.y >= D3DX_PI)
		{
			m_rot.y -= D3DX_PI * 2.0f;
		}

		float offsetDist = (pInputKey->GetPress(DIK_W) || pInputKey->GetPress(DIK_A) ||
			pInputKey->GetPress(DIK_S) || pInputKey->GetPress(DIK_D)) ? 20.0f : 0.0f;

		m_posV.x = m_posR.x + sinf(m_rot.y + D3DX_PI) * (m_angle.z + offsetDist);
		m_posV.z = m_posR.z + cosf(m_rot.y + D3DX_PI) * (m_angle.z + offsetDist);
	}

	if (pInputKey->GetPress(DIK_LEFT) == true || pInputKey->GetPress(DIK_LSHIFT) == true)
	{
		m_rot.y -= 0.03f;

		if (m_rot.y <= -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2.0f;
		}

		float offsetDist = (pInputKey->GetPress(DIK_W) || pInputKey->GetPress(DIK_A) ||
			pInputKey->GetPress(DIK_S) || pInputKey->GetPress(DIK_D)) ? 20.0f : 0.0f;

		m_posV.x = m_posR.x + sinf(m_rot.y + D3DX_PI) * (m_angle.z + offsetDist);
		m_posV.z = m_posR.z + cosf(m_rot.y + D3DX_PI) * (m_angle.z + offsetDist);
	}

	//注視点および視点の移動
	m_posR.x += (m_posRDest.x - m_posR.x) * 0.3f;
	m_posR.y += (m_posRDest.y - m_posR.y) * 0.3f;
	m_posR.z += (m_posRDest.z - m_posR.z) * 0.3f;

	m_posV.x += (m_posVDest.x - m_posV.x) * 0.3f;
	m_posV.y += (m_posVDest.y - m_posV.y) * 0.3f;
	m_posV.z += (m_posVDest.z - m_posV.z) * 0.3f;

	//壁判定・遮蔽物回避(レイキャスト)
	D3DXVECTOR3 rayStart = m_posR; //レイの始点(注視点)
	D3DXVECTOR3 rayEnd = m_posV;   //レイの終点(カメラ視点)
	D3DXVECTOR3 rayDir = rayEnd - rayStart;
	float fullDist = D3DXVec3Length(&rayDir);

	if (fullDist > 0.0001f)
	{
		D3DXVec3Normalize(&rayDir, &rayDir);
		float closestHitDist = fullDist;
		bool bHitAny = false;

		for (int nPriority = 0; nPriority < PRIORITY_MAX; nPriority++)
		{
			for (CObject* pObj = CObject::GetTop(nPriority); pObj != NULL; pObj = pObj->GetNext())
			{
				//ダウンキャスト
				CObjectX* pObjX = dynamic_cast<CObjectX*>(pObj);

				if (pObjX != NULL && pObjX->GetType() == CObjectX::TYPE_WALL)
				{
					LPD3DXMESH pMesh = pObjX->GetMesh();
					D3DXMATRIX mtxWorld = pObjX->GetWorldMatrix();

					float hitDist = 0.0f;
					if (CheckRayToCamera(pMesh, mtxWorld, rayStart, rayEnd, &hitDist))
					{//レイを使った判定
						if (hitDist < closestHitDist)
						{
							closestHitDist = hitDist;
							bHitAny = true;
						}
					}
				}
			}
		}

		if (bHitAny == true)
		{
			//壁とカメラの間のマージンを小さくして、極端に近づきすぎるのを防止
			float margin = 5.0f;
			float adjustedDist = max(20.0f, closestHitDist - margin); //最低距離を20.0fに保持
			m_posV = rayStart + rayDir * adjustedDist;
		}
	}

	//デバッグ表示
	if (pDebug != NULL)
	{
		pDebug->PrintDebugProc("\n********カメラの表示********\n");
		pDebug->PrintDebugProc("視点の座標[X:%.2f | Y:%.2f | Z:%.2f]\n", m_posV.x, m_posV.y, m_posV.z);
		pDebug->PrintDebugProc("視点の向き[X:%.2f | Y:%.2f | Z:%.2f]\n", m_rot.x, m_rot.y, m_rot.z);
		pDebug->PrintDebugProc("注視点の座標[X:%.2f | Y:%.2f | Z:%.2f]\n", m_posR.x, m_posR.y, m_posR.z);
	}
}

//=============================================================================
//設定
void CCamera::SetCamera(void)
{
	//レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;		//計算用マトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_rot.y, m_rot.x, m_rot.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_posV.x, m_posV.y, m_posV.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	//プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(60.0f),		//視野角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		10.0f,			//Z値の最小値
		5000.0f);		//Z値の最大値

	/*D3DXMatrixOrthoLH(&m_mtxProjection,
		(float)SCREEN_WIDTH,
		(float)SCREEN_HEIGHT,
		10.0f,
		2200.0f);*/

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	//ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_posU);

	//ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//=============================================================================
//全設定(初期化)
void CCamera::SetAll(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 posU, D3DXVECTOR3 rot)
{
	m_posV = posV;
	m_posR = posR;
	m_posU = posU;
	m_rot = rot;
}

//=============================================================================
//レイのカメラとメッシュの判定
bool CCamera::CheckRayToCamera(LPD3DXMESH pMesh, const D3DXMATRIX& mtxWorld, const D3DXVECTOR3& rayStart, const D3DXVECTOR3& rayEnd, float* pOutHitDist)
{
	if (pMesh == NULL)
	{
		return false;
	}

	//逆行列を算出(レイをメッシュのローカル座標系に変換)
	D3DXMATRIX mtxInvWorld;
	D3DXMatrixInverse(&mtxInvWorld, NULL, &mtxWorld);

	//レイの始点と終点localStartに変換する
	D3DXVECTOR3 localStart, localEnd;
	D3DXVec3TransformCoord(&localStart, &rayStart, &mtxInvWorld);
	D3DXVec3TransformCoord(&localEnd, &rayEnd, &mtxInvWorld);

	//方向ベクトルと距離を計算
	D3DXVECTOR3 localDir = localEnd - localStart;
	float maxDist = D3DXVec3Length(&localDir);
	if (maxDist < 0.0001f) return false;

	D3DXVec3Normalize(&localDir, &localDir);

	//交差判定用変数
	BOOL hit = FALSE;
	DWORD faceIndex = 0;
	FLOAT U = 0.0f, V = 0.0f, distance = 0.0f;

	//D3DXIntersectでメッシュとレイの当たり判定
	HRESULT hr = D3DXIntersect(
		pMesh,
		&localStart,
		&localDir,
		&hit,
		&faceIndex,
		&U,
		&V,
		&distance,
		NULL,
		NULL
	);

	if (SUCCEEDED(hr) && hit)
	{//正常に通っていたら
		//遮蔽物がレイの終点(カメラ位置)より手前にある場合
		if (distance < maxDist)
		{
			if (pOutHitDist)
			{
				*pOutHitDist = distance;
			}
			return true;
		}
	}

	return false;
}

//縦視点移動っぽい
/*m_posV.x -= sinf(D3DX_PI - m_rot.y) * 2;
		m_posR.x = sinf(D3DX_PI - m_rot.y) * 2;
		m_posV.z -= cosf(D3DX_PI - m_rot.y) * 2;
		m_posR.z += cosf(D3DX_PI - m_rot.y) * 2;*/

#if 0
//レイとメッシュの交差判定関数
bool CheckRayToCamera(
	LPD3DXMESH pMesh,                  //対象のメッシュ
	const D3DXVECTOR3& objPos,         //対象物の位置（始点）
	const D3DXVECTOR3& camPos          //カメラの位置
) {
	//レイの始点を対象物の位置にする
	D3DXVECTOR3 rayPos = objPos;

	//レイの方向を「対象物からカメラの方向」に計算する
	D3DXVECTOR3 rayDir;
	D3DXVECTOR3 rayDirUnnormalized = camPos - objPos;

	//ベクトルの長さを計算（距離の確認用など）
	float dist = D3DXVec3Length(&rayDirUnnormalized);
	if (dist < 0.0001f) return false; //距離が近すぎる場合は判定しない

	//方向を正規化する（単位ベクトルにする）
	D3DXVec3Normalize(&rayDir, &rayDirUnnormalized);

	//交差判定用の変数
	BOOL hit = FALSE;
	DWORD faceIndex = 0;
	FLOAT u = 0.0f, v = 0.0f, distance = 0.0f;
	LPD3DXBUFFER pAllHits = NULL;

	//D3DXIntersectでメッシュとレイの当たり判定を行う
	HRESULT hr = D3DXIntersect(
		pMesh,              // 当たり判定をするメッシュ
		&rayPos,            // レイの始点
		&rayDir,            // レイの方向
		&hit,               // ヒットしたかどうか
		&faceIndex,         // ヒットしたポリゴンの番号
		&u,                 // バリセントリック座標 U
		&v,                 // バリセントリック座標 V
		&distance,          // 始点からの距離
		&pAllHits,          // すべての交差（使わない場合はNULL）
		NULL                // ヒット数（使わない場合はNULL）
	);

	if (SUCCEEDED(hr) && hit) {
		// カメラに届くまでに障害物（メッシュのポリゴン）があるか確認
		// distance がカメラまでの距離 (dist) より小さければ、途中で遮られている
		if (distance <= dist) {
			return true; // 遮られている
		}
	}

	return false; // 遮られていない
}

#endif