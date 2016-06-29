// fftw.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

// cf.
// http://www.koj-m.sakura.ne.jp/hurobint/index.php?fftw
// ���g����̓��C�u�����ufftw3�v�g����


#include "stdafx.h"

#include <fftw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <algorithm>
#define pi 3.14159

using namespace std;

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")

// �T���v����
SHORT N = 1024;
fftw_complex *in, *out;
fftw_plan p;
ofstream fout;

BOOL fftwSetup()
{
    // ���g����͂̃p�����[�^�ݒ�
    in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fout.open("data.csv", ios::out | ios::ate);
    fout << "�T���v���ԍ�" << "," << "���͐M��" << "," << "���g��[Hz]" << "," << "�U��" << "," << endl;
    fout << "starttime sec, endtime sec." << endl << endl;
    return TRUE;
}
DOUBLE fftw(LPBYTE d, DWORD size, INT bit, INT tag, DOUBLE lim)
{
    // ���ݕ� 
    double dt = 0.001;
    DOUBLE res,max;
    time_t       timer,tend;

    // ���g����͂̑ΏۂƂȂ�g�`�̐���
    if (tag == WAVE_FORMAT_IEEE_FLOAT && bit == 32) {
        for (int i = 0;i < N;i++) {
            in[i][0] = SHORT(((FLOAT*)d)[i] * 0x8000);
            in[i][1] = 0;
        }
    }
    else if (tag == WAVE_FORMAT_IEEE_FLOAT && bit == 64) {
        for (int i = 0;i < N;i++) {
            in[i][0] = SHORT(((DOUBLE*)d)[i] * 0x8000);
            in[i][1] = 0;
        }
    }
    else if (bit == 16) {
        for (int i = 0;i < N;i++) {
            in[i][0] = ((SHORT*)d)[i];
            in[i][1] = 0;
        }
    }
    else if (bit == 32) {
        for (int i = 0;i < N;i++) {
            in[i][0] = ((LONG*)d)[i] / 0x8000;
            in[i][1] = 0;
        }
    }
    else if (bit == 8) {
        for (int i = 0;i < N;i++) {
            in[i][0] = ((SHORT)(d[i])-0x80)*0x100;
            in[i][1] = 0;
        }
    }
    // FFT���s
    double st,et;
    time(&timer);
    _timeb tv,tv2;
    _ftime_s(&tv);
    st = (double)tv.time;
    st += (double)tv.millitm / 1000.0;
    fftw_execute(p);

    // ��͌��ʂ̃t�@�C���o��
    // N�c1�����CN*dt�c1����[s]�C1/(N*dt)�c���g��[Hz]
    vector<DOUBLE> vec(N);
    for (int i = 0;i < N;i++) {
        res = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / ((double)N / 2.);
        vec[i] = res;
    }
    max = *max_element(vec.begin(), vec.end());
    DOUBLE lmt = max / 2;
    if(lmt<2) lmt = 2;
    fout << endl  << "// max " << max << endl;
    fout<< ">> over " <<lmt << endl;
    for (int i = 0;i < N;i++) {
        res = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / ((double)N / 2.);
        if (res > lmt) {
            fout << i << "," << in[i][0] << "," << (double)i / ((double)N*dt) << "," << res << "," << endl;
        }
    }
    time(&tend);
    _ftime_s(&tv2);
    et = (double)tv2.time;
    et += (double)tv2.millitm / 1000.0;
    fout << tv.time <<"."<< tv.millitm << endl;
    fout << tv2.time <<"."<< tv2.millitm << endl;

    return max;
}
void fftwDestroy()
{
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    fout.close();
}
