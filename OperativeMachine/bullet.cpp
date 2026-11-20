//=============================================================================
//
// 弾の出力処理[bullet.cpp]
// 
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "bullet.h"

#include "manager.h"
#include "renderer.h"
#include "debugproc.h"
#include "input.h"
#include "texture.h"
#include "color.h"

#include "game.h"
#include "tutorial.h"

#include "score.h"
#include "target.h"
#include "effect.h"
#include "wall.h"

//静的メンバ変数宣言-----------------------------------------------------------
int CBullet::m_nIdxTexture = -1;
float CBullet::m_fCollisionSize = 10.0f;

//=============================================================================
//コンストラクタ
CBullet::CBullet(void)
{
	//値をクリア(初期化)
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_straightMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fDistanceToStraight = 0.0f;
	m_bHasStraightMove = false;
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
CBullet* CBullet::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 move, const int nLife, const D3DXVECTOR3& straightMove, float distanceToStraight)
{
    CBullet* pBullet = new CBullet;
    if (pBullet == NULL)
    {
        return NULL;
    }

    pBullet->m_pos = pos;
    pBullet->m_size = size;
    pBullet->m_move = move;
    pBullet->m_straightMove = straightMove;
    pBullet->m_fDistanceToStraight = distanceToStraight;
    pBullet->m_bHasStraightMove = distanceToStraight > 0.0f && D3DXVec3LengthSq(&straightMove) > 0.0001f;
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

    CWall **pWall = (CManager::GetScene()->GetMode() == CScene::MODE_GAME) ? CGame::GetWall() : CTutorial::GetWall();
    
    //発射位置から照準レイ上の点まで飛ばし、到達後は照準レイに沿って直進させる
    //-----------------------------------------------------------------------------------
    //クロスヘアからまっすぐ飛ばす処理
    const D3DXVECTOR3 previousPos = m_pos;
    D3DXVECTOR3 frameMove = m_move;
    const float speed = D3DXVec3Length(&m_move);
    if (m_bHasStraightMove && speed > 0.0001f)
    {
        if (m_fDistanceToStraight <= speed)
        {//合流点に到達したら、残りの移動量を照準レイ方向へ進める
            D3DXVECTOR3 approachDirection = m_move / speed;
            D3DXVECTOR3 straightDirection = m_straightMove;
            D3DXVec3Normalize(&straightDirection, &straightDirection);
            const float approachDistance = m_fDistanceToStraight;
            frameMove = approachDirection * approachDistance
                + straightDirection * (speed - approachDistance);
            m_move = m_straightMove;
            m_bHasStraightMove = false;
        }
        else
        {
            m_fDistanceToStraight -= speed;
        }
    }
    m_pos += frameMove;
    SetPosition(m_pos);
    m_nLife--;

    CEffect::Create(CEffect::TEX_DIAMOND, m_pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(7.0f, 50.0f, 0.0f), COLOR_NORMAL, 10);

    const float radius = (m_size.x > m_size.y) ? m_size.x : m_size.y;
    CollisionTarget(previousPos, m_pos, radius);

    for (int nCntWall = 0; nCntWall < CWall::GetNumAll(); nCntWall++)
    {
        if (pWall[nCntWall]->CollisionBullet(this) == true)
        {
            Uninit();
        }
    }

    CManager::GetDebugproc()->PrintDebugProc("\n弾のpos[X:%.2f | Y:%.2f | Z:%.2f]\n", m_pos.x, m_pos.y, m_pos.z);
}

//=============================================================================
//ターゲットとの当たり判定
bool CBullet::CollisionTarget(const D3DXVECTOR3& start, const D3DXVECTOR3& end, float radius)
{
    //TARGET_PRIORITY→ターゲットクラスのプライオリティ
    CObject* pObject = CObject::GetTop(TARGET_PRIORITY);
    while (pObject != NULL)
    {
        CObject* pNext = pObject->GetNext();
        CTarget* pTarget = dynamic_cast<CTarget*>(pObject);
        if (pTarget != NULL && pTarget->CollisionBullet(start, end, radius))
        {
            pTarget->DethTarget();
            return true;
        }
        pObject = pNext;
    }
    return false;
}
