//=============================================================================
// 
// モデルクラス[model.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _MODEL_H_ 
#define _MODEL_H_

//インクルード-----------------------------------------------------------------
#include "main.h"

//モデルクラス-----------------------------------------------------------------
class CModel
{
public:
	CModel();
	~CModel();

	void Init(const std::string& fileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawShadow(const D3DXMATRIX& shadowMatrix);

	static CModel* Create(const std::string& fileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot);

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetSize(const float fsize) { m_fSize = fsize; }
	void SetParent(CModel* pModel);

	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetOffSetPos(void) { return m_offsetPos; }
	D3DXVECTOR3 GetOffSetRot(void) { return m_offsetRot; }
	float GetSize(void){return m_fSize;}
private:
	LPD3DXMESH m_pMesh;
	LPD3DXBUFFER m_pBuffMat;
	DWORD m_dwNumMat;
	D3DXMATRIX m_mtxWorld;
	D3DXVECTOR3 m_vtxMin, m_vtxMax;		//モデルの最小値、最大値
	int m_nIdxTexture[100];
	int m_nIdxModel;		//モデルインデックスは配列である必要はない

	D3DXVECTOR3 m_pos;					//位置
	D3DXVECTOR3 m_rot;					//向き
	D3DXVECTOR3 m_move;					//位置
	D3DXCOLOR m_col;					//色

	D3DXVECTOR3 m_offsetPos;			//オフセットpos
	D3DXVECTOR3 m_offsetRot;			//オフセットrot
	CModel* m_pParent;					//親モデルへのポインタ
	std::string m_FileName;
	static int m_nParentModel;			//親モデルが存在するモデルの数

	float m_fSize;
};

#endif