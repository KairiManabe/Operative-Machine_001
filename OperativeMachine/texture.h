//=============================================================================
// 
// テクスチャの管理[texture.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _TEXTURE_H_ 
#define _TEXTURE_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_TEXTURE		(256)
#define MAX_PATH_LENGTH (256) // ファイル名の最大文字数

//テクスチャクラス-------------------------------------------------------------
class CTexture
{
public:
	CTexture();
	~CTexture();

	HRESULT Load(void);
	void Unload(void);
	int Register(const char* pFilename);
	LPDIRECT3DTEXTURE9 GetAddres(int nIdx);
private:
	LPDIRECT3DTEXTURE9 m_apTexture[MAX_TEXTURE];
	char m_aFilename[MAX_TEXTURE][MAX_PATH_LENGTH]; // ファイル名を保持する配列
	int m_nNumAll;
};

#endif