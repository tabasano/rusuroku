// rusuroku.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "rusuroku.h"

#define MAX_LOADSTRING 1000

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
    static const INT    bufferNum = 20;
    static HWND         hwndStatus = NULL;
    static INT          bufferNumR = bufferNum; //successfully allocated
    static DWORD        logstep = 10;
    static LPBYTE       lpWaveData[bufferNum];
    static HWAVEIN      hwi = NULL;
    static WAVEHDR      wh[bufferNum];
    static PWAVEHDR     pWaveHdr[bufferNum];
    static DWORD        recordedSize = 0;
    static DWORD        rectime = 0;
    static DWORD        bufferUsed = 0;
    static BOOL         ending = FALSE;
    static BOOL         nowRecording = FALSE;
    static FLOAT        per = 0;
    static DWORD        recsec;
    static BOOL         stop = FALSE;
    static time_t       timer;

    static WAVEFORMATEX wf = { 0 };
    static INT bps = 16;
    static ULONG sps = 44100;
    static INT tag = WAVE_FORMAT_PCM;

    ULONG i;
    DWORD rsize;
    DWORD point;
    WCHAR ibuf[1000];

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

    case WM_CREATE: {
        INITCOMMONCONTROLSEX ic;
        DWORD dwRecordSecond = 6; //seconds per buffer
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
        GetPrivateProfileString(TEXT("init"), TEXT("f"), NULL, ibuf, MAX_PATH, myinit);
        swscanf_s(ibuf, TEXT("%f"), &per);

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
        SetTimer(hwnd, 1, 200, NULL);


        //		if(WritePrivateProfileString(TEXT("init"), TEXT("mode"), TEXT("16"), TEXT("./rusuroku.ini"))==0)
        //			MessageBox(NULL, TEXT("init write�ł��܂���B"), NULL, MB_ICONWARNING);
        return 0;
    }

    case MM_WIM_DATA:
        TCHAR  szBuf[256];
        LPBYTE d;
        DWORD tsize;
        DWORD size;
        BOOL re;
        tsize = wf.nAvgBytesPerSec;
        d = (LPBYTE)((PWAVEHDR)lParam)->lpData;
        rsize = (DWORD)((PWAVEHDR)lParam)->dwBytesRecorded;
        if (!stop) {
            while (rsize > 0) {
                size = tsize;
                if (rsize < tsize) size = rsize;
                recsec = rectime / tsize;
                re = checkNotZero(d, size, wf.wBitsPerSample, wf.wFormatTag, lv, per);
                if (re) {
                    savetmp(d, size, ou);
                    recordedSize += size;
                    if (!nowRecording || recsec%logstep == 0) {
                        v.push_back(recsec);
                    }
                }
                else if (nowRecording) {
                    x.push_back(recsec);
                };

                nowRecording = re;
                rectime += size;
                d += size;
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
        DWORD  dwSecond;
        MMTIME mmt;
        INT r;

        mmt.wType = TIME_SAMPLES;
        r = waveInGetPosition(hwi, &mmt, sizeof(MMTIME));

        dwSecond = mmt.u.sample / wf.nSamplesPerSec;
        if (r == 0) {
            wsprintf(szBuf, TEXT("%02d:%02d  buf: %d/%d rec: %d skip: %d"), dwSecond / 60, dwSecond % 60, bufferUsed + 1, bufferNumR, v.size(), x.size());
        }
        else {
            wsprintf(szBuf, TEXT(" buf: %d formatTag:%d"), bufferUsed + 1, bufferNumR, wf.wFormatTag);
        }

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