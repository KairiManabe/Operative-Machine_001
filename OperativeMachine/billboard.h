//=============================================================================
// 
// ビルボード[billboard.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _BILLBOARD_H_ 
#define _BILLBOARD_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_BILL			(100)	//ポリゴンサイズ
#define SIZE_BILL_Y			(150)	//ポリゴンサイズ
#define BILLBOARD_PRIORITY	(3)		//自分のプライオリティを持っておく

//ビルボードクラス-------------------------------------------------------------
class CBillboard : public CObject
{
public:
	CBillboard(int nPriority = BILLBOARD_PRIORITY);
	~CBillboard();

	static CBillboard* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetColor(D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetSize(void) { return m_size; }

	void BindTexture(const LPDIRECT3DTEXTURE9 pTexture);
private:
	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_size;
	D3DXCOLOR m_col;
	D3DXMATRIX m_mtxWorld;
};


#endif // !1

