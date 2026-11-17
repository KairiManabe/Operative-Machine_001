//=============================================================================
//
//弾bullet.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "bullet.h"
#include "game.h"
#include "tutorial.h"
#include "manager.h"
#include "input.h"
#include "renderer.h"
#include "score.h"
#include "texture.h"
#include "target.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CBullet::m_nIdxTexture = -1;

//=============================================================================
//コンストラクタ
CBullet::CBullet(void)
{
	//値をクリア(初期化)
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_nLife = 0;
}

//=============================================================================
//デストラクタ
CBullet::~CBullet()
{
}

//=============================================================================
//弾の読み込み
HRESULT CBullet::Load(void)
{
    CTexture* pTexture = CManager::GetTexture();
    if (pTexture == NULL)
    {
        return E_FAIL;
    }

    m_nIdxTexture = pTexture->Register("data/TEXTURE/bullet000.png");
    return (m_nIdxTexture >= 0) ? S_OK : E_FAIL;
}

//=============================================================================
//アンロード
void CBullet::Unload(void)
{
    m_nIdxTexture = -1;
}

//=============================================================================
//弾の生成処理
CBullet* CBullet::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 move, const int nLife)
{
    CBullet* pBullet = new CBullet;
    if (pBullet == NULL)
    {
        return NULL;
    }

    pBullet->m_pos = pos;
    pBullet->m_size = size;
    pBullet->m_move = move;
    pBullet->m_nLife = nLife;

    if (FAILED(pBullet->Init(pos, size)))
    {
        pBullet->Uninit();
        return NULL;
    }
    return pBullet;
}

//=============================================================================
//弾の初期化
HRESULT CBullet::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
    SetType(TYPE_BULLET);

    //ビルボードの初期化
    HRESULT hr = CBillboard::Init(pos, size);
    if (FAILED(hr))
    {
        return hr;
    }

    CTexture* pTexture = CManager::GetTexture();
    if (pTexture == NULL || m_nIdxTexture < 0)
    {
        return E_FAIL;
    }
    BindTexture(pTexture->GetAddres(m_nIdxTexture));
    return S_OK;
}

//=============================================================================
//弾の終了
void CBullet::Uninit(void)
{
    CBillboard::Uninit();
}

//=============================================================================
//弾の描画
void CBullet::Draw(void)
{
    CBillboard::Draw();
}

//=============================================================================
//弾の更新
void CBullet::Update(void)
{
    if (m_nLife <= 0)
    {
        Uninit();
        return;
    }

    const D3DXVECTOR3 previousPos = m_pos;
    m_pos += m_move;
    SetPosition(m_pos);
    --m_nLife;

    const float radius = (m_size.x > m_size.y) ? m_size.x : m_size.y;

    CollisionHuman(previousPos, m_pos, radius);

    if (m_nLife <= 0)
    {//ライフがないときに消す
        Uninit();
    }
}

//=============================================================================
//ターゲットとの当たり判定
bool CBullet::CollisionHuman(const D3DXVECTOR3& start, const D3DXVECTOR3& end, float radius)
{
    CObject* pObject = CObject::GetTop(4);
    while (pObject != NULL)
    {
        CObject* pNext = pObject->GetNext();
        CTarget* pHuman = dynamic_cast<CTarget*>(pObject);
        if (pHuman != NULL && pHuman->CollisionBullet(start, end, radius))
        {
            pHuman->DethHuman();
            return true;
        }
        pObject = pNext;
    }
    return false;
}
