//=============================================================================
//
// 2Dオブジェクト[object2D.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _OBJECT2D_H_			//このマクロが定義されてなかったら
#define _OBJECT2D_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//オブジェクト2Dクラス---------------------------------------------------------
class CObject2D :public CObject
{
public:
	CObject2D(int nPriority = 3);
	~CObject2D();

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 pos);
	void SetColor(D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	void BindTexture(const LPDIRECT3DTEXTURE9 pTexture);
	LPDIRECT3DTEXTURE9 GetTexture(void) { return m_pTexture; }
private:
	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
};

#endif