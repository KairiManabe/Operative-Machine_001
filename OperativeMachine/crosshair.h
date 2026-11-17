//=============================================================================
//
// クロスヘアー[crosshair.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _CROSSHAIR_H_			//このマクロが定義されてなかったら
#define _CROSSHAIR_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//プレイヤークラス-------------------------------------------------------------
class CCrosshair :public CObject
{
public:
public:
	CCrosshair(int nPriority = 6);
	~CCrosshair();

	static CCrosshair* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetColor(D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXCOLOR GetColor(void) { return m_col; }
private:
	std::string m_filename;
	int m_nIdxTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
};

#endif