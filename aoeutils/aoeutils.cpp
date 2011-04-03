// aoeutils.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "aoeutils.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
HWND hDlg; 

// このコード モジュールに含まれる関数の宣言を転送します:
BOOL				InitInstance(HINSTANCE, int);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。
    MSG msg;
    HACCEL hAccelTable;

    // グローバル文字列を初期化しています。
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_AOEUTILS, szWindowClass, MAX_LOADSTRING);

    // アプリケーションの初期化を実行します:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AOEUTILS));

    // メイン メッセージ ループ:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            //		if (!IsDialogMessage(msg.hwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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
    HWND hWnd;

    hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

    hDlg = hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// バージョン情報ボックスのメッセージ ハンドラです。
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

// バージョン情報ボックスのメッセージ ハンドラです。
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        {
            int wmId    = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDOK:
            case IDCANCEL:
            case IDM_EXIT:
                DestroyWindow(hDlg);
                return (INT_PTR)TRUE;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, About);
                return (INT_PTR)TRUE;
            case IDM_RUN1:
            case IDC_BUTTON1:
                run1(hDlg, lParam);
                return (INT_PTR)TRUE;
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
    case WM_APP_RUN1:
        run2(hDlg, lParam);
        break;
    }

    return (INT_PTR)FALSE;
}

void run1(HWND hWnd, LPARAM lParam)
{
    HWND targetWnd = FindWindow(NULL, TARGET_WINDOW_NAME);

    RECT rect = {0};

    SetForegroundWindow(targetWnd);
    Sleep(500);
    PostMessage(hWnd, WM_APP_RUN1, NULL, NULL);
    return;
}

void run_capture_target_window(BITMAPINFO *pbmi, void **ppbits){
    HWND targetWnd = FindWindow(NULL, TARGET_WINDOW_NAME);

    RECT rect = {0};

    GetClientRect(targetWnd, &rect);
    LONG width = rect.right - rect.left;
    LONG height = rect.bottom - rect.top;
    HDC hdcScreen = GetDC(targetWnd);

    //BITMAPFILEHEADERの作成
    ZeroMemory(&pbmi->bmiHeader, sizeof(pbmi->bmiHeader));
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = width;
    pbmi->bmiHeader.biHeight = height;
    pbmi->bmiHeader.biPlanes = 1;

    //１ピクセルあたりビット数を格納
    pbmi->bmiHeader.biBitCount = GetDeviceCaps(hdcScreen, BITSPIXEL);

    //ビットマップのバイト数を格納
    //(今時の環境では、色は１ピクセル=４バイト表現なので、単純に*4とした。)
    pbmi->bmiHeader.biSizeImage = pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biHeight * 4;

    //DIB を作成してウインドウからビットマップをコピー
    HBITMAP hBitmap = CreateDIBSection(hdcScreen, pbmi, DIB_RGB_COLORS, ppbits, NULL, 0);
    HDC hdcCompat = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmapPrev = (HBITMAP)SelectObject(hdcCompat, hBitmap);
    BitBlt(hdcCompat, 0, 0, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight, hdcScreen, rect.left, rect.top, SRCCOPY);
    SelectObject(hdcCompat, hBitmapPrev);
    ReleaseDC(NULL, hdcScreen);
}

void run_filesave(BYTE* bmp_data, SIZE_T bmp_filesize){
    HANDLE hfile;
    DWORD tmp;
    //DIB をファイルへセーブ
    hfile = CreateFile(_T("TEST1.bmp"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
    WriteFile(hfile, bmp_data, bmp_filesize, &tmp, NULL);
    CloseHandle(hfile);
}

void run_httpsend(BYTE* bmp_data, SIZE_T bmp_filesize){
    /* WININET初期化 */
    HINTERNET hInternet = InternetOpen(_T("WININET Sample Program"),
        INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    /* サーバへ接続 */
    HINTERNET hHttpSession = InternetConnect(hInternet,
        _T("pokonori.fous.jp"), INTERNET_DEFAULT_HTTP_PORT,
        NULL, NULL,
        INTERNET_SERVICE_HTTP,
        0, 0);

    /* HTTP要求の作成 */
    HINTERNET hHttpRequest = HttpOpenRequest(hHttpSession,
        _T("POST"), _T("/bin/db/host_parser.cgi"), NULL, NULL,
        NULL, 0, 0);

    bool res;
    TCHAR* h1 = _T("Content-Type: multipart/form-data; boundary=z--aoeutils--z");
    res = HttpAddRequestHeaders(hHttpRequest, h1, _tcslen(h1), HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);

    const char* b1 = 
        "Content-Type: multipart/form-data; boundary=z--aoeutils--z\x0D\x0A"
        "--z--aoeutils--z\x0D\x0A"
        "Content-Disposition: form-data; name=\"host1\"; filename=\"host1.bmp\"\x0D\x0A"
        "Content-Type: image/bmp\x0D\x0A\x0D\x0A";
    const char* b3 = "\x0D\x0A--z--aoeutils--z--\x0D\x0A";
    SIZE_T req_size = strlen(b1) + bmp_filesize + strlen(b3);
    BYTE* req_data = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, req_size);
    CopyMemory(req_data, b1, strlen(b1));
    CopyMemory(req_data + strlen(b1), bmp_data, bmp_filesize);
    CopyMemory(req_data + strlen(b1) + bmp_filesize, b3, strlen(b3));

    res = HttpSendRequest(hHttpRequest, NULL, 0, req_data, req_size);
    assert(res);
    HeapFree(GetProcessHeap(), 0, req_data);

    DWORD resBufSize = 8192;
    BYTE* resBuf = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, resBufSize);

    /* コンテンツの内容を取得・表示 */
    while(1){
        DWORD readSize;
        res = InternetReadFile(hHttpRequest, resBuf, resBufSize, &readSize);
        if(res == 1 && readSize == 0) break;
        assert(res);

        int resTextLen = MultiByteToWideChar(CP_THREAD_ACP, 0, (char*)resBuf, readSize, NULL, 0) + 1;
        TCHAR* resText = new TCHAR[resTextLen];
        ZeroMemory(resText, sizeof(TCHAR)*resTextLen);
        MultiByteToWideChar(CP_THREAD_ACP, 0, (char*)resBuf, readSize, resText, resTextLen);
        HWND editCtl = GetDlgItem(hDlg, IDC_EDIT1);
        SetDlgItemText(hDlg, IDC_EDIT1, resText);
        delete[] resText;
    }

    /* 後処理 */
    HeapFree(GetProcessHeap(), 0, resBuf);
    InternetCloseHandle(hHttpRequest);
    InternetCloseHandle(hHttpSession);
    InternetCloseHandle(hInternet);
}

void run_bmp2png(){
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    CLSID   encoderClsid;
    Status  stat;
    Image*   image = new Image(L"Bird.bmp");

    // Get the CLSID of the PNG encoder.
    GetEncoderClsid(L"image/png", &encoderClsid);

    stat = image->Save(L"Bird.png", &encoderClsid, NULL);

    if(stat == Ok)
        printf("Bird.png was saved successfully\n");
    else
        printf("Failure: stat = %d\n", stat); 

    delete image;
    GdiplusShutdown(gdiplusToken);

}

void run2(HWND hWnd, LPARAM lParam)
{
    void *pbits;
    BITMAPINFO *pbmi = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)];
    // キャプチャ
    run_capture_target_window(pbmi, &pbits);

    //ビットマップファイルをメモリ上に作成

    //ヘッダーを作成
    BITMAPFILEHEADER bmfh;
    bmfh.bfType = 0x4D42;   //BitMapファイルを表すコード
    //("BM"をWORD型に格納したもので、決まりごとです。)
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    bmfh.bfSize = bmfh.bfOffBits+pbmi->bmiHeader.biSizeImage;

    SIZE_T bmp_filesize = bmfh.bfOffBits + pbmi->bmiHeader.biSizeImage;
    BYTE* bmp_data = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, bmp_filesize);
    CopyMemory(bmp_data, &bmfh, sizeof(bmfh));
    CopyMemory(bmp_data + sizeof(bmfh), &pbmi->bmiHeader, sizeof(pbmi->bmiHeader));
    CopyMemory(bmp_data + bmfh.bfOffBits, pbits, pbmi->bmiHeader.biSizeImage);

    run_filesave(bmp_data, bmp_filesize);
    run_httpsend(bmp_data, bmp_filesize);

    //BITMAPINFOの解放
    delete[] (BYTE*)pbmi;    
    HeapFree(GetProcessHeap(), 0, bmp_data);
}
