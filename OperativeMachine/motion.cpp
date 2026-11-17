//=============================================================================
// 
// モーション[motion.h]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "motion.h"

#include "manager.h"
#include "debugproc.h"

#include "model.h"

//静的メンバ変数宣言-----------------------------------------------------------
bool CMotion::m_bFinish = false;

//=============================================================================
//コンストラクタ
CMotion::CMotion()
{
	//値をクリア(初期化)
	memset(&m_aInfo[0], 0, sizeof(m_aInfo));
	memset(&m_offsetPos[0], 0, sizeof(m_offsetPos));
	memset(&m_offsetRot[0], 0, sizeof(m_offsetRot));
	m_ppModel = NULL;
	m_nNumAll = 0;
	m_nNumModel = 0;
	m_nType = -1;
	m_bLoop = false;
	m_nNumKey = 0;
	m_nKey = 0;
	m_nCounter = 0;

	//モーションブレンド用メンバ変数の初期化
	m_bBlending = false;
	m_nBlendCounter = 0;
	m_nBlendFrame = 0;
	memset(m_aBlendStartPos, 0, sizeof(m_aBlendStartPos));
	memset(m_aBlendStartRot, 0, sizeof(m_aBlendStartRot));
}

//=============================================================================
//デストラクタ
CMotion::~CMotion()
{
}

//=============================================================================
//モデルの設定処理
void CMotion::SetModel(CModel** ppModel, int nNumModel)
{
	m_ppModel = ppModel;
	m_nNumModel = nNumModel;
}

//=============================================================================
//モーションの設定処理
void CMotion::SetInfo(INFO info)
{
	m_aInfo[m_nNumAll] = info;
	m_nNumAll++;
}

//=============================================================================
//設定処理（第2引数でブレンドフレーム数を指定可能に拡張：デフォルト値などをヘッダー側で設定可）
void CMotion::Set(int nType, int nBlendFrame)
{
	if (m_nType != nType)
	{
		//現在モデルの位置・回転をブレンド開始状態として保存（初回のSetでない場合かつブレンド指定時）
		if (m_nType != -1 && nBlendFrame > 0 && m_ppModel != NULL)
		{
			for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
			{
				m_aBlendStartPos[nCntModel] = m_ppModel[nCntModel]->GetPosition();
				m_aBlendStartRot[nCntModel] = m_ppModel[nCntModel]->GetRotation();
			}
			m_bBlending = true;
			m_nBlendCounter = 0;
			m_nBlendFrame = nBlendFrame;
		}
		else
		{
			m_bBlending = false;
		}

		//タイプを代入
		m_nType = nType;
		m_nNumKey = m_aInfo[nType].nNumKey;
		m_bLoop = m_aInfo[nType].bLoop;
		m_nKey = 0;
		m_nCounter = 0;
		m_bFinish = false;
	}

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{//モデルの数分
		m_offsetPos[nCntModel] = m_ppModel[nCntModel]->GetOffSetPos();
		m_offsetRot[nCntModel] = m_ppModel[nCntModel]->GetOffSetRot();

		//ブレンドしない場合のみ即時更新
		if (!m_bBlending)
		{
			//パーツの位置・向き設定
			m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX, m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ, m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ));
			m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX, m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY, m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ));
		}
	}
}

//=============================================================================
//更新処理
void CMotion::Update(void)
{
	//デバッグを取得
	CDebugproc* pDebug = CManager::GetDebugproc();

	float fDiffKey, fRateKey;
	float fPosX, fPosY, fPosZ, fRotX, fRotY, fRotZ;

	//各初期化
	fDiffKey = 0.0f;
	fRateKey = 0.0f;
	fPosX = 0.0f;
	fPosY = 0.0f;
	fPosZ = 0.0f;
	fRotX = 0.0f;
	fRotY = 0.0f;
	fRotZ = 0.0f;

	// ------------------------------------------------------------------------
	// ブレンド処理判定
	// ------------------------------------------------------------------------
	if (m_bBlending)
	{
		float fBlendRate = (float)m_nBlendCounter / (float)m_nBlendFrame;

		for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
		{
			// 遷移先のキーフレーム0の目標値
			float fTargetPosX = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fPosX;
			float fTargetPosY = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fPosY;
			float fTargetPosZ = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fPosZ;
			float fTargetRotX = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fRotX;
			float fTargetRotY = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fRotY;
			float fTargetRotZ = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fRotZ;

			// 位置の補間
			fPosX = m_aBlendStartPos[nCntModel].x + (fTargetPosX - m_aBlendStartPos[nCntModel].x) * fBlendRate;
			fPosY = m_aBlendStartPos[nCntModel].y + (fTargetPosY - m_aBlendStartPos[nCntModel].y) * fBlendRate;
			fPosZ = m_aBlendStartPos[nCntModel].z + (fTargetPosZ - m_aBlendStartPos[nCntModel].z) * fBlendRate;

			// 回転の補間（最短距離の差分を取る）
			float fDiffRotX = Normalize(fTargetRotX - m_aBlendStartRot[nCntModel].x);
			float fDiffRotY = Normalize(fTargetRotY - m_aBlendStartRot[nCntModel].y);
			float fDiffRotZ = Normalize(fTargetRotZ - m_aBlendStartRot[nCntModel].z);

			fRotX = m_aBlendStartRot[nCntModel].x + (fDiffRotX * fBlendRate);
			fRotY = m_aBlendStartRot[nCntModel].y + (fDiffRotY * fBlendRate);
			fRotZ = m_aBlendStartRot[nCntModel].z + (fDiffRotZ * fBlendRate);

			// 角度の正規化修正
			fRotX = Normalize(fRotX);
			fRotY = Normalize(fRotY);
			fRotZ = Normalize(fRotZ);

			// 位置・向きを設定
			m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));
			m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(fRotX, fRotY, fRotZ));

			// モデルの更新処理
			m_ppModel[nCntModel]->Update();
		}

		m_nBlendCounter++;

		// ブレンド終了チェック
		if (m_nBlendCounter >= m_nBlendFrame)
		{
			m_bBlending = false;
			m_nCounter = 0;
		}
		return; // ブレンド期間中は通常のキーフレーム進行をスキップ
	}

	// ------------------------------------------------------------------------
	// 普通のアニメーション
	fRateKey = ((float)m_nCounter) / ((float)m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame);
	int Next = (m_nKey + 1) % m_nNumKey;

	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		//fPosX
		fDiffKey = m_aInfo[m_nType].aKeyInfo[Next].aKey[nCntModel].fPosX - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX;
		fPosX = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX + (fDiffKey * fRateKey);

		//fPosY
		fDiffKey = m_aInfo[m_nType].aKeyInfo[Next].aKey[nCntModel].fPosY - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY;
		fPosY = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY + (fDiffKey * fRateKey);

		//fPosZ
		fDiffKey = m_aInfo[m_nType].aKeyInfo[Next].aKey[nCntModel].fPosZ - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ;
		fPosZ = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ + (fDiffKey * fRateKey);

		//fRotX
		fDiffKey = Normalize(m_aInfo[m_nType].aKeyInfo[Next].aKey[nCntModel].fRotX - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX);
		fRotX = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX + (fDiffKey * fRateKey);

		//pDebug->PrintDebugProc("fRotX[%d] : %.1f | ", nCntModel, (rotNormalize(fDiffKey * fRateKey)));

		//fRotY
		fDiffKey = Normalize(m_aInfo[m_nType].aKeyInfo[Next].aKey[nCntModel].fRotY - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY);
		fRotY = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY + (fDiffKey * fRateKey);

		//pDebug->PrintDebugProc("fRotY[%d] : %.1f | ", nCntModel, (rotNormalize(fDiffKey * fRateKey)));

		//fRotZ
		fDiffKey = Normalize(m_aInfo[m_nType].aKeyInfo[Next].aKey[nCntModel].fRotZ - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ);
		fRotZ = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ + (fDiffKey * fRateKey);

		//pDebug->PrintDebugProc("fRotZ[%d] : %.1f\n", nCntModel, (rotNormalize(fDiffKey * fRateKey)));

		//修正
		fRotX = Normalize(fRotX);
		fRotY = Normalize(fRotY);
		fRotZ = Normalize(fRotZ);

		//位置・向きを設定
		m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));
		m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(fRotX, fRotY, fRotZ));

		//モデルの更新処理
		m_ppModel[nCntModel]->Update();
	}
	m_nCounter++;

	if (m_aInfo[m_nType].bLoop == false)
	{
		if (m_nNumKey - 1 <= m_nKey)
		{
			m_bFinish = true;
		}
	}
	if (m_nCounter > m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
	{
		m_nKey++;

		m_nKey = m_nKey % m_nNumKey;

		m_nCounter = 0;
	}

	pDebug->PrintDebugProc("キー数:%d\n", m_nKey);
}

//=============================================================================
//モーション終了
bool CMotion::IsFinish(void)
{
	float fDiffKey, fRateKey;
	float fPosX, fPosY, fPosZ, fRotX, fRotY, fRotZ;

	//各初期化
	fDiffKey = 0.0f;
	fRateKey = 0.0f;
	fPosX = 0.0f;
	fPosY = 0.0f;
	fPosZ = 0.0f;
	fRotX = 0.0f;
	fRotY = 0.0f;
	fRotZ = 0.0f;

	if (m_bFinish == true)
	{
		fRateKey = ((float)m_nCounter) / ((float)m_aInfo[MOTIONTYPE_NEUTRAL].aKeyInfo[m_nKey].nFrame);
		int Next = (m_nKey + 1) % m_nNumKey;

		for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
		{
			//fPosX
			fDiffKey = m_aInfo[MOTIONTYPE_NEUTRAL].aKeyInfo[0].aKey[nCntModel].fPosX - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX;
			fPosX = m_aInfo[m_nType].aKeyInfo[0].aKey[nCntModel].fPosX + (Normalize(fDiffKey * fRateKey));

			//fPosY
			fDiffKey = m_aInfo[MOTIONTYPE_NEUTRAL].aKeyInfo[0].aKey[nCntModel].fPosY - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY;
			fPosY = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY + (Normalize(fDiffKey * fRateKey));

			//fPosZ
			fDiffKey = m_aInfo[MOTIONTYPE_NEUTRAL].aKeyInfo[0].aKey[nCntModel].fPosZ - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ;
			fPosZ = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ + (Normalize(fDiffKey * fRateKey));

			//fRotX
			fDiffKey = m_aInfo[MOTIONTYPE_NEUTRAL].aKeyInfo[0].aKey[nCntModel].fRotX - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX;
			fRotX = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX + (Normalize(fDiffKey * fRateKey));

			//fRotY
			fDiffKey = m_aInfo[MOTIONTYPE_NEUTRAL].aKeyInfo[0].aKey[nCntModel].fRotY - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY;
			fRotY = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY + (Normalize(fDiffKey * fRateKey));

			//fRotZ
			fDiffKey = m_aInfo[MOTIONTYPE_NEUTRAL].aKeyInfo[0].aKey[nCntModel].fRotZ - m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ;
			fRotZ = m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ + (Normalize(fDiffKey * fRateKey));

			//修正
			fRotX = Normalize(fRotX);
			fRotY = Normalize(fRotY);
			fRotZ = Normalize(fRotZ);

			//位置・向きを設定
			m_ppModel[nCntModel]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));
			m_ppModel[nCntModel]->SetRotation(D3DXVECTOR3(fRotX, fRotY, fRotZ));

			//モデルの更新処理
			m_ppModel[nCntModel]->Update();

			m_bFinish = false;

			return false;
		}
	}
	else
	{//普段はtrueを返す
		return true;
	}
	return NULL;
}

//==================================================================================
//	角度の正規化
float CMotion::Normalize(float fAngle)
{
	if (fAngle < -D3DX_PI)
	{
		fAngle += D3DX_PI * 2;
	}
	else if (fAngle > D3DX_PI)
	{
		fAngle -= D3DX_PI * 2;
	}

	return fAngle;
}