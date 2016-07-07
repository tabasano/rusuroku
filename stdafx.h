// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:

//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>

#include <vector>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

using namespace std;
// TODO: プログラムに必要な追加ヘッダーをここで参照してください

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "comctl32.lib")

#define 	WAVE_FORMAT_IEEE_FLOAT   0x0003

//#define MODE_FLOAT
//#define MODE_FLOAT64
#define SHORTMAX 32768
#define LONGMAX (DOUBLE)0x10000*32768
#define B8MAX 0x80