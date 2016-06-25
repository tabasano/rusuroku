// rusuroku.cpp : アプリケーションのエントリ ポイントを定義します。
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

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
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


// このコード モジュールに含まれる関数の宣言を転送します:
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

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化しています。
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーションの初期化を実行します:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

    MSG msg;

    // メイン メッセージ ループ:
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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
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
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

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
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウの描画
//  WM_DESTROY  - 中止メッセージを表示して戻る
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
    static FLOAT        picstep = 0.5;
    static DWORD        recsec;
    static BOOL         stop = FALSE;
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

    switch (message) {

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 選択されたメニューの解析:
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
    {
        active=TRUE;
        return 0;
    }

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
            MessageBox(NULL, TEXT("WAVEデバイスのオープンに失敗しました。"), NULL, MB_ICONWARNING);
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
            MessageBox(NULL, TEXT("memoryが確保できません。"), NULL, MB_ICONWARNING);
            return -1;
        }
        bufferUsed = 0;
        point = 0;

        Sleep(200);
        // 現在時刻の取得
        time(&timer);

        waveInStart(hwi);
        SetTimer(hwnd, 1, timerMS, NULL);
        active=TRUE;

        //		if(WritePrivateProfileString(TEXT("init"), TEXT("mode"), TEXT("16"), TEXT("./rusuroku.ini"))==0)
        //			MessageBox(NULL, TEXT("init writeできません。"), NULL, MB_ICONWARNING);
        return 0;
    }
    case WM_SIZE:
        GetClientRect(hwnd, &rc);
        SendMessage(hwndStatus, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
        return 0;
    case WM_PAINT:
        DOUBLE tmpv;
        DOUBLE hrate;
        DWORD step, ystep,cdiff;
        DOUBLE xstep;
        DWORD boxx,boxy;
        DWORD center;
        DWORD chk;
        DWORD max;
        DWORD maxpos;
        DWORD ny, lx, ly, xover;
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
        xstep = 0.5;
        ystep = 5;
        max = 0;
        chk = 0;
        center = boxy / 2;
        SelectObject(hdc, GetStockObject(BLACK_PEN));
        SelectObject(hdc, GetStockObject(BLACK_BRUSH));
        Rectangle(hdc, 0, 0, boxx, boxy);
        if (silent) {
            SelectObject(hdc, penlineC);
        }
        else {
            SelectObject(hdc, GetStockObject(WHITE_PEN));
        }
        LINE(0, center, boxx, center);
        //paint only it is needed and not hidden
        if (picflag == PIC_ON && active && picBuffer && boxy > 0 && !silent)
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
            wsprintf(szBuf, TEXT("%s,%d"), TEXT("バッファ追加"), bufferUsed);
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
            wsprintf(szBuf, TEXT("%02d:%02d.%01d  buf: %d/%d rec: %d skip: %d"), (DWORD)timeSecond / 60, (DWORD)timeSecond % 60, timerc % 10, bufferUsed + 1, bufferNumR, v.size(), x.size());
        }
        else {
            wsprintf(szBuf, TEXT(" buf: %d formatTag:%d"), bufferUsed + 1, bufferNumR, wf.wFormatTag);
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
            SetWindowText(hwnd, TEXT("waveInClose..."));
            //show this message for waiting rest buffer writing down. replace it with better solution if exist
            res = MessageBox(NULL, TEXT("waveInClose..."), TEXT("OK"), MB_OKCANCEL);
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
    if (v[v.size() - 1] < x[xi]) sec += x[xi] - v[v.size() - 1];
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
    if (tag == WAVE_FORMAT_IEEE_FLOAT && bit == 32) {
        size = size / sizeof(FLOAT);
        for (c = 0; c < size; c++) {
            if (((FLOAT*)d)[c] > fh || ((FLOAT*)d)[c] < -fh)
            {
                a = abs(((FLOAT*)d)[c]) / fh;
                lv.push_back(a);
                ret = TRUE;
                break;
            }
        }
    }
    else if (tag == WAVE_FORMAT_IEEE_FLOAT && bit == 64) {
        size = size / sizeof(DOUBLE);
        for (c = 0; c < size; c++) {
            if (((DOUBLE*)d)[c] > fh || ((DOUBLE*)d)[c] < -fh)
            {
                a = abs(((DOUBLE*)d)[c]) / fh;
                lv.push_back(a);
                ret = TRUE;
                break;
            }
        }
    }
    else if (bit == 16) {
        h = SHORTMAX*per;
        size = size / sizeof(SHORT);
        for (c = 0; c < size; c++) {
            //if (((SHORT*)d)[c] - 32768 > h || ((SHORT*)d)[c] - 32768 < -h)
            if (((SHORT*)d)[c] > h || ((SHORT*)d)[c] < -h)
            {
                a = abs(((SHORT*)d)[c]) / 0x80;
                lv.push_back(a);
                ret = TRUE;
                break;
            }
        }
    }
    else if (bit == 32) {
        h = LONGMAX*per;
        size = size / sizeof(LONG);
        for (c = 0; c < size; c++) {
            if (((LONG*)d)[c] > h || ((LONG*)d)[c] < -h)
            {
                a = abs(((LONG*)d)[c]) / 0x8000;
                lv.push_back(a);

                ret = TRUE;
                break;
            }
        }
    }
    else if (bit == 8) {
        h = B8MAX*per;
        m = 0x80;
        for (c = 0; c < size; c++) {
            if (d[c] > h + m || d[c] < -h + m)
            {
                a = abs((INT)(d[c] - m));
                lv.push_back(a);
                ret = TRUE;
                break;
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
    dwtoint(0x010, formatchanksize);//固定
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


// バージョン情報ボックスのメッセージ ハンドラーです。
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
