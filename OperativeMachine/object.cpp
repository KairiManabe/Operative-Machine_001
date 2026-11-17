//=============================================================================
//
//オブジェクト[object.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "object.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"

#include "camera.h"

//静的メンバ変数宣言-----------------------------------------------------------
CObject* CObject::m_pTop[PRIORITY_MAX] = {};
CObject* CObject::m_pCur[PRIORITY_MAX] = {};
CObject* CObject::m_apObject[PRIORITY_MAX][MAX_DATA] = {};
int CObject::m_nNumAll = 0;

//=============================================================================
//コンストラクタ
CObject::CObject(int nPriority)
{
	CObject* pObject = this;

	m_bDeath = false;
	m_nPriority = nPriority;
	m_pNext = NULL;
	m_pPrev = NULL;

	if (m_pTop[m_nPriority] == NULL)
	{
		//自分自身を代入
		m_pTop[m_nPriority] = pObject;

		m_pCur[m_nPriority] = m_pTop[m_nPriority];

		//総数を増やす
		m_nNumAll++;
	}
	else 
	{
		//自分自身を代入
		m_pCur[m_nPriority]->m_pNext = pObject;

		pObject->m_pPrev = m_pCur[m_nPriority];
		m_pCur[m_nPriority] = pObject;		//最後尾

		//総数を増やす
		m_nNumAll++;
	}

	m_nID = 0;
	m_type = {};

#if 0
	for (int nCount = 0; nCount < MAX_DATA; nCount++)
	{
		if (m_apObject[nPriority][nCount] == NULL)
		{
			//自分自身を代入
			m_apObject[nPriority][nCount] = this;

			m_nPriority = nPriority;

			//総数を増やす
			m_nNumAll++;

			//自分自身のIDを保存
			m_nID = nCount;

			break;
		}
	}
#endif
}

//=============================================================================
//デストラクタ
CObject::~CObject()
{
}

//=============================================================================
//全破棄
void CObject::ReleaseAll(void)
{
	for (int nCntPriority = 0; nCntPriority < PRIORITY_MAX; nCntPriority++)
	{
		CObject* pObject = m_pTop[nCntPriority];//先頭オブジェクトを代入

		while (pObject != NULL)
		{
			CObject* pObjectNext = pObject->m_pNext;	//次のオブジェクトを保存

			//終了処理
			pObject->Uninit();

			pObject = pObjectNext;
		}
	}

	for (int nCntPriority = 0; nCntPriority < PRIORITY_MAX; nCntPriority++)
	{
		CObject* pObject = m_pTop[nCntPriority];		//先頭オブジェクトを代入

		while (pObject != NULL)
		{
			CObject* pObjectNext = pObject->m_pNext;	//次のオブジェクトを保存

			if (pObject->m_bDeath == true)
			{
				// リストから外す
				if (pObject->m_pPrev)
				{
					pObject->m_pPrev->m_pNext = pObject->m_pNext;
				}
				if (pObject->m_pNext)
				{
					pObject->m_pNext->m_pPrev = pObject->m_pPrev;
				}

				// 先頭だった場合は更新
				if (m_pTop[nCntPriority] == pObject)
				{
					m_pTop[nCntPriority] = pObject->m_pNext;
				}

				//最後尾だった場合
				if (m_pCur[nCntPriority] == pObject)
				{
					m_pCur[nCntPriority] = pObject->m_pPrev;
				}

				delete pObject;
				pObject = NULL;
			}

			pObject = pObjectNext;
		}
	}

#if 0
	for (int nCntPriority = 0; nCntPriority < PRIORITY_MAX; nCntPriority++)
	{
		for (int nCntObject = 0; nCntObject < MAX_DATA; nCntObject++)
		{
			if (m_apObject[nCntPriority][nCntObject] != NULL)
			{//メモリが確保されているか	
				//敵の更新処理
				m_apObject[nCntPriority][nCntObject]->Uninit();
			}
		}
	}
#endif
}

//=============================================================================
//全更新
void CObject::UpdateAll(void)
{
	for (int nCntPriority = 0; nCntPriority < PRIORITY_MAX; nCntPriority++)
	{
		CObject* pObject = m_pTop[nCntPriority];		//先頭オブジェクトを代入

		while (pObject != NULL)
		{
			CObject* pObjectNext = pObject->m_pNext;	//次のオブジェクトを保存

			//更新処理
			pObject->Update();

			pObject = pObjectNext;
		}
	}

	for (int nCntPriority = 0; nCntPriority < PRIORITY_MAX; nCntPriority++)
	{
		CObject* pObject = m_pTop[nCntPriority];		//先頭オブジェクトを代入

		while (pObject != NULL)
		{
			CObject* pObjectNext = pObject->m_pNext;	//次のオブジェクトを保存

			if (pObject->m_bDeath == true)
			{
				// リストから外す
				if (pObject->m_pPrev)
				{
					pObject->m_pPrev->m_pNext = pObject->m_pNext;
				}
				if (pObject->m_pNext)
				{
					pObject->m_pNext->m_pPrev = pObject->m_pPrev;
				}

				// 先頭だった場合は更新
				if (m_pTop[nCntPriority] == pObject)
				{
					m_pTop[nCntPriority] = pObject->m_pNext;
				}

				//最後尾だった場合
				if (m_pCur[nCntPriority] == pObject)
				{
					m_pCur[nCntPriority] = pObject->m_pPrev;
				}

				delete pObject;
				pObject = NULL;
			}

			pObject = pObjectNext;
		}
	}

	CManager::GetDebugproc()->PrintDebugProc("オブジェクトの総数:%d", m_nNumAll);

#if 0
	for (int nCntPriority = 0; nCntPriority < PRIORITY_MAX; nCntPriority++)
	{
		for (int nCntObject = 0; nCntObject < MAX_DATA; nCntObject++)
		{
			if (m_apObject[nCntPriority][nCntObject] != NULL)
			{//メモリが確保されているか	
				//敵の更新処理
				m_apObject[nCntPriority][nCntObject]->Update();
			}
		}
	}
#endif
}

//=============================================================================
//全描画
void CObject::DrawAll(void)
{
	CCamera* pCamera = CManager::GetCamera();

	pCamera->SetCamera();

	for (int nCntPriority = 0; nCntPriority < PRIORITY_MAX; nCntPriority++)
	{
		CObject* pObject = m_pTop[nCntPriority];//先頭オブジェクトを代入

		while (pObject != NULL)
		{
			CObject* pObjectNext = pObject->m_pNext;	//次のオブジェクトを保存

			//描画処理
			pObject->Draw();

			pObject = pObjectNext;
		}
	}

#if 0
	for (int nCntPriority = 0; nCntPriority < PRIORITY_MAX; nCntPriority++)
	{
		for (int nCntObject = 0; nCntObject < MAX_DATA; nCntObject++)
		{
			if (m_apObject[nCntPriority][nCntObject] != NULL)
			{//メモリが確保されているか	
				//敵の描画処理
				m_apObject[nCntPriority][nCntObject]->Draw();
			}
		}
	}
#endif
}

//=============================================================================
//破棄
void CObject::Release(void)
{
#if 0
	if (m_apObject[m_nPriority][m_nID] != NULL)
	{
		m_apObject[m_nPriority][m_nID] = NULL;

		delete this;

		m_nNumAll--;
	}
#endif

	m_bDeath = true;

	//// リストから外す
	//if (m_pPrev)
	//{
	//	m_pPrev->m_pNext = m_pNext;
	//}
	//if (m_pNext)
	//{
	//	m_pNext->m_pPrev = m_pPrev;
	//}

	//// 先頭だった場合は更新
	//if (m_pTop[m_nPriority] == this)
	//{
	//	m_pTop[m_nPriority] = m_pNext;
	//}

	m_nNumAll--;

	//delete this;
}

//=============================================================================
//タイプの設定
void CObject::SetType(const TYPE type)
{
	m_type = type;
}