// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
// Windows �w�b�_�[ �t�@�C��:

//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>

#include <vector>

// C �����^�C�� �w�b�_�[ �t�@�C��
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

using namespace std;
// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă�������

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "comctl32.lib")

#define 	WAVE_FORMAT_IEEE_FLOAT   0x0003

//#define MODE_FLOAT
//#define MODE_FLOAT64
#define SHORTMAX 32768
#define LONGMAX (DOUBLE)0x10000*32768
#define B8MAX 0x80