//=============================================================================
// 
// タイマー[timer.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "timer.h"

#include "manager.h"
#include "fade.h"
#include "color.h"

#include "game.h"

#include "number.h"
#include "score.h"

//=============================================================================
//コンストラクタオーバーロード
CTimer::CTimer(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	memset(&m_apNumber[0], 0, sizeof(m_apNumber));
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = COLOR_CYAN;
	m_nTimer = 0;
	m_nNumber = 0;
	m_nCounter = 60;
}

//=============================================================================
//デストラクタ
CTimer::~CTimer()
{
}

//=============================================================================
//タイマーの生成処理
CTimer* CTimer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col, int nInterval, int nNum, int nTimer)
{
	//プレイヤー
	CTimer* pTimer = NULL;

	if (pTimer == NULL)
	{
		//弾生成
		pTimer = new CTimer;

		if (pTimer != NULL)
		{
			//pos
			pTimer->m_pos = pos;

			//タイマーの数値を代入
			pTimer->m_nTimer = nTimer;

			//サイズを代入
			pTimer->m_size = size;

			//色を代入
			pTimer->m_col = col;

			//間隔を代入
			pTimer->m_nInterval = nInterval;

			//桁数をもらう
			pTimer->m_nNumber = nNum;

			pTimer->Init(pTimer->m_pos, D3DXVECTOR3(0, 0, 0));
		}
	}
	return pTimer;
}

//=============================================================================
//オブジェクトの初期化
HRESULT CTimer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	for (int nCntNum = 0; nCntNum < m_nNumber; nCntNum++)
	{//桁数分回す
		if (m_apNumber[nCntNum] == NULL)
		{//ナンバーを生成
			m_apNumber[nCntNum] = CNumber::Create(m_pos, m_size, m_nInterval, nCntNum);
		}
	}

	return S_OK;
}

//=============================================================================
//オブジェクトの終了
void CTimer::Uninit(void)
{
	for (int nCntNum = 0; nCntNum < m_nNumber; nCntNum++)
	{//桁数分回す
		m_apNumber[nCntNum]->Uninit();
	}
	CObject::Release();
}

//=============================================================================
//オブジェクトの更新
void CTimer::Update(void)
{
	//フェードを取得
	CFade* pFade = CManager::GetFade();

	int aTexU[NUM_TIMER];		//各桁の数字を格納
	
	m_nCounter--;
	if (m_nCounter == 0)
	{
		if (m_nTimer > 0)
		{
			//タイマーを進める
			m_nTimer--;
			
			/*if (m_nTimer % 10 == 0)
			{
				CEnemy::Create(D3DXVECTOR3(640.0f, 300.0f, 0.0f), SIZE_TARGET);
			}*/
		}

		if (m_nTimer == 10)
		{
			m_col = COLOR_RED;
		}

		if (m_nTimer <= 0)
		{
			pFade->SetFade(CScene::MODE_FAIL);
		}

		//初期値にする
		m_nCounter = 60;
	}

	aTexU[0] = m_nTimer % 10 / 1;
	aTexU[1] = m_nTimer % 60 / 10;
	aTexU[2] = 10;
	aTexU[3] = m_nTimer / 60;

	for (int nCntNum = 0; nCntNum < m_nNumber; nCntNum++)
	{
		m_apNumber[nCntNum]->SetNumber(aTexU[nCntNum]);
		m_apNumber[nCntNum]->SetColor(m_col);
		m_apNumber[nCntNum]->Update();
	}
}

//=============================================================================
//オブジェクトの描画
void CTimer::Draw(void)
{
	for (int nCntNum = 0; nCntNum < m_nNumber; nCntNum++)
	{//桁数分回す
		m_apNumber[nCntNum]->Draw();
	}
}
