//=============================================================================
// 
// テクスチャの管理[texture.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "texture.h"
#include "renderer.h"
#include "manager.h"

//静的メンバ変数宣言-----------------------------------------------------------


//=============================================================================
//コンストラクタ
CTexture::CTexture()
{
	//値をクリア(初期化)
	memset(&m_apTexture[0], 0, sizeof(m_apTexture));
	memset(&m_aFilename[0][0], 0, sizeof(m_aFilename));
	 m_nNumAll = 0;
}

//=============================================================================
//デストラクタ
CTexture::~CTexture()
{
}


//=============================================================================
//既存のテクスチャの読み込み
HRESULT CTexture::Load(void)
{
	return S_OK;
}

//=============================================================================
//すべてのテクスチャ破棄
void CTexture::Unload(void)
{
	for (int nCntTex = 0; nCntTex < MAX_TEXTURE; ++nCntTex)
	{
		if (m_apTexture[nCntTex] != NULL)
		{
			m_apTexture[nCntTex]->Release();
			m_apTexture[nCntTex] = NULL;
		}
	}
    memset(&m_aFilename[0][0], 0, sizeof(m_aFilename));
	m_nNumAll = 0;
}

//=============================================================================
//テクスチャの登録処理
int CTexture::Register(const char* pFilename)
{
    CRenderer* pRenderer = CManager::GetRenderer();
    if (pRenderer == NULL || pFilename == NULL)
    {
        return -1;
    }
    LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();
    if (pDevice == NULL)
    {
        return -1;
    }

    if (pFilename == NULL)
    {//テクスチャが存在しない
        return -1;
    }

    //すでに同じファイル名のテクスチャが読み込まれているかチェック
    for (int nCntIdx = 0; nCntIdx < MAX_TEXTURE; nCntIdx++)
    {
        if (m_apTexture[nCntIdx] != NULL)
        {
            //文字列を比較して同じファイル名なら既存のインデックスを返す
            if (strcmp(m_aFilename[nCntIdx], pFilename) == 0)
            {
                return nCntIdx;
            }
        }
    }

    //空きスロットを探して新しく読み込む
    for (int nCntIdx = 0; nCntIdx < MAX_TEXTURE; nCntIdx++)
    {
        if (m_apTexture[nCntIdx] == NULL)
        {
            //テクスチャの読み込み
            if (FAILED(D3DXCreateTextureFromFile(pDevice, pFilename, &m_apTexture[nCntIdx])))
            {
                return -1; //読み込み失敗
            }

            //ファイル名を保存
            strncpy_s(m_aFilename[nCntIdx], sizeof(m_aFilename[nCntIdx]), pFilename, _TRUNCATE);

            m_nNumAll++;        //総数を増やす

            return nCntIdx;     //idxを返す
        }
    }

    return -1; //空きがない場合
}

//=============================================================================
// アドレス取得
LPDIRECT3DTEXTURE9 CTexture::GetAddres(int nIdx)
{
    if (nIdx < 0 || nIdx >= MAX_TEXTURE)
    {//インデックスが-1(テクスチャが存在しない時)はNULLを返す
        return NULL;
    }

	return m_apTexture[nIdx];
}