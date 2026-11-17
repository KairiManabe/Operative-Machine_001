//=============================================================================
// 
// スコア[score.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "score.h"
#include "number.h"
#include "color.h"
#include "manager.h"
#include "scene.h"
#include "game.h"
#include "title.h"

//=============================================================================
//コンストラクタオーバーロード
CScore::CScore(int nPriority) :CObject(nPriority)
{
	//値をクリア(初期化)
	memset(&m_apNumber[0], 0, sizeof(m_apNumber));
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = COLOR_THIN_ORANGE;
	m_nScore = 0;
	m_nNumber = 0;
}

//=============================================================================
//デストラクタ
CScore::~CScore()
{
}

//=============================================================================
//タイマーの生成処理
CScore* CScore::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col, int nInterval, int nNum)
{
	//プレイヤー
	CScore* pScore = NULL;

	if (pScore == NULL)
	{
		//弾生成
		pScore = new CScore;

		if (pScore != NULL)
		{
			//pos
			pScore->m_pos = pos;

			//サイズを代入
			pScore->m_size = size;

			//色を代入
			pScore->m_col = col;

			//間隔を代入
			pScore->m_nInterval = nInterval;

			//桁数をもらう
			pScore->m_nNumber = nNum;

			//スコアの初期化処理
			pScore->Init(pScore->m_pos, D3DXVECTOR3(0, 0, 0));
		}
	}
	return pScore;
}

//=============================================================================
//オブジェクトの初期化
HRESULT CScore::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
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
void CScore::Uninit(void)
{
	for (int nCntNum = 0; nCntNum < m_nNumber; nCntNum++)
	{//桁数分回す
		m_apNumber[nCntNum]->Uninit();
	}
	CObject::Release();
}

//=============================================================================
//オブジェクトの更新
void CScore::Update(void)
{
	int aTexU[NUM_SCORE];		//各桁の数字を格納

	aTexU[0] = m_nScore % 100000000 / 10000000;
	aTexU[1] = m_nScore % 10000000 / 1000000;
	aTexU[2] = m_nScore % 1000000 / 100000;
	aTexU[3] = m_nScore % 100000 / 10000;
	aTexU[4] = m_nScore % 10000 / 1000;
	aTexU[5] = m_nScore % 1000 / 100;
	aTexU[6] = m_nScore % 100 / 10;
	aTexU[7] = m_nScore % 10 / 1;

	for (int nCntNum = 0; nCntNum < m_nNumber; nCntNum++)
	{
		m_apNumber[nCntNum]->SetPosition(m_pos);
		m_apNumber[nCntNum]->SetSize(m_size, m_nInterval);
		m_apNumber[nCntNum]->SetNumber(aTexU[nCntNum]);
		m_apNumber[nCntNum]->SetColor(m_col);
		m_apNumber[nCntNum]->Update();
	}
}

//=============================================================================
//オブジェクトの描画
void CScore::Draw(void)
{
	for (int nCntNum = 0; nCntNum < m_nNumber; nCntNum++)
	{//桁数分回す
		m_apNumber[nCntNum]->Draw();
	}
}

//=============================================================================
//スコアの設定処理
void CScore::AddScore(int nScore)
{
	m_nScore += nScore;
}