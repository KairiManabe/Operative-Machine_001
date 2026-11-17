//=============================================================================
// 
// 入力処理[input.h]
// Author : 眞鍋快陸
// 
//=============================================================================

//インクルードファイル---------------------------------------------------------
#include "input.h"
#include "debugproc.h"
#include "manager.h"

//静的メンバ変数宣言-----------------------------------------------------------
LPDIRECTINPUT8 CInput::m_pInput = NULL;

//=============================================================================
//コンストラクタ
CInput::CInput()
{
	//値のクリア
	m_pInput = NULL;
	m_pDevice = NULL;
}

//=============================================================================
//デストラクタ
CInput::~CInput()
{
}

//=============================================================================
//初期化処理
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInput == NULL)
	{
		//DirectInoutオブジェクトの生成
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
//終了処理
void CInput::Uninit(void)
{
	//デバイスの破棄
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	//オブジェクトの破棄
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}

}

//キーボード*******************************************************************

//=============================================================================
//コンストラクタ
CInputKeyboard::CInputKeyboard()
{
	//キー情報のクリア
	m_aKeyState[0] = {};
	m_aKeyStateTrigger[0] = {};
}

//=============================================================================
//デストラクタ
CInputKeyboard::~CInputKeyboard()
{
}

//=============================================================================
//初期化処理
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	//インプットの初期化
	CInput::Init(hInstance, hWnd);

	//入力デバイス(キーボードの生成)
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	//データフォーマット設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//協調モード
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return E_FAIL;
	}

	//キーボードへのアクセス権
	m_pDevice->Acquire();

	return S_OK;
}

//=============================================================================
//終了処理
void CInputKeyboard::Uninit(void)
{
	//インプットの終了
	CInput::Uninit();
}

//=============================================================================
//更新処理
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY];

	int nCntKey;

	//入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY; nCntKey++)
		{
			//キーボードのトリガー情報
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];

			//キーボードのプレス情報
			m_aKeyState[nCntKey] = aKeyState[nCntKey];
		}
	}
	else
	{
		m_pDevice->Acquire();		//キーボードへのアクセス権を獲得
	}
}

//=============================================================================
//キーボードのプレス情報を取得
bool  CInputKeyboard::GetPress(int nKey)
{
	return(m_aKeyState[nKey] & 0x80) ? true : false;
}

//=============================================================================
//キーボードのトリガー情報を取得
bool CInputKeyboard::GetTrigger(int nKey)
{
	return(m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//マウス***********************************************************************

//=============================================================================
//コンストラクタ
CInputMouse::CInputMouse()
{
	//マウス情報のクリア
	m_mouseState = {};
	m_mouseStateTrigger = {};
	m_mausePos = {};
	m_hWnd = NULL;
}

//=============================================================================
//デストラクタ
CInputMouse::~CInputMouse()
{
}

//=============================================================================
//初期化処理
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	//インプットの初期化
	CInput::Init(hInstance, hWnd);

	m_hWnd = hWnd;

	//入力デバイス(マウスの生成)
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	//データフォーマット設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return E_FAIL;
	}

	//協調モード
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return E_FAIL;
	}

	//キーボードへのアクセス権
	m_pDevice->Acquire();

	// カーソルを非表示にする
	//ShowCursor(FALSE);

	return S_OK;
}

//=============================================================================
//終了処理
void CInputMouse::Uninit(void)
{
	//インプットの終了
	CInput::Uninit();
}

//=============================================================================
//更新処理
void CInputMouse::Update(void)
{
	CDebugproc* pDebug = CManager::GetDebugproc();

	DIMOUSESTATE2 mouseState;

	m_mausePos = GetMausePoint();

	//入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState)))
	{
		for (int nCntMouse = 0; nCntMouse < MAX_MOUSE_NUM; nCntMouse++)
		{
			//キーボードのトリガー情報
			m_mouseStateTrigger.rgbButtons[nCntMouse] = (m_mouseState.rgbButtons[nCntMouse] ^ mouseState.rgbButtons[nCntMouse]) & mouseState.rgbButtons[nCntMouse];

			//マウスのプレス
			m_mouseState.rgbButtons[nCntMouse] = mouseState.rgbButtons[nCntMouse];
		}
	}
	else
	{
		m_pDevice->Acquire();		//マウスへのアクセス権を獲得
	}

	//pDebug->PrintDebugProc("マウスの座標(X:%.1f  Y:%.1f)", m_mausePos.x, m_mausePos.y);
}

//=============================================================================
//マウスのプレス情報を取得
bool CInputMouse::GetPress(int nMouse)
{
	return(m_mouseState.rgbButtons[nMouse] & 0x80) ? true : false;
}

//=============================================================================
//マウスのトリガー情報を取得
bool CInputMouse::GetTrigger(int nMouse)
{
	return(m_mouseStateTrigger.rgbButtons[nMouse] & 0x80) ? true : false;
}

//=============================================================================
//マウスの座標を取得
D3DXVECTOR3 CInputMouse::GetMausePoint(void)
{
	POINT point;

	//座標取得
	if (GetCursorPos(&point) != TRUE)
	{
		//マウスへのアクセス権
		m_pDevice->Acquire();
	}
	else
	{
		ScreenToClient(m_hWnd, &point);
	}

	return D3DXVECTOR3((float)point.x + 10.0f, (float)point.y + 16.0f, 0.0f);
}

//ジョイパッド*****************************************************************

//=============================================================================
//コンストラクタ
CInputJoypad::CInputJoypad()
{
	//マウス情報のクリア
	m_xJoykeyState = {};
	m_xJoykeyStateTrigger = {};
	m_xVibration = {};
	m_Power = NULL;
	m_nVibeCounter = 0;
	m_hWnd = NULL;
}

//=============================================================================
//デストラクタ
CInputJoypad::~CInputJoypad()
{
}

//=============================================================================
//ジョイパッドの初期化
HRESULT CInputJoypad::Init(HINSTANCE hInstance, HWND hWnd)
{
	//メモリのクリア
	memset(&m_xJoykeyState, 0, sizeof(XINPUT_STATE));

	//XInputのステートを設定(有効にする)
	XInputEnable(true);

	return S_OK;
}

//=============================================================================
//ジョイパッドの終了処理
void CInputJoypad::Uninit(void)
{
	//XInputのステート設定(無効にする)
	XInputEnable(false);
}

//=============================================================================
//ジョイパッドの更新処理
void CInputJoypad::Update(void)
{
	XINPUT_STATE joykeyState;		//ジョイパッドの入力情報

	m_xVibration.wLeftMotorSpeed = m_Power;
	m_xVibration.wRightMotorSpeed = m_Power;

	m_nVibeCounter--;
	if (m_nVibeCounter < 0)
	{
		m_Power = 0;
	}

	//ジョイパッドの状態を取得
	if (XInputGetState(0, &joykeyState) == ERROR_SUCCESS)
	{
		//ジョイパッドのトリガー情報
		m_xJoykeyStateTrigger.Gamepad.wButtons = (m_xJoykeyState.Gamepad.wButtons ^ joykeyState.Gamepad.wButtons) & joykeyState.Gamepad.wButtons;

		XInputSetState(0, &m_xVibration);

		m_xJoykeyState = joykeyState;		//ジョイパッドのプレス情報を保存
	}
}

//=============================================================================
//ジョイパッドのプレス情報を取得
bool CInputJoypad::GetPress(JOYKEY key)
{
	return(m_xJoykeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
//キーボードのトリガー情報を取得
bool CInputJoypad::GetTrigger(JOYKEY key)
{
	return(m_xJoykeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// スティックの入力を方向パッドフラグに変換
WORD CInputJoypad::ThumbToDPad(SHORT sThumbX, SHORT sThumbY, SHORT sDeadZone)
{
	WORD wButtons = 0;

	if (sThumbY >= sDeadZone)
	{
		wButtons |= XINPUT_GAMEPAD_DPAD_UP;
	}
	else if (sThumbY <= -sDeadZone)
	{
		wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
	}

	if (sThumbX <= -sDeadZone)
	{
		wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
	}
	else if (sThumbX >= sDeadZone)
	{
		wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
	}

	return wButtons;
}

//=============================================================================
// 左スティック
bool CInputJoypad::GetStickLeft(int* pLValueH, int* pLValueV)
{
	if (pLValueH)
	{
		*pLValueH = m_xJoykeyState.Gamepad.sThumbLX;
	}
	if (pLValueV)
	{
		*pLValueV = m_xJoykeyState.Gamepad.sThumbLY;
	}
	if (m_xJoykeyState.Gamepad.sThumbLX > 2000 || m_xJoykeyState.Gamepad.sThumbLX < -2000 ||
		m_xJoykeyState.Gamepad.sThumbLY > 2000 || m_xJoykeyState.Gamepad.sThumbLY < -2000)
	{//入力があった場合[いずれかの方向に一定の値を超えた入力があった場合]
		return true;
	}
	else
	{//入力されていないとみなす
		return false;
	}
}

//=============================================================================
// 右スティック
bool CInputJoypad::GetStickRight(int* pRValueH, int* pRValueV)
{
	if (pRValueH)
	{
		*pRValueH = m_xJoykeyState.Gamepad.sThumbRX;
	}
	if (pRValueV)
	{
		*pRValueV = m_xJoykeyState.Gamepad.sThumbRY;
	}
	if (m_xJoykeyState.Gamepad.sThumbRX > 1500 || m_xJoykeyState.Gamepad.sThumbRX < -1500 ||
		m_xJoykeyState.Gamepad.sThumbRY > 1500 || m_xJoykeyState.Gamepad.sThumbRY < -1500)
	{//入力があった場合[いずれかの方向に一定の値を超えた入力があった場合]
		return true;
	}
	else
	{//入力されていないとみなす
		return false;
	}
}

//=============================================================================
// 振動の設定
void CInputJoypad::SetVibration(WORD power, int vibe)
{
	//XINPUT_STATE state;
	//if (XInputGetState(controllerIndex, &state) == ERROR_SUCCESS)
	//{
	//	XINPUT_GAMEPAD_VIBRATION vibration;

	//	vibration.LeftMotorSpeed = leftMotorSpeed;		// 左モーターの速度
	//	vibration.RightMotorSpeed = rightMotorSpeed;	// 右モーターの速度
	//	XInputSetState(controllerIndex, &vibration);
	//}

	m_Power = power;

	m_nVibeCounter = vibe;
}
