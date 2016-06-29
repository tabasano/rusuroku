#pragma once

#include "resource.h"
DOUBLE fftw(LPBYTE d, DWORD size, INT bit, INT tag, DOUBLE lim);
void fftwDestroy();
BOOL fftwSetup();

