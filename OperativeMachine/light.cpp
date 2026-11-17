//=============================================================================
// 
// ライト[light.cpp]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "light.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"

//=============================================================================
//コンストラクタ
CLight::CLight()
{
	//値をクリア(初期化)
	memset(&m_aLight[0], 0, sizeof(m_aLight));
}

//=============================================================================
//デストラクタ
CLight::~CLight()
{
}

//=============================================================================
//初期化
void CLight::Init(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXVECTOR3 vecDir[MAX_LIGHT];			//ライトの方向ベクトル

	//ライトの情報をクリアする
	ZeroMemory(&m_aLight[0], sizeof(D3DLIGHT9));

	//ライトの種類を設定
	m_aLight[0].Type = D3DLIGHT_DIRECTIONAL;

	//ライトの拡散光を設定
	m_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//ライトの方向を設定
	vecDir[0] = D3DXVECTOR3(0.22f, -0.87f, 0.44f);
	D3DXVec3Normalize(&vecDir[0], &vecDir[0]);		//ベクトルを正規化
	m_aLight[0].Direction = vecDir[0];

	//ライトを設定
	pDevice->SetLight(0, &m_aLight[0]);

	//ライトを有効にする
	pDevice->LightEnable(0, TRUE);

	//ライトの情報をクリアする
	ZeroMemory(&m_aLight[1], sizeof(D3DLIGHT9));

	//ライトの種類を設定
	m_aLight[1].Type = D3DLIGHT_DIRECTIONAL;

	//ライトの拡散光を設定
	m_aLight[1].Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);

	//ライトの方向を設定
	vecDir[1] = D3DXVECTOR3(-0.18f, 0.88f, -0.44f);
	D3DXVec3Normalize(&vecDir[1], &vecDir[1]);		//ベクトルを正規化
	m_aLight[1].Direction = vecDir[1];

	//ライトを設定
	pDevice->SetLight(1, &m_aLight[1]);

	//ライトを有効にする
	pDevice->LightEnable(1, TRUE);

	//ライトの情報をクリアする
	ZeroMemory(&m_aLight[2], sizeof(D3DLIGHT9));

	//ライトの種類を設定
	m_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	//ライトの拡散光を設定
	m_aLight[2].Diffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);

	//ライトの方向を設定
	vecDir[2] = D3DXVECTOR3(0.89f, 0.11f, 0.44f);
	D3DXVec3Normalize(&vecDir[2], &vecDir[2]);		//ベクトルを正規化
	m_aLight[2].Direction = vecDir[2];

	//ライトを設定
	pDevice->SetLight(2, &m_aLight[2]);

	//ライトを有効にする
	pDevice->LightEnable(2, TRUE);

	//ライトの情報をクリアする
	ZeroMemory(&m_aLight[3], sizeof(D3DLIGHT9));

	//ライトの種類を設定
	m_aLight[3].Type = D3DLIGHT_DIRECTIONAL;

	//ライトの拡散光を設定
	m_aLight[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//ライトの方向を設定
	vecDir[3] = D3DXVECTOR3(-0.22f, -0.87f, -0.44f);
	D3DXVec3Normalize(&vecDir[3], &vecDir[3]);		//ベクトルを正規化
	m_aLight[3].Direction = vecDir[3];

	//ライトを設定
	pDevice->SetLight(3, &m_aLight[3]);

	//ライトを有効にする
	pDevice->LightEnable(3, TRUE);
}

//=============================================================================
//
void CLight::Uninit(void)
{

}

//=============================================================================
//
void CLight::Update(void)
{
	D3DXVECTOR3 vecDir;

	CCamera* pCamera = CManager::GetCamera();
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//ライトの情報をクリアする
	ZeroMemory(&m_aLight[0], sizeof(D3DLIGHT9));

	//ライトの種類を設定
	m_aLight[0].Type = D3DLIGHT_DIRECTIONAL;

	//ライトの拡散光を設定
	m_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//ライトの方向を設定
	vecDir = D3DXVECTOR3(0.22f, -0.87f, 0.44f);
	D3DXVec3Normalize(&vecDir, &vecDir);		//ベクトルを正規化
	m_aLight[0].Direction = vecDir;

	//ライトを設定
	pDevice->SetLight(0, &m_aLight[0]);

	//ライトを有効にする
	pDevice->LightEnable(0, TRUE);
}