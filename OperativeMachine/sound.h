//=============================================================================
//
// サウンド処理 [sound.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//サウンドクラス---------------------------------------------------------------
class CSound
{
public:
	CSound();
	~CSound();

//*****************************************************************************
// サウンド一覧
//*****************************************************************************
	typedef enum
	{
		SOUND_LABEL_TITLEBGM = 0,	// タイトルBGM
		SOUND_LABEL_GAMEBGM,		// ゲーム内BGM
		SOUND_LABEL_BGM000,			// 使うかわからないBGM
		SOUND_LABEL_MOVESE,			// 移動SE
		SOUND_LABEL_JUMPSE,			// ジャンプSE
		SOUND_LASBEL_FAILEDSE,		// failedSE(敵の索敵範囲に当たったとき)
		SOUND_LASBEL_TRANS000,		// 変型時のSE1
		SOUND_LASBEL_TRANS001,		// 変型時のSE2
		SOUND_LABEL_MAX,
	} SOUND_LABEL;

	//*****************************************************************************
	// サウンド情報の構造体定義
	//*****************************************************************************
	typedef struct
	{
		const char* pFilename;	// ファイル名
		int nCntLoop;			// ループカウント
	} SOUNDINFO;

	HRESULT Init(HWND hWnd);
	void Uninit(void);
	static HRESULT PlaySound(SOUND_LABEL label);
	static void StopSound(SOUND_LABEL label);
	static void StopSound(void);
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
private:
	// サウンドの情報
	SOUNDINFO m_aSoundInfo[SOUND_LABEL_MAX] =
	{
		{"data/BGM/titleBGM001.wav", -1},			// タイトルBGM
		{"data/BGM/gameBGM000.wav", 0},				// ゲーム内BGM
		{"data/BGM/stoptime.wav", 0},				// 使うかわからないBGM
		{"data/SE/jett000.wav", 0},					// 移動SE
		{"data/SE/jump000.wav", 0},					// ジャンプSE
		{"data/SE/failed000.wav", 0},				// failedSE(敵の索敵範囲に当たったとき)
		{"data/SE/transform000.wav", 0},				// 変型時に鳴らすSE1
		{"data/SE/transform001.wav", 0},				// 変型時に鳴らすSE2
	};

	IXAudio2* m_pXAudio2;									// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice* m_pMasteringVoice;				// マスターボイス
	IXAudio2SourceVoice* m_apSourceVoice[SOUND_LABEL_MAX];	// ソースボイス
	BYTE* m_apDataAudio[SOUND_LABEL_MAX];					// オーディオデータ
	DWORD m_aSizeAudio[SOUND_LABEL_MAX];					// オーディオデータサイズ
};	

#if 0
//*****************************************************************************
// サウンド一覧
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM000 = 0,		// BGM0
	SOUND_LABEL_SE_SHOT,		// 弾発射音
	SOUND_LABEL_SE_HIT,			// ヒット音
	SOUND_LABEL_SE_EXPLOSION,	// 爆発音
	SOUND_LABEL_SE_CHECK,		// 決定音
	SOUND_LABEL_SE_ALARM,		// アラート
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);
#endif

#endif
