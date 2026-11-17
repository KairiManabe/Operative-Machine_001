//=============================================================================
//
// メッシュフィールド[meshfield.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _MESHFIELD_H_ 
#define _MESHFIELD_H_

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"
#include "object3D.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_MF1			(100.0f)				//ポリゴンサイズ
#define SIZE_MF2			(1100.0f)				//ポリゴンサイズ

//メッシュフィールドクラス---------------------------------------------------------
class CMeshField :public CObject
{
public:
	CMeshField(int nPriority = 3);
	~CMeshField();

	static HRESULT Load(void);
	static void Unload(void);
	static CMeshField* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size,int nDivisionDepth, int nDivisionWidth);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 pos);
	void SetColor(D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	float GetHeight(void) { return m_fHeight; }
	D3DXVECTOR3 GetNor(const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	void BindTexture(const LPDIRECT3DTEXTURE9 pTexture);
	bool CollisionPlayer(void);
	float CalcHeight(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	bool PointInTriangle(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;
	D3DXMATRIX m_mtxWorld;	//ワールドマトリックス
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	float m_fHeight;
	int m_nDivisionDepth;
	int m_nDivisionWidth;
	static int m_nIdxTexture;
};
#endif // !1

