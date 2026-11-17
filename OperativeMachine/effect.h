//===================================================================
// 
// エフェクト処理[effect.h]
// 
// Author : 眞鍋快陸
// 
//===================================================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

//インクルードファイル-----------------------------------------------
#include "main.h"
#include "object.h"
#include "billboard.h"

//マクロ定義---------------------------------------------------------
#define MAX_EFFECT	(1600)

//エフェクトクラス（3D仕様）-----------------------------------------
class CEffect : public CBillboard
{
public:
	CEffect(int nPriority = 3);
	~CEffect();

	typedef enum
	{
		TEX_NONE = 0,
		TEX_CIRCLE,
		TEX_DIAMOND,
		TEX_MAX
	}EFFECT_TEX;

	static HRESULT Load(void);
	static void Unload(void);
	static CEffect* Create(EFFECT_TEX tex, D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXVECTOR3 size, D3DXCOLOR col, int nLife);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetTex(const EFFECT_TEX tex) { m_tex = tex; }
	static int GetNumAll(void) { return m_nNumAll; }

private:
	static int m_nIdxTexture[TEX_MAX];	// テクスチャインデックス (CTexture用)
	D3DXVECTOR3 m_move;			// 移動量
	D3DXCOLOR m_col;			// 色
	int m_nLife;				// 寿命

	EFFECT_TEX m_tex;

	static int m_nNumAll;		// 総数
};

#endif