// rusuroku.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "rusuroku.h"

#define MAX_LOADSTRING 1000

#define PIC_ON 0
#define LINE(x0,y0,x1,y1) \
			{MoveToEx(hdc,x0,y0,NULL);\
			LineTo(hdc,x1,y1);}
HPEN penline = CreatePen(PS_SOLID, 1, RGB(200, 0, 100));
HPEN penline2 = CreatePen(PS_SOLID, 1, RGB(200, 0, 200));
HPEN penlineC = CreatePen(PS_SOLID, 1, RGB(100, 100, 200));
HPEN penb = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

// �O���[�o���ϐ�:
HINSTANCE hInst;                                // ���݂̃C���^�[�t�F�C�X
WCHAR szTitle[MAX_LOADSTRING];                  // �^�C�g�� �o�[�̃e�L�X�g
WCHAR szWindowClass[MAX_LOADSTRING];            // ���C�� �E�B���h�E �N���X��
WCHAR name[MAX_LOADSTRING];
#define myinit TEXT("./rusuroku.ini")
//WCHAR initf[MAX_LOADSTRING];
vector<DWORD> v;
vector<DWORD> x;
vector<DWORD> lv;

ofstream ou;
void savetmp(LPBYTE lpWaveData, DWORD dwDataSize, ofstream &of, DWORD pos = 0);
void writeheader(WAVEFORMATEX &waveform, DWORD size, ofstream &ou);
void writedatasize(DWORD size, ofstream &ou);
BOOL checkNotZero(LPBYTE d, DWORD size, INT bit, INT tag, vector<DWORD> &lv, FLOAT p);
void logsave(vector<DWORD> &v, vector<DWORD> &x, vector<DWORD> &lv, time_t timer, DWORD step);

const DWORD headersize = 0x02c;


// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �����ɃR�[�h��}�����Ă��������B

    // �O���[�o������������������Ă��܂��B
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // �A�v���P�[�V�����̏����������s���܂�:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

    MSG msg;

    // ���C�� ���b�Z�[�W ���[�v:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


void SetWaveHdr(PWAVEHDR &pWaveHdr, PBYTE &pBuffer, ULONG size, DWORD flag, DWORD dwRepetitions)
{
    pWaveHdr->lpData = (char *)pBuffer;
    pWaveHdr->dwBufferLength = size;
    pWaveHdr->dwBytesRecorded = 0;
    pWaveHdr->dwUser = 0;
    pWaveHdr->dwFlags = flag;//
    pWaveHdr->dwLoops = dwRepetitions;
    pWaveHdr->lpNext = NULL;
    pWaveHdr->reserved = 0;
}


// memo
//ou.write((const char*)((PWAVEHDR) lParam)->lpData,((PWAVEHDR) lParam)->dwBytesRecorded);
//recordedsize +=((PWAVEHDR) lParam)->dwBytesRecorded;




//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:    ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND  - �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT    - ���C�� �E�B���h�E�̕`��
//  WM_DESTROY  - ���~���b�Z�[�W��\�����Ė߂�
//
//



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static const INT    bufferNum = 40;
    static DWORD        activeMax = 40;//active pic sec.
    static HWND         hwndStatus = NULL;
    static INT          bufferNumR = bufferNum; //successfully allocated
    static INT          picflag = PIC_ON;
    static DWORD        logstep = 10;
    static DWORD        timerc = 0;
    static LPBYTE       lpWaveData[bufferNum];
    static LPBYTE       picBuffer;
    static HWAVEIN      hwi = NULL;
    static WAVEHDR      wh[bufferNum];
    static PWAVEHDR     pWaveHdr[bufferNum];
    static DWORD        recordedSize = 0;
    static DWORD        rectime = 0;
    static DWORD        bufferUsed = 0;
    static BOOL         ending = FALSE;
    static BOOL         active = FALSE;
    static BOOL         silent = FALSE;
    static DWORD        activeCount = 0;
    static BOOL         nowRecording = FALSE;
    static FLOAT        per = 0;
    static FLOAT        perstep = 0.005;
    static FLOAT        perstepshift = 10;
    static FLOAT        picstep = 0.5;
    static DWORD        recsec;
    static BOOL         stop = FALSE;
    static BOOL         closing = FALSE;
    static time_t       timer;
    static DWORD        timerMS = 100;
    static LPBYTE       ptmpBuffer = NULL;

    static DOUBLE       timeSecond = 0;
    static DWORD        tmpBaseSec = 0;
    static DWORD        rsize = 0;
    static DWORD        tmpBufferSize = 0;
    static DWORD        dwRecordSecond = 3; //seconds per buffer

    PAINTSTRUCT	paintstruct;
    HDC hdc;

    static WAVEFORMATEX wf = { 0 };
    static INT bps = 16;
    static ULONG sps = 44100;
    static INT tag = WAVE_FORMAT_PCM;

    RECT rc;
    ULONG i;
    DWORD point;
    WCHAR ibuf[1000];
    DOUBLE shiftr = 1.0;

    switch (message) {

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // �I�����ꂽ���j���[�̉��:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
            break;
        case IDM_STOP:
            stop = TRUE;
            break;
        case IDM_RESUME:
            stop = FALSE;
            SendMessage(hwndStatus, SB_SETTEXT, 255 | 0, (LPARAM)TEXT("rec resume..."));
            break;
        case IDM_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    break;
    case WM_KEYDOWN:
        if (GetKeyState(VK_SHIFT) < 0) shiftr = perstepshift;
        switch(wParam){
        case VK_DOWN:
        case '-':
            if (per > perstep*shiftr) per -= perstep*shiftr;
            break;
        case VK_UP:
        case '+':
            if (per < 1.0) per += perstep*shiftr;
            break;
        case VK_LEFT:
            if (per > perstep/shiftr/2) per -= perstep/shiftr/2;
            break;
        case VK_RIGHT:
            if (per < 1.0) per += perstep/shiftr/2;
            break;
        }
        active=TRUE;
        return 0;

    case WM_LBUTTONDOWN:
        active=TRUE;
        return 0;

    case WM_CREATE: {
        INITCOMMONCONTROLSEX ic;
        DWORD dwDataSize;

        ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
        ic.dwICC = ICC_BAR_CLASSES;
        InitCommonControlsEx(&ic);

        hwndStatus = CreateWindowEx(0, STATUSCLASSNAME, TEXT(""), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        SendMessage(hwndStatus, SB_SIMPLE, TRUE, 0);

        bps = GetPrivateProfileInt(TEXT("init"), TEXT("bit"), bps, myinit);
        sps = GetPrivateProfileInt(TEXT("init"), TEXT("sps"), sps, myinit);
        tag = GetPrivateProfileInt(TEXT("init"), TEXT("tag"), tag, myinit);
        logstep = GetPrivateProfileInt(TEXT("init"), TEXT("logstep"), logstep, myinit);
        picflag = GetPrivateProfileInt(TEXT("init"), TEXT("picflag"), picflag, myinit);
        GetPrivateProfileString(TEXT("init"), TEXT("f"), TEXT("0.01"), ibuf, MAX_PATH, myinit);
        swscanf_s(ibuf, TEXT("%f"), &per);
        GetPrivateProfileString(TEXT("init"), TEXT("picstep"), TEXT("0.5"), ibuf, MAX_PATH, myinit);
        swscanf_s(ibuf, TEXT("%f"), &picstep);

        wf.nChannels = 2;
        wf.wFormatTag = tag;
        wf.nSamplesPerSec = sps;
        wf.wBitsPerSample = bps;
        wf.nBlockAlign = wf.wBitsPerSample / 8 * wf.nChannels;
        wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

        if (waveInOpen(&hwi, WAVE_MAPPER, &wf, (DWORD)hwnd, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR) {
            MessageBox(NULL, TEXT("WAVE�f�o�C�X�̃I�[�v���Ɏ��s���܂����B"), NULL, MB_ICONWARNING);
            return -1;
        }

        wsprintf(name, TEXT("test.wav"));

        ou.open(name, ios::out | ios::binary | ios::trunc | ios::ate);
        writeheader(wf, 0, ou);
        dwDataSize = wf.nAvgBytesPerSec * dwRecordSecond;
        picBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, dwDataSize);

        for (i = 0;i < bufferNum;i++)
        {
            lpWaveData[i] = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, dwDataSize);
            if (lpWaveData[i] == NULL) {
                bufferNumR = i;
                break;
            }
            wh[i].lpData = (LPSTR)lpWaveData[i];
            wh[i].dwBufferLength = dwDataSize;
            wh[i].dwFlags = 0;
            waveInPrepareHeader(hwi, &wh[i], sizeof(WAVEHDR));
            waveInAddBuffer(hwi, &wh[i], sizeof(WAVEHDR));
        }
        if (bufferNumR == 0) {
            MessageBox(NULL, TEXT("memory���m�ۂł��܂���B"), NULL, MB_ICONWARNING);
            return -1;
        }
        bufferUsed = 0;
        point = 0;

        Sleep(200);
        // ���ݎ����̎擾
        time(&timer);

        waveInStart(hwi);
        SetTimer(hwnd, 1, timerMS, NULL);
        active=TRUE;

        //		if(WritePrivateProfileString(TEXT("init"), TEXT("mode"), TEXT("16"), TEXT("./rusuroku.ini"))==0)
        //			MessageBox(NULL, TEXT("init write�ł��܂���B"), NULL, MB_ICONWARNING);
        return 0;
    }
    case WM_SIZE:
        GetClientRect(hwnd, &rc);
        SendMessage(hwndStatus, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
        return 0;
    case WM_PAINT:
        DOUBLE tmpv;
        DOUBLE hrate,prate;
        DWORD step, ystep,cdiff;
        DOUBLE xstep;
        DWORD boxx,boxy;
        DWORD center;
        DWORD chk;
        DWORD max;
        DWORD maxpos;
        DWORD ny, lx, ly, xover, tx, ty;
        DWORD dy,dx;
        DOUBLE nx;
        RECT rw, rs;
        GetWindowRect(hwnd, &rw);
        GetClientRect(hwnd, &rc);
        GetClientRect(hwndStatus, &rs);
        step = wf.nAvgBytesPerSec*((DWORD)timeSecond % dwRecordSecond);
        //timerc is not correct value exactly. temporally use
        step += wf.nAvgBytesPerSec / 10 * (timerc % 10);
        hdc = BeginPaint(hwnd, &paintstruct);
        boxx = rc.right;
        boxy = rc.bottom-rs.bottom;
        xover = boxx;
        maxpos = 15000;
        tmpv = 0;
        hrate = 0x100;
        cdiff = 1;
        dy = 1;
        xstep = 0.5;
        ystep = 5;
        max = 0;
        chk = 0;
        center = boxy / 2;
        SelectObject(hdc, GetStockObject(BLACK_PEN));
        SelectObject(hdc, GetStockObject(BLACK_BRUSH));
        Rectangle(hdc, 0, 0, boxx, boxy);
        ty=(0x8000*per / hrate)*ystep;
        SelectObject(hdc, penlineC);
        LINE(0, center + ty, boxx, center + ty);
        LINE(0, center - ty, boxx, center - ty);
        if (silent || closing || stop) {
            tx = boxx / 2 - 10;
            ty = center - 20;
            LINE(tx, ty, tx + 20, ty + 40);
            LINE(tx + 10, ty, tx + 10 + 20, ty + 40);
        }
        else {
            SelectObject(hdc, GetStockObject(WHITE_PEN));
            LINE(0, center - dy, boxx, center - dy);
            LINE(0, center + dy, boxx, center + dy);
            if (!active) {
                SelectObject(hdc, penline);
                prate = (timerc % 10) >= 5  ? 2 : -2;
                dx = 4;
                tx = boxx / 2 - 40;
                ty = center;
                LINE(tx, ty, tx + dx, ty + 3 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty + 6 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty + 8 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty + 9 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty + 10 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty + 9 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty + 8 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty + 6 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty + 3 * prate);
                tx += dx;
                LineTo(hdc,tx + dx, ty);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 3 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 6 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 8 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 9 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 10 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 9 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 8 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 6 * prate);
                tx += dx;
                LineTo(hdc, tx + dx, ty - 3 * prate);
                tx += dx;
                LineTo(hdc,tx + dx, ty);
            }
        }
        LINE(0, center, boxx, center);
        //paint only it is needed and not hidden
        if (picflag == PIC_ON && active && picBuffer && boxy > 0 && !silent && !closing)
        {
            SelectObject(hdc, penline);

            for (INT c = 0;c < wf.nChannels;c++) {
                if (c > 0)SelectObject(hdc, penline2);
                nx = 0;
                lx = 0;
                ny = center;
                ly = center;
                if (tmpBufferSize - 1 <= step) step = tmpBufferSize - 1;
                if (tag == WAVE_FORMAT_IEEE_FLOAT) {
                    if (bps == 32) {
                        for (INT i = 0;i < maxpos;i += 4)
                        {
                            if (step + i - 1 >= tmpBufferSize) break;
                            else
                            {

                                tmpv = *((FLOAT*)picBuffer + (step + i) / sizeof(FLOAT) )*0x8000;
                                if (abs((LONG)tmpv) > max)
                                    max = abs((LONG)tmpv);
                                nx += 1 * xstep;
                                ny = ((tmpv / hrate)*ystep + center) + c * cdiff;
                                LINE(lx, ly, nx, ny);
                                ly = ny;
                                lx = nx;
                                chk++;
                                if (lx > xover) break;
                            }
                        }
                    }
                    else if (bps == 64) {
                        for (INT i = 0;i < maxpos;i += 4)
                        {
                            if (step + i - 1 >= tmpBufferSize) break;
                            else
                            {

                                tmpv = *((DOUBLE*)picBuffer + (step + i) / sizeof(DOUBLE) )*0x8000;
                                if (abs((LONG)tmpv) > max)
                                    max = abs((LONG)tmpv);
                                nx += 1 * xstep;
                                ny = ((tmpv / hrate)*ystep + center) + c * cdiff;
                                LINE(lx, ly, nx, ny);
                                ly = ny;
                                lx = nx;
                                chk++;
                                if (lx > xover) break;
                            }
                        }
                    }
                }
                else {
                    for (INT i = 0;i < maxpos;i += 4)
                    {
                        if (step + i - 1 >= tmpBufferSize) break;
                        else
                        {
                            if (bps == 16) tmpv = *((SHORT*)picBuffer + (step + i) / sizeof(SHORT) + c);
                            else if (bps == 32) tmpv = *((LONG*)picBuffer + (step + i) / sizeof(LONG)) / 0x10000;
                            else if (bps == 8) tmpv = ((LONG)*((LPBYTE)picBuffer + (step + i) / sizeof(BYTE)) - 0x80) * 0x100;
                            else break;
                            if (abs((LONG)tmpv) > max)
                                max = abs((LONG)tmpv);
                            nx += 1 * xstep;
                            ny = ((tmpv / hrate)*ystep +  center) + c * cdiff;
                            LINE(lx, ly, nx, ny);
                            ly = ny;
                            lx = nx;
                            chk++;
                            if (lx > xover) break;
                        }
                    }
                }
                
            }
            SelectObject(hdc, penline);
            if (max > 5) {
                if (max>3000) SelectObject(hdc, GetStockObject(WHITE_BRUSH));
                else if (max>2000) SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));
                else if (max>1000) SelectObject(hdc, GetStockObject(GRAY_BRUSH));
                else SelectObject(hdc, GetStockObject(DKGRAY_BRUSH));
                SelectObject(hdc, penline);
            }
            else{
                SelectObject(hdc, GetStockObject(BLACK_BRUSH));
                SelectObject(hdc, penline2);
            }
            //Rectangle(hdc, 0, 0, 50, 50);
            //SelectObject(hdc, penline2);
            // LINE(0, 0,50,50);
            //if (chk > 10) LINE(52, 0, 52, 50);
        }
        EndPaint(hwnd, &paintstruct);

        //InvalidateRect(hwndStatus, NULL, FALSE);
        return 0;

        //against flashing screen
    case WM_ERASEBKGND:
        return 0;
    case MM_WIM_DATA:
        TCHAR  szBuf[256];
        DWORD tsize;
        DWORD size;
        BOOL re;
        tsize = wf.nAvgBytesPerSec;
        ptmpBuffer = (LPBYTE)((PWAVEHDR)lParam)->lpData;
        rsize = (DWORD)((PWAVEHDR)lParam)->dwBytesRecorded;
        if (picBuffer) memcpy(picBuffer, (const char*)ptmpBuffer, rsize);
        tmpBaseSec = timeSecond;
        tmpBufferSize = rsize;
        if (!stop) {
            while (rsize > 0) {
                size = tsize;
                if (rsize < tsize) size = rsize;
                recsec = rectime / tsize;
                re = checkNotZero(ptmpBuffer, size, wf.wBitsPerSample, wf.wFormatTag, lv, per);
                if (re) {
                    savetmp(ptmpBuffer, size, ou);
                    recordedSize += size;
                    if (!nowRecording || recsec%logstep == 0) {
                        v.push_back(recsec);
                    }
                    silent = FALSE;
                }
                else if (nowRecording) {
                    x.push_back(recsec);
                    silent = TRUE;
                };

                nowRecording = re;
                rectime += size;
                ptmpBuffer += size;
                rsize -= size;
            }
        }
        else {
            //MessageBox(NULL, TEXT("now stop.."), TEXT("OK"), MB_OK);
        }
        bufferUsed += 1;
        waveInAddBuffer(hwi, (PWAVEHDR)lParam, sizeof(WAVEHDR));
        //GetWindowText(hwndStatus, szBuf, 255);
        if (stop) {
            wsprintf(szBuf, TEXT("%s"), TEXT("rec stop"));
        }
        else {
            wsprintf(szBuf, TEXT("%s,%d"), TEXT("�o�b�t�@�ǉ�"), bufferUsed);
        }
        SendMessage(hwndStatus, SB_SETTEXT, 255 | 0, (LPARAM)szBuf);
        //SetWindowText(hwnd, szBuf);
        return 0;

    case MM_WIM_CLOSE:
        SendMessage(hwndStatus, SB_SETTEXT, 255 | 0, (LPARAM)TEXT("REC CLOSE..."));
        Sleep(1000);
        ending = TRUE;
        return 0;

    case WM_TIMER: {
        TCHAR  szBuf[256];
        DWORD ms;
        MMTIME mmt;
        INT r;

        mmt.wType = TIME_SAMPLES;
        r = waveInGetPosition(hwi, &mmt, sizeof(MMTIME));

        timeSecond = mmt.u.sample / wf.nSamplesPerSec;
        ms = (timeSecond - (DWORD)timeSecond) * 100;
        timerc++;
        if (active) {
            activeCount++;
            if (activeCount > activeMax*(1000.0/timerMS)) {
                active=FALSE;
                activeCount=0;
            }
        }
        if (r == 0) {
            wsprintf(szBuf, TEXT("%02d:%02d.%01d  buf: %d/%d rec: %d skip: %d limit: %d.%d%%"), (DWORD)timeSecond / 60, (DWORD)timeSecond % 60, timerc % 10, bufferUsed + 1, bufferNumR, v.size(), x.size(), (INT)(per * 100), ((INT)(per * 1000) % 10));
        }
        else {
            wsprintf(szBuf, TEXT(" buf: %d formatTag:%d limit: %d.%d%%"), bufferUsed + 1, bufferNumR, wf.wFormatTag, (INT)(per*100), ((INT)(per * 1000) % 10));
        }

        InvalidateRect(hwnd, NULL, TRUE);
        SetWindowText(hwnd, szBuf);

        return 0;
    }

    case WM_CLOSE:
        INT res;

        if (hwi != NULL) {
            MMTIME mmt;

            mmt.wType = TIME_BYTES;
            waveInGetPosition(hwi, &mmt, sizeof(MMTIME));

            waveInReset(hwi);

            for (i = 0;i < bufferNumR;i++)
            {
                waveInUnprepareHeader(hwi, pWaveHdr[i], sizeof(WAVEHDR));
            }
            waveInClose(hwi);
            closing = TRUE;
            SetWindowText(hwnd, TEXT("closing..."));
            //show this message for waiting rest buffer writing down. replace it with better solution if exist
            res = MessageBox(NULL, TEXT("end of recording, bye..."), TEXT("waveIn"), MB_OK);
            //while (! ending) Sleep(100);
            writedatasize(recordedSize, ou);
            ou.close();
            x.push_back(recsec);
            logsave(v, x, lv, timer, logstep);

            KillTimer(hwnd, 1);

        }
        //Sleep(1000);

        if (res == IDOK) DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        DeleteObject(penline);
        DeleteObject(penline2);
        DeleteObject(penlineC);
        DeleteObject(penb);
        if (lpWaveData != NULL)
            for (i = 0;i < bufferNumR;i++)
            {
                HeapFree(GetProcessHeap(), 0, lpWaveData[i]);
            }

        PostQuitMessage(0);

        return 0;

    default:
        break;

    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void logsave(vector<DWORD> &v, vector<DWORD> &x, vector<DWORD> &lv, time_t timer, DWORD step)
{
    ofstream o;
    DWORD last = 0;
    DWORD sec = 0;
    DWORD xi = 0;
    WCHAR logname[MAX_LOADSTRING];
    char  buf[256];
    BOOL start = TRUE;

    ctime_s(buf, 255, &timer);
    if (step == 0) step = 1;

    wsprintf(logname, TEXT("reclog.txt"));

    o.open(logname, ios::out | ios::trunc);
    o << "start at: " << buf << endl;
    o << "rec-time, real-time, sample-level" << endl;
    for (DWORD i = 0; i < (int)v.size(); ++i) {
        if (v[i] > x[xi]) {
            sprintf_s(buf, "--:--  %02d:%02d --", (INT)(x[xi] / 60), (INT)(x[xi] % 60));
            o << buf << endl;
            sec += x[xi] - last;
            xi++;
        }
        else {
            sec += v[i] - last;
        }
        sprintf_s(buf, "%02d:%02d  %02d:%02d %d", sec / 60, sec % 60, (INT)(v[i] / 60), (INT)(v[i] % 60), lv[i]);
        o << buf << endl;
        last = v[i];
    }
    if (v.size()>0 && v[v.size() - 1] < x[xi]) sec += x[xi] - v[v.size() - 1];
    sprintf_s(buf, "%02d:%02d  end", sec / 60, sec % 60);
    o << buf << endl;
    o.close();
}

BOOL checkNotZero(LPBYTE d, DWORD size, INT bit, INT tag, vector<DWORD> &lv, FLOAT per)
{
    BOOL ret = FALSE;
    DOUBLE h;
    DOUBLE fh = (DOUBLE)per;
    DWORD a;
    DWORD c, m;
    DWORD bl;
    DWORD block = 4;
    DWORD blocksize;
    DWORD start;
    DWORD current;
    if (tag == WAVE_FORMAT_IEEE_FLOAT && bit == 32) {
        size = size / sizeof(FLOAT);
        blocksize = size / block;
        for (c = 0; c < blocksize; c++) {
            start = 0;
            for (bl = 0; bl < block; bl++) {
                current = c + start;
                if (((FLOAT*)d)[current] > fh || ((FLOAT*)d)[current] < -fh)
                {
                    a = abs(((FLOAT*)d)[current]) / fh;
                    lv.push_back(a);
                    ret = TRUE;
                    break;
                }
                start += blocksize;
            }
        }
    }
    else if (tag == WAVE_FORMAT_IEEE_FLOAT && bit == 64) {
        size = size / sizeof(DOUBLE);
        blocksize = size / block;
        for (c = 0; c < blocksize; c++) {
            start = 0;
            for (bl = 0; bl < block; bl++) {
                current = c + start;
                if (((DOUBLE*)d)[current] > fh || ((DOUBLE*)d)[current] < -fh)
                {
                    a = abs(((DOUBLE*)d)[current]) / fh;
                    lv.push_back(a);
                    ret = TRUE;
                    break;
                }
                start += blocksize;
            }
        }
    }
    else if (bit == 16) {
        h = SHORTMAX*per;
        size = size / sizeof(SHORT);
        blocksize = size / block;
        for (c = 0; c < blocksize; c++) {
            start = 0;
            for (bl = 0; bl < block; bl++) {
                current = c + start;
                if (((SHORT*)d)[current] > h || ((SHORT*)d)[current] < -h)
                {
                    a = abs(((SHORT*)d)[current]) / 0x80;
                    lv.push_back(a);
                    ret = TRUE;
                    break;
                }
                start += blocksize;
            }
        }
    }
    else if (bit == 32) {
        h = LONGMAX*per;
        size = size / sizeof(LONG);
        blocksize = size / block;
        for (c = 0; c < blocksize; c++) {
            start = 0;
            for (bl = 0; bl < block; bl++) {
                current = c + start;
                if (((LONG*)d)[current] > h || ((LONG*)d)[current] < -h)
                {
                    a = abs(((LONG*)d)[current]) / 0x8000;
                    lv.push_back(a);
                    ret = TRUE;
                    break;
                }
                start += blocksize;
            }
        }
    }
    else if (bit == 8) {
        h = B8MAX*per;
        m = 0x80;
        blocksize = size / block;
        for (c = 0; c < blocksize; c++) {
            start = 0;
            for (bl = 0; bl < block; bl++) {
                current = c + start;
                if (d[current] > h + m || d[current] < -h + m)
                {
                    a = abs((INT)(d[current] - m));
                    lv.push_back(a);
                    ret = TRUE;
                    break;
                }
                start += blocksize;
            }
        }
    }
    return ret;
}

void wtoint(WORD w, int *sampleps)
{
    sampleps[1] = HIBYTE(w);
    sampleps[0] = LOBYTE(w);
}

void dwtoint(DWORD dw, int *sampleps)
{
    wtoint(HIWORD(dw), sampleps + 2);
    wtoint(LOWORD(dw), sampleps);
}

void writedatasize(DWORD size, ofstream &ou)
{
    int i;
    int datasize[4];
    dwtoint(size + headersize - 0x02c, datasize);

    ou.seekp(headersize - 8, ios::beg);
    ou << "data";
    if (size > 0)
    {
        for (i = 0;i < 4;i++)ou.put((unsigned char)datasize[i]);
    }
    else for (i = 0;i < 4;i++)ou.put((unsigned char)255);
}

void writeheader(WAVEFORMATEX &waveform, DWORD size, ofstream &ou)
{
    int i;
    int chanksize[4];
    dwtoint(size + headersize - 8, chanksize);
    int formatchanksize[4];
    dwtoint(0x010, formatchanksize);//�Œ�
    int tag[2];//PCM 1,float 3
    wtoint(waveform.wFormatTag, tag);
    int channels[2];
    wtoint(waveform.nChannels, channels);
    int sampleps[4];
    dwtoint(waveform.nSamplesPerSec, sampleps);
    int avg[4];
    dwtoint(waveform.nAvgBytesPerSec, avg);
    int block[2];
    wtoint(waveform.nBlockAlign, block);
    int bitps[2];
    wtoint(waveform.wBitsPerSample, bitps);
    int datasize[4];
    dwtoint(size + headersize - 0x02c, datasize);


    ou.seekp(0, ios::beg);
    ou << "RIFF";
    for (i = 0;i < 4;i++)ou.put((unsigned char)chanksize[i]);
    ou << "WAVE";
    ou << "fmt ";
    for (i = 0;i < 4;i++)ou.put((unsigned char)formatchanksize[i]);
    for (i = 0;i < 2;i++)ou.put((unsigned char)tag[i]);
    for (i = 0;i < 2;i++)ou.put((unsigned char)channels[i]);
    for (i = 0;i < 4;i++)ou.put((unsigned char)sampleps[i]);
    for (i = 0;i < 4;i++)ou.put((unsigned char)avg[i]);
    for (i = 0;i < 2;i++)ou.put((unsigned char)block[i]);
    for (i = 0;i < 2;i++)ou.put((unsigned char)bitps[i]);
    writedatasize(size, ou);
}

void setwaveformRec(WAVEFORMATEX &wf, int format, int channels, long rate, int samplesize)
{
    wf.wFormatTag = format;
    wf.nChannels = channels;
    wf.nSamplesPerSec = rate;
    wf.nAvgBytesPerSec = samplesize / 8 * channels*rate;
    wf.nBlockAlign = samplesize / 8 * channels;
    wf.wBitsPerSample = samplesize;
    wf.cbSize = 0;
}


void savetmp(LPBYTE lpWaveData, DWORD dwDataSize, ofstream &of, DWORD pos)
{

    of.write(reinterpret_cast<const char *>(lpWaveData), dwDataSize);
    of.flush();
}


// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
