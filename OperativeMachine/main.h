//=============================================================================
// 
// メイン[main.h]
// Author : 眞鍋快陸
// 
//=============================================================================
#ifndef _MAIN_H_					//このマクロが定義されてなかったら
#define _MAIN_H_					//2重インクルード防止のマクロを定義

//インクルードファイル---------------------------------------------------------
#include<windows.h>
#include <time.h>
#include <cstdio>
#include <iostream>
#include <chrono>
#include "d3dx9.h"						//描画処理に必要
#define DIRECTINPUT_VERSION	(0x0800)	//ビルド時の警告対処処理
#include "dinput.h"						//入力処理に必要
#include "xaudio2.h"
#include <XInput.h>

//ライブラリのリンク
#pragma comment(lib,"d3d9.lib")		//描画処理に必要
#pragma comment(lib,"d3dx9.lib")	//[d3d9]の拡張ライブラリ
#pragma comment(lib,"dxguid.lib")	//DirectXコンポネント使用に必要
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"xinput.lib")	//ジョイパッド処理に必要

//マクロ定義
#define CLASS_NAME			"windowClass"			//ウィンドウクラスの名前
#define WINDOW_NAME			"Operative Mchine"		//ウィンドウの名前

#define SCREEN_WIDTH		(1280)					//ウィンドウの幅
#define SCREEN_HEIGHT		(720)					//ウィンドウの高さ
#define BESIDE_MOVE			(1.0f)					//横方向への移動速度
#define VERTICAL_MOVE		(1.0f)					//上の移動速度
#define DIAGONAL_MOVE		(0.7f)					//斜め移動
#define FVF_VERTEX_2D		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)					//頂点フォーマット[2D]
#define FVF_VERTEX_3D		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)		//頂点フォーマット[3D]

#pragma comment(lib,"winmm.lib")	//システム時刻取得に必要

//頂点情報[3D]の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;		//頂点座標(x,y,z)
	D3DXVECTOR3 nor;		//法線ベクトル
	D3DCOLOR col;			//頂点カラー(r,d,b,a)
	D3DXVECTOR2 tex;		//テクスチャ座標
}VERTEX_3D;

//頂点情報[2D]の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;		//頂点座標(x,y,z)
	float rhw;				//座標変換用係数(1.0fで固定)
	D3DCOLOR col;			//頂点カラー(r,d,b,a)
	D3DXVECTOR2 tex;		//テクスチャ座標
}VERTEX_2D;

#endif 