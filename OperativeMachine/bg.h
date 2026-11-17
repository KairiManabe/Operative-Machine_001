//=============================================================================
// 
// îwåièoóÕ[bg.h]
// Author : ·¡ìÁâıó§
// 
//=============================================================================
#ifndef _BG_H_ 
#define _BG_H_

#include "main.h"

//É}ÉNÉç
#define BG_H		(720)
#define BG_W		(1280)
#define MAX_BG		(1)

class CBG
{
public:
	CBG();
	~CBG();

	void Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	int m_nIdxTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
};
#endif

