// fftw.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

// cf.
// http://www.koj-m.sakura.ne.jp/hurobint/index.php?fftw
// 周波数解析ライブラリ「fftw3」使い方


#include "stdafx.h"

#include <fftw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <algorithm>

#include "fftw.h"

#define pi 3.14159

using namespace std;

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")

// サンプル数
SHORT N = 1024;
vector<DOUBLE> flimit(N);
vector<DOUBLE> flimit0(N);
fftw_complex *in, *out;
fftw_plan p;
ofstream fout;
BOOL peak = FALSE;
BOOL doing = FALSE;

BOOL getPeak()
{
    return peak;
}
BOOL fftwSetup()
{
    // 周波数解析のパラメータ設定
    in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fout.open("data-csv.txt", ios::out | ios::ate);
    fout << "サンプル番号" << "," << "入力信号" << "," << "周波数[Hz]" << "," << "振幅" << "," << endl;
    fout << "calc starttime sec, endtime sec." << endl << endl;
    for (SHORT i = 0;i < N;i++) {
        flimit[i] = 0;
    }
    return TRUE;
}
DOUBLE fftw(LPBYTE d, DWORD size, WORD bit, WORD tag, WORD ch, DWORD sps, DOUBLE lim, BOOL log)
{
    if (doing) {
        fout << "! skip" << endl;
        return 0;
    }
    if (size  < N * (bit / 8) * ch)
    {
        return 0;
    }
    doing = TRUE;
    peak = FALSE;
    // 刻み幅 
    DOUBLE dt = 1.0 / sps;
    DOUBLE peakrate = 5;
    DOUBLE res,max,last,last2;
    time_t       timer,tend;

    // 周波数解析の対象となる波形の生成
    if (tag == WAVE_FORMAT_IEEE_FLOAT && bit == 32) {
        for (int i = 0;i < N;i++) {
            in[i][0] = SHORT(((FLOAT*)d)[i*ch] * 0x8000);
            in[i][1] = 0;
        }
    }
    else if (tag == WAVE_FORMAT_IEEE_FLOAT && bit == 64) {
        for (int i = 0;i < N;i++) {
            in[i][0] = SHORT(((DOUBLE*)d)[i*ch] * 0x8000);
            in[i][1] = 0;
        }
    }
    else if (bit == 16) {
        for (int i = 0;i < N;i++) {
            in[i][0] = ((SHORT*)d)[i*ch];
            in[i][1] = 0;
        }
    }
    else if (bit == 32) {
        for (int i = 0;i < N;i++) {
            in[i][0] = ((LONG*)d)[i*ch] / 0x8000;
            in[i][1] = 0;
        }
    }
    else if (bit == 8) {
        for (int i = 0;i < N;i++) {
            in[i][0] = ((SHORT)(d[i*ch])-0x80)*0x100;
            in[i][1] = 0;
        }
    }
    // FFT実行
    double st,et;
    time(&timer);
    _timeb tv,tv2;
    _ftime_s(&tv);
    st = (double)tv.time;
    st += (double)tv.millitm / 1000.0;
    fftw_execute(p);

    // 解析結果のファイル出力
    // N…1周期，N*dt…1周期[s]，1/(N*dt)…周波数[Hz]
    vector<DOUBLE> vec(N);
    for (SHORT i = 0;i < N;i++) {
        res = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / ((double)N / 2.);
        vec[i] = res;
        if (vec[i] > flimit[i] * 1.1) {
            flimit[i] += (vec[i] - flimit[i]) / 10;
            flimit0[i] = vec[i];
        }
        else if (vec[i] < flimit[i] * 0.6) {
            flimit[i] += (vec[i] - flimit[i]) / 10;
        }
    }
    max = *max_element(vec.begin(), vec.end());
    if (!log)
    {
        doing = FALSE;
        return max;
    }
    DOUBLE lmt = max / 2;
    if(lmt<2) lmt = 2;
    fout << endl  << "// max " << max << endl;
    fout<< ">> over " <<lmt << endl;
    last=0;
    last2=0;
    for (SHORT i = 0;i < N;i++) {
        res = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / ((double)N / 2.);
        // find out a peak
        if (last > lmt) {
            fout << i - 1 << "," << in[i - 1][0] << "," << (double)(i - 1) / ((double)N*dt) << "," << last;
            if ((i > 1 && last2<last && last>res) || i == 1) {
                fout << ", *";
                if (vec[i-1] > 10 && vec[i-1] > flimit[i-1] * peakrate) {
                    peak = TRUE;
                }
            }
            fout << endl;
        }
        last2=last;
        last=res;
    }
    time(&tend);
    _ftime_s(&tv2);
    et = (double)tv2.time;
    et += (double)tv2.millitm / 1000.0;
    fout << tv.time <<"."<< tv.millitm << endl;
    fout << tv2.time <<"."<< tv2.millitm << endl;

    doing = FALSE;
    return max;
}

DOUBLE fftwPeak(LPBYTE d, DWORD size, WORD bit, WORD tag, WORD ch, DWORD sps, DOUBLE lim)
{
    return fftw(d, size, bit, tag, ch, sps, lim, FALSE);
}
void fftwDestroy()
{
    fout << endl;
    fout << "// each max, limit" << endl;
    for (SHORT i = 0;i < N;i++) {
        fout << i << "," << flimit0[i] << " // " << flimit[i] << endl;
    }
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    fout.close();
}
