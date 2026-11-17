//=============================================================================
//
// レンダラー[renderer.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _RENDERER_H_			//このマクロが定義されてなかったら
#define _RENDERER_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"

//前方宣言---------------------------------------------------------------------
class CBG;

//レンダラークラス-------------------------------------------------------------
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//デバイスの取得
	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pDevice; }

	void ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU);
	LPDIRECT3DTEXTURE9 GetTextureMT(void) { return m_pTextureMT; }
private:
	LPDIRECT3D9 m_pD3D;
	LPDIRECT3DDEVICE9 m_pDevice;
	static CBG* m_pBG;

	LPDIRECT3DTEXTURE9 m_pTextureMT;
	LPDIRECT3DSURFACE9 m_pRenderMT;
	LPDIRECT3DSURFACE9 m_pZBuffMT;
	D3DVIEWPORT9 m_viewportMT;
};

#endif 