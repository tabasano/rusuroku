#pragma once

DOUBLE fftw(LPBYTE d, DWORD size, WORD bit, WORD tag, WORD ch, DWORD sps, DOUBLE lim, BOOL log);
DOUBLE fftwPeak(LPBYTE d, DWORD size, WORD bit, WORD tag, WORD ch, DWORD sps, DOUBLE lim);
void fftwDestroy();
BOOL fftwSetup();
BOOL getPeak();

