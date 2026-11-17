//=============================================================================
// 
//	ポーズ処理[pause.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "pause.h"

#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "texture.h"
#include "sound.h"

#include "scene.h"
#include "game.h"
#include "fade.h"

//=============================================================================
//コンストラクタ
CPause::CPause()
{
	//値をクリア
	m_pVtxBuff = NULL;
	m_PauseMenu = P_CONTINUE;
	m_nStickCounter = 0;
	memset(m_nIdxTexture, 0, sizeof(m_nIdxTexture));
	m_nStickCounter = 50;
}

//=============================================================================
//デストラクタ
CPause::~CPause()
{

}

//=============================================================================
//ポーズの初期化処理
void CPause::Init(void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//テクスチャの取得
	CTexture *pTexture = CManager::GetTexture();

	m_nStickCounter = 50;		//50に設定

	m_nIdxTexture[0] = pTexture->Register("data\\TEXTURE\\pause000.png");
	m_nIdxTexture[1] = pTexture->Register("data\\TEXTURE\\pause001.png");
	m_nIdxTexture[2] = pTexture->Register("data\\TEXTURE\\pause002.png");

	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_MENU,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;		//頂点情報へのポインタ

	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**) &pVtx, 0);

	for (int nCnt = 0; nCnt < MAX_MENU; nCnt++)
	{
		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(580.0f, 200.0f + (nCnt * 100), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(780.0f, 200.0f + (nCnt * 100), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(580.0f, 250.0f + (nCnt * 100), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(780.0f, 250.0f + (nCnt * 100), 0.0f);

		//rhw
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

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

		pVtx += 4;			//頂点データのポインタを4つ分集める
	}

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();

	m_PauseMenu = P_CONTINUE;
}

//=============================================================================
//ポーズの描画
void CPause::Draw(void)
{
	//デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTexture = 0; nCntTexture < 3; nCntTexture++)
	{
		pDevice->SetTexture(0, pTexture->GetAddres(m_nIdxTexture[nCntTexture]));

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTexture * 4, 2);
	}
}

//=============================================================================
//ポーズの終了処理
void CPause::Uninit(void)
{
	//頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//=============================================================================
//ポーズの更新処理
void CPause::Update(void)
{
	VERTEX_2D* pVtx;		//頂点情報へのポインタ

	//シーン取得、モードを取得
	CScene* pScene = CManager::GetScene();
	CScene::MODE mode = pScene->GetMode();

	//フェード取得
	CFade* pFade = CManager::GetFade();

	//入力系の取得
	CInputKeyboard *pInputKey = CManager::GetInputKeyboard();
	CInputJoypad*pInputJoy = CManager::GetInputJoypad();

	int nResult = m_PauseMenu;

	bool Pause = CManager::GetPause();

	if (pInputJoy != NULL && pInputKey != NULL && Pause == true)
	{//キーボードのポーズ操作
		if (pInputKey->GetTrigger(DIK_W) == true)
		{
			nResult--;

			if (nResult <= -1)
			{
				nResult = P_QUIT;
			}
		}

		if (pInputKey->GetTrigger(DIK_S) == true)
		{
			nResult++;

			if (nResult >= MAX_MENU)
			{
				nResult = P_CONTINUE;
			}
		}

		int LValueH = 0, LValueV = 0;

		if (pInputJoy->GetStickLeft(&LValueH, &LValueV))
		{//ジョイパットのポーズ操作
			if (LValueV >= 5000.0f)
			{
				m_nStickCounter++;
				if (m_nStickCounter > 50)
				{
					m_nStickCounter = 0;
					nResult--;
				}
				if (nResult <= -1)
				{
					nResult = P_QUIT;
				}
			}
			else if (LValueV <= -5000.0f)
			{
				m_nStickCounter++;
				if (m_nStickCounter > 50)
				{
					m_nStickCounter = 0;
					nResult++;
				}

				if (nResult >= MAX_MENU)
				{
					nResult = P_CONTINUE;
				}
			}
		}
		if (pInputJoy->GetStickLeft(&LValueH, &LValueV) == false)
		{
			m_nStickCounter = 50;
		}

		if (pInputJoy->GetTrigger(JOYKEY_UP) == true)
		{
			nResult--;

			if (nResult <= -1)
			{
				nResult = P_QUIT;
			}
		}

		if (pInputJoy->GetTrigger(JOYKEY_DOWN) == true)
		{
			nResult++;

			if (nResult >= MAX_MENU)
			{
				nResult = P_CONTINUE;
			}
		}

		if (pInputKey->GetTrigger(DIK_P) == true)
		{
			nResult = P_CONTINUE;
		}

		if (pInputJoy->GetTrigger(JOYKEY_START) == true)
		{
			nResult = P_CONTINUE;
		}

		m_PauseMenu = (PAUSE_MENU)nResult;

		if (pInputKey->GetTrigger(DIK_RETURN) == true || pInputJoy->GetTrigger(JOYKEY_A) == true)
		{
			//PlaySound(SOUND_LABEL_SE_CHECK);

			if (m_PauseMenu == P_CONTINUE)
			{
				Pause = false;
				CManager::SetPause(Pause);
			}
			else if (m_PauseMenu == P_RETRY && pScene->GetMode() == CScene::MODE_TUTORIAL)
			{//チュートリアル中だったら
				pFade->SetFade(CScene::MODE_TUTORIAL);

				Pause = false;
				CManager::SetPause(Pause);
				//*Stop = true;
			}
			else if (m_PauseMenu == P_RETRY && pScene->GetMode() == CScene::MODE_GAME)
			{//ゲーム中だったら
				pFade->SetFade(CScene::MODE_GAME);

				Pause = false;
				CManager::SetPause(Pause);
				//*Stop = true;
			}
			else if (m_PauseMenu == P_QUIT)
			{
				pFade->SetFade(CScene::MODE_TITLE);

				Pause = false;
				CManager::SetPause(Pause);
				//*Stop = true;
			}
		}
	}
	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < MAX_MENU; nCnt++)
	{
		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(580.0f, 200.0f + (nCnt * 100), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(780.0f, 200.0f + (nCnt * 100), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(580.0f, 250.0f + (nCnt * 100), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(780.0f, 250.0f + (nCnt * 100), 0.0f);

		if (nCnt == (int)m_PauseMenu)
		{
			//頂点カラー
			pVtx[0].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			//頂点カラー
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}

		//テクスチャ座標
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;			//頂点データのポインタを4つ分集める
	}

	//頂点バッファのアンロック
	m_pVtxBuff->Unlock();
}