//=============================================================================
//
// 3Dオブジェクト[object3D.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _OBJECT3D_H_			//このマクロが定義されてなかったら
#define _OBJECT3D_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_F1			(30)				//ポリゴンサイズ
#define SIZE_F2			(400)				//ポリゴンサイズ

//オブジェクト2Dクラス---------------------------------------------------------
class CObject3D :public CObject
{
public:
	CObject3D(int nPriority = 3);
	~CObject3D();

	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_FLOOR,
		TYPE_MAX
	}TYPE;

	static HRESULT Load(void);
	static void Unload(void);
	static CObject3D* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 pos);
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetColor(D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetNor(const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	TYPE GetType(void){return m_type;}
	float GetHeight(void) { return m_fHeight; }
	void BindTexture(const LPDIRECT3DTEXTURE9 pTexture);
	bool CollisionPlayer(void);
	float CalcHeight(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	bool PointInTriangle(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	//LPDIRECT3DTEXTURE9 GetTexture(void) { return m_pTexture; }

	static int GetNumAll(void) { return m_nNumAll; }
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_size;		//サイズ		
	D3DXCOLOR m_col;		//色
	D3DXMATRIX m_mtxWorld;	//ワールドマトリックス			
	float m_fHeight;
	static int m_nIdxTexture;
	static int m_nNumAll;
	TYPE m_type;
};

#endif