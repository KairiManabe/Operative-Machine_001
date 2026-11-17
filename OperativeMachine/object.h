//=============================================================================
//
//オブジェクト[object.h]
// 
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _OBJECT_H_				//このマクロが定義されてなかったら
#define _OBJECT_H_				//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include "main.h"

//マクロ定義-------------------------------------------------------------------
#define MAX_DATA		(512)
#define PRIORITY_MAX	(8)		//最大の優先順位

//オブジェクトクラス-----------------------------------------------------------
class CObject
{
public:
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_PLAYER,
		TYPE_ENEMY,
		TYPE_TARGET,
		TYPE_BULLET,
		TYPE_EFFECT,
		TYPE_MAX
	}TYPE;

	CObject(int nPriority = 3);
	virtual ~CObject();

	virtual HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 size) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void SetPosition(const D3DXVECTOR3 pos) = 0;
	virtual D3DXVECTOR3 GetPosition(void) = 0;
	virtual D3DXVECTOR3 GetSize(void) = 0;
	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	void SetType(const TYPE type);
	TYPE GetType(void) { return m_type; }
	static CObject* Getobject(int nPriority, const int nIdx) { return m_apObject[nPriority][nIdx]; }

	CObject* GetNext(void) const { return m_pNext; }
	static CObject* GetTop(int nPriority) { return m_pTop[nPriority]; }
protected:
	void Release(void);
private:
	static CObject* m_pTop[PRIORITY_MAX];	//先頭のオブジェクトへのポインタ
	static CObject* m_pCur[PRIORITY_MAX];	//最後尾のオブジェクトへのポインタ
	CObject* m_pPrev;						//前のオブジェクトへのポインタ
	CObject* m_pNext;						//次のオブジェクトへのポインタ
	static CObject* m_apObject[PRIORITY_MAX][MAX_DATA];
	static int m_nNumAll;	//オブジェクト総数
	int m_nID;				//自分自身のID
	int m_nPriority;		//優先順位
	TYPE m_type;
	bool m_bDeath;			//死亡フラグ
};

#endif 