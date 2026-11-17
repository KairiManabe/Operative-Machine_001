//=============================================================================
//
// モデルのエフェクト[effectmodel.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _EFFECTMODEL_H_			//このマクロが定義されてなかったら
#define _EFFECTMODEL_H_			//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"
#include "object.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_PARTICLEMODEL	(10000)

//オブジェクトXクラス----------------------------------------------------------
class CEffectModel :public CObject
{
public:
	CEffectModel(int nPriority = 4);
	~CEffectModel();

	typedef enum
	{
		EFFECT_NONE = -1,
		EFFECT_BREAK,
		EFFECT_BOMB,
		EFFECT_DETH_HUMAN,
		EFFECT_MAX,
	}TYPE;

	static CEffectModel* Create(TYPE type, const std::string& fileName, D3DXVECTOR3 pos, D3DXVECTOR3 move, int nLife);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void DrawShadow(void);

	void SetPosition(const D3DXVECTOR3 pos);
	void SetColor(const D3DXCOLOR col) { m_col = col; }
	void SetEffModel(CEffectModel* pObj) { m_pEffModel = pObj; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXMATRIX GetWorldMatrix(void) { return m_mtxWorld; }
	TYPE GetType(void) { return m_type; }
	static int GetNumAll(void) { return m_nNumAll; }

	static bool GetWallClim(void) { return m_bWallClim; }
private:
	CEffectModel* m_pEffModel;
	int m_nIdxTexture[100];
	int m_nIdxModel;		//モデルインデックスは配列である必要はない
	std::string m_pFileName;
	D3DXMATRIX m_mtxWorld;
	D3DXVECTOR3 m_vtxMin, m_vtxMax;		//モデルの最小値、最大値
	D3DXVECTOR3 m_Center;	//オブジェクトの中心座標
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_posOld;	//位置
	D3DXVECTOR3 m_move;		//位置
	D3DXVECTOR3 m_moveRot;		//位置
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_size;		//サイズ
	D3DXCOLOR m_col;		//色
	TYPE m_type;			//オブジェクトのタイプ

	static bool m_bWallClim;//壁登り状態
	static int m_wallnum;//壁登り状態
	static int m_nNumAll;
	int m_nFloating;		//浮遊上下切り替え用
	float m_fInit;			//Yの初期位置
	int m_nLife;
};

#endif