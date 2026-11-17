//=============================================================================
// 
// 入力処理[input.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _INPUT_H_ 
#define _INPUT_H_

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//インクルードファイル---------------------------------------------------------
#include "main.h"

//マクロ定義-------------------------------------------------------------------
#define NUM_KEY			(256)
#define MAX_MOUSE_NUM	(3)

//ジョイパッドのキー種類-------------------------------------------------------
typedef enum
{
	JOYKEY_UP = 0,			//十字キー上
	JOYKEY_DOWN,			//十字キー下
	JOYKEY_LEFT,			//十字左
	JOYKEY_RIGHT,			//十字右
	JOYKEY_START,			//スタートボタン
	JOYKEY_BACK,			//バックボタン
	JOYKEY_LSTICKPUT,		//Lスティック押し込み
	JOYKEY_RSTICKPUT,		//Rスティック押し込み
	JOYKEY_LB,				//LB
	JOYKEY_RB,				//RB
	JOYKEY_DUMMY1,
	JOYKEY_DUMMY2,
	JOYKEY_A,
	JOYKEY_B,
	JOYKEY_X,
	JOYKEY_Y,
	JOYKEY_,
	JOYKEY_MAX
}JOYKEY;

//入力クラス-------------------------------------------------------------------
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;
protected:
	static LPDIRECTINPUT8 m_pInput;
	LPDIRECTINPUTDEVICE8 m_pDevice;
};

//キーボードクラス(派生クラス)-------------------------------------------------
class CInputKeyboard:public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
private:
	BYTE m_aKeyState[NUM_KEY];				
	BYTE m_aKeyStateTrigger[NUM_KEY];		//キーボードのトリガー情報
};

//マウスクラス(派生クラス)-------------------------------------------------
class CInputMouse :public CInput
{
public:
	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nMouse);
	bool GetTrigger(int nMouse);
	D3DXVECTOR3 GetMausePoint(void);
private:
	DIMOUSESTATE2 m_mouseState;				//マウスのプレス情報
	DIMOUSESTATE2 m_mouseStateTrigger;		//マウスのトリガー情報
	D3DXVECTOR3 m_mausePos;
	HWND m_hWnd;
};

//ジョイパッドクラス(派生クラス)-----------------------------------------------
class CInputJoypad :public CInput
{
public:
	CInputJoypad();
	~CInputJoypad();

	typedef struct
	{
		DWORD LeftMotorSpeed;
		DWORD RightMotorSpeed;
	}XINPUT_GAMEPAD_VIBRATION;

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(JOYKEY key);
	bool GetTrigger(JOYKEY key);
	bool GetStickLeft(int* pLValueH, int* pLValueV);
	bool GetStickRight(int* pRValueH, int* pRValueV);
	void SetVibration(WORD power, int vibe);
	WORD ThumbToDPad(SHORT sThumbX, SHORT sThumbY, SHORT sDeadZone);	// スティックの入力を方向パッドフラグに変換
private:
	XINPUT_STATE m_xJoykeyState;			//ジョイパッドのプレス情報
	XINPUT_STATE m_xJoykeyStateTrigger;		//ジョイパッドのプレス情報
	XINPUT_VIBRATION m_xVibration;
	WORD m_Power;
	int m_nVibeCounter;
	HWND m_hWnd;
};
#endif