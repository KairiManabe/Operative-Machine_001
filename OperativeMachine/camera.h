//=============================================================================
// 
// カメラクラス[camera.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _CAMERA_H_ 
#define _CAMERA_H_

//インクルードファイル---------------------------------------------------------
#include"main.h"

//マクロ-----------------------------------------------------------------------
#define CAMERA_ANGLE		(150.0f)
#define CAMERA_ANGLE_PY		(80.0f)
#define CAMERA_ANGLE_PZ		(150.0f)
#define STAND_BY			(100)		//待機

//前方宣言---------------------------------------------------------------------
class CPlayer;

//カメラクラス-----------------------------------------------------------------
class CCamera
{
public:
	CCamera();
	~CCamera();

	void Init(void);
	void Uninit(void);
	void Update(void);
	
	void SetCamera(void);

	D3DXVECTOR3 GetPositionV(void) { return m_posV; }
	D3DXVECTOR3 GetPositionR(void) { return m_posR; }
	D3DXVECTOR3 GetPositionU(void) { return m_posU; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetAngle(void) { return m_angle; }

	void SetPositionV(D3DXVECTOR3 posV) { m_posV = posV; }
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; }
	void SetAngle(D3DXVECTOR3 angle) { m_angle = angle; }
	void SetAll(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 posU, D3DXVECTOR3 rot);
	bool CheckRayToCamera(LPD3DXMESH pMesh, const D3DXMATRIX& mtxWorld, const D3DXVECTOR3& rayStart, const D3DXVECTOR3& rayEnd, float* pOutHitDist);
private:
	D3DXVECTOR3 m_posV;				//始点
	D3DXVECTOR3 m_posR;				//注視点
	D3DXVECTOR3 m_posU;				//上方向ベクトル
	D3DXVECTOR3 m_angle;			//上方向ベクトル
	D3DXMATRIX m_mtxProjection;		//プロジェクションマトリックス
	D3DXMATRIX m_mtxView;			//ビューマトリックス
	D3DXMATRIX m_mtxWorld;			//ワールドマトリックス
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_posVDest;			//目的の始点
	D3DXVECTOR3 m_posRDest;			//目的の注視点
	int m_counter;
	int m_fastKeyPress;
};

#endif // !1
