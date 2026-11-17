//=============================================================================
//
// 3Dオブジェクト床[field.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _FIELD_H_			//このマクロが定義されてなかったら
#define _FIELD_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"
#include "effectmodel.h"

//マクロ定義-------------------------------------------------------------------
#define SIZE_F1			(30)				//ポリゴンサイズ
#define SIZE_F2			(400)				//ポリゴンサイズ

//オブジェクト2Dクラス---------------------------------------------------------
class CField :public CObject
{
public:
	CField(int nPriority = 1);
	~CField();

	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_FLOOR,
		TYPE_WALL,
		TYPE_MAX
	}TYPE;

	static CField* Create(std::string fileName, D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXVECTOR3 rot, TYPE type);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 pos);
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetColor(const D3DXCOLOR col) { m_col = col; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetNor(const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	TYPE GetType(void){return m_type;}
	float GetHeight(void) { return m_fHeight; }
	void BindTexture(const LPDIRECT3DTEXTURE9 pTexture);
	bool CollisionPlayer(void);
	bool CollisionModelEffect(CEffectModel *pEffectModel);
	float CalcHeight(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	bool PointInTriangle(const D3DXVECTOR3& player, const D3DXVECTOR3& A, const D3DXVECTOR3& B, const D3DXVECTOR3& C);
	//LPDIRECT3DTEXTURE9 GetTexture(void) { return m_pTexture; }

	static int GetNumAll(void) { return m_nNumAll; }

	// 影平面計算用にワールド座標系の地面の点と法線を取得する関数
	D3DXVECTOR3 GetPlanePoint(void) const { return m_pos; }
	D3DXVECTOR3 GetPlaneNormal(void)
	{
		//フィールドの回転（m_rot）を反映した上向き法線（0, 1, 0）を計算
		D3DXMATRIX mtxRot;
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);

		D3DXVECTOR3 normal(0.0f, 1.0f, 0.0f);
		D3DXVec3TransformNormal(&normal, &normal, &mtxRot);
		return normal;
	}
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