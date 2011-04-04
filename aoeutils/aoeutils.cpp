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
            case IDC_BUTTON1:
                run_capturetg(hDlg, lParam);
                return (INT_PTR)TRUE;
            case IDC_BUTTON2:
                run_filetg(hDlg, lParam);
                return (INT_PTR)TRUE;
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
    case WM_APP_RUN1:
        run_capturetg(hDlg, lParam);
        break;
    }

    return (INT_PTR)FALSE;
}

void run_foreground(HWND hWnd, LPARAM lParam)
{
    HWND targetWnd = FindWindow(NULL, TARGET_WINDOW_NAME);

    RECT rect = {0};

    SetForegroundWindow(targetWnd);
    Sleep(500);
    PostMessage(hWnd, WM_APP_RUN1, NULL, NULL);
    return;
}

struct bmp2png_buf_desc{
    void* hptr;
    SIZE_T length;
    SIZE_T capacity;
    LONG width;
    LONG height;

    bmp2png_buf_desc(): hptr(NULL), length(0), capacity(0), width(0), height(0){
    }

    ~bmp2png_buf_desc(){
        if(hptr != NULL){
            HeapFree(GetProcessHeap(), 0, hptr);
        }
    }

    void require(SIZE_T size){
        if(size > capacity){
            SIZE_T newcap = max(max(capacity, 1024)*2, size);
            void* newptr = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, newcap);
            if(hptr != NULL){
               CopyMemory(newptr, hptr, length);
               HeapFree(GetProcessHeap(), 0, hptr);
            }
            hptr = newptr;
            capacity = newcap;
        }
    }
};

void run_bmp_bgra2rgb(BYTE* dst, const BYTE* src, SIZE_T src_length){
    const BYTE* end = src + src_length;

    while (src < end){
      BYTE b = *src++; // b
      BYTE g = *src++; // b
      BYTE r = *src++; // b
      *dst++ = r;
      *dst++ = g;
      *dst++ = b;
      src += 1; // skip
    }
}

void run_capture_target_window(bmp2png_buf_desc* bmpbuf){
    void *pbits;
    BITMAPINFO *pbmi = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)];

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
    pbmi->bmiHeader.biBitCount = GetDeviceCaps(hdcScreen, BITSPIXEL);
    pbmi->bmiHeader.biSizeImage = pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biHeight * pbmi->bmiHeader.biBitCount/8;

    //DIB を作成してウインドウからビットマップをコピー
    HBITMAP hBitmap = CreateDIBSection(hdcScreen, pbmi, DIB_RGB_COLORS, &pbits, NULL, 0);
    HDC hdcCompat = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmapPrev = (HBITMAP)SelectObject(hdcCompat, hBitmap);
    BitBlt(hdcCompat, 0, 0, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight, hdcScreen, rect.left, rect.top, SRCCOPY);

    bmpbuf->require(pbmi->bmiHeader.biSizeImage*3/4);
    run_bmp_bgra2rgb((BYTE*) bmpbuf->hptr, (BYTE*) pbits, pbmi->bmiHeader.biSizeImage);
    bmpbuf->width = pbmi->bmiHeader.biWidth;
    bmpbuf->height = pbmi->bmiHeader.biHeight;
    bmpbuf->length = pbmi->bmiHeader.biSizeImage*3/4;

    SelectObject(hdcCompat, hBitmapPrev);
    ReleaseDC(NULL, hdcScreen);
    delete[] (BYTE*)pbmi;
}

static void PNGCBAPI run_bmp2png_write_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    bmp2png_buf_desc* buf = (bmp2png_buf_desc*) png_get_io_ptr(png_ptr);
    buf->require(buf->length + length);
    CopyMemory((BYTE*)buf->hptr + buf->length, data, length);
    buf->length += length;
    // png_error(png_ptr,"Write Error");
}

static void PNGCBAPI run_bmp2png_flush_fn(png_structp png_ptr){
    ;
}

bool run_bmp2png(bmp2png_buf_desc* pngbuf, bmp2png_buf_desc* bmpbuf){
  png_struct  *png_ptr = NULL;
  png_info    *info_ptr = NULL;
  png_byte    *bmp_pixels = (png_byte*) bmpbuf->hptr;
  png_byte    **row_pointers = NULL;
  png_uint_32 width = bmpbuf->width;
  png_uint_32 height = bmpbuf->height;
  png_uint_32 in_row_bytes = width * 3;

  /* prepare the standard PNG structures */
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    return FALSE;
  }
  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
  {
    png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
    return FALSE;
  }

  /* setjmp() must be called in every function that calls a PNG-reading libpng function */
  if (setjmp (png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
    return FALSE;
  }

  /* initialize the png structure */
//  FILE* fp = fopen("test2.png", "wb");
//  png_init_io(png_ptr, fp);
  png_set_write_fn(png_ptr, pngbuf, run_bmp2png_write_fn, run_bmp2png_flush_fn);

  /* we're going to write more or less the same PNG as the input file */
  png_set_IHDR (png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  /* write the file header information */
  png_write_info (png_ptr, info_ptr);

  /* if needed we will allocate memory for an new array of row-pointers */
  if (row_pointers == (unsigned char**) NULL)
  {
    if ((row_pointers = (png_byte **) malloc (height * sizeof (png_bytep))) == NULL)
    {
      png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
      return FALSE;
    }
  }

  /* set the individual row_pointers to point at the correct offsets */
  for (unsigned int i = 0; i < (height); i++)
    row_pointers[i] = bmp_pixels + (height-i-1) * in_row_bytes;

  /* write out the entire image data in one call */
  png_write_image (png_ptr, row_pointers);

  /* write the additional chuncks to the PNG file (not really needed) */
  png_write_end (png_ptr, info_ptr);

  /* clean up after the write, and free any memory allocated */
  png_destroy_write_struct (&png_ptr, (png_infopp) NULL);

  if (row_pointers != (unsigned char**) NULL)
    free (row_pointers);
  // fclose(fp);

  return TRUE;
} /* end of pnm2png */

void run_filesave(bmp2png_buf_desc* pngbuf){
    HANDLE hfile;
    DWORD tmp;
    //DIB をファイルへセーブ
    hfile = CreateFile(_T("TEST1.png"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
    WriteFile(hfile, pngbuf->hptr, pngbuf->length, &tmp, NULL);
    CloseHandle(hfile);
}

void run_httpsend(bmp2png_buf_desc* pngbuf){
    std::string queryStr("/bin/db/host_parser.cgi");
    for(int i = 0; i < 8; i++){
        queryStr.append(i == 0 ? "?p" : "&p");
        queryStr.push_back('1' + i);
        queryStr.append(".rank=");
        
        TCHAR rankText[2];
        char cRankText[6];
        GetDlgItemText(hDlg, IDC_EDIT2+i, rankText, sizeof(rankText));
        WideCharToMultiByte(CP_THREAD_ACP, 0, rankText, 2, cRankText, 6, NULL, NULL);
        queryStr.append(cRankText);
    }
    
    int queryTextLen = MultiByteToWideChar(CP_THREAD_ACP, 0, (char*) queryStr.c_str(), queryStr.length(), NULL, 0) + 1;
    TCHAR* queryText = new TCHAR[queryTextLen];
    ZeroMemory(queryText, sizeof(TCHAR)*queryTextLen);
    MultiByteToWideChar(CP_THREAD_ACP, 0, (char*)queryStr.c_str(), queryStr.length(), queryText, queryTextLen);

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
        _T("POST"), queryText, NULL, NULL,
        NULL, 0, 0);
    delete[] queryText;

    BOOL res;
    TCHAR* h1 = _T("Content-Type: multipart/form-data; boundary=z--aoeutils--z");
    res = HttpAddRequestHeaders(hHttpRequest, h1, _tcslen(h1), HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD);

    const char* b1 = 
        "Content-Type: multipart/form-data; boundary=z--aoeutils--z\x0D\x0A"
        "--z--aoeutils--z\x0D\x0A"
        "Content-Disposition: form-data; name=\"host1\"; filename=\"host1.png\"\x0D\x0A"
        "Content-Type: image/png\x0D\x0A\x0D\x0A";
    const char* b3 = "\x0D\x0A--z--aoeutils--z--\x0D\x0A";
    SIZE_T req_size = strlen(b1) + pngbuf->length + strlen(b3);
    BYTE* req_data = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, req_size);
    CopyMemory(req_data, b1, strlen(b1));
    CopyMemory(req_data + strlen(b1), pngbuf->hptr, pngbuf->length);
    CopyMemory(req_data + strlen(b1) + pngbuf->length, b3, strlen(b3));

    res = HttpSendRequest(hHttpRequest, NULL, 0, req_data, req_size);
    assert(res);
    HeapFree(GetProcessHeap(), 0, req_data);

    DWORD resBufSize = 8192;
    BYTE* resBuf = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, resBufSize);

    std::string resString;
    /* コンテンツの内容を取得・表示 */
    while(1){
        DWORD readSize;
        res = InternetReadFile(hHttpRequest, resBuf, resBufSize, &readSize);
        if(res == 1 && readSize == 0) break;
        assert(res);
        resString.append((char*) resBuf, readSize);
    }
    int resTextLen = MultiByteToWideChar(CP_THREAD_ACP, 0, (char*)resString.c_str(), resString.length(), NULL, 0) + 1;
    TCHAR* resText = new TCHAR[resTextLen];
    ZeroMemory(resText, sizeof(TCHAR)*resTextLen);
    MultiByteToWideChar(CP_THREAD_ACP, 0, (char*)resString.c_str(), resString.length(), resText, resTextLen);
    HWND editCtl = GetDlgItem(hDlg, IDC_EDIT1);
    SetDlgItemText(hDlg, IDC_EDIT1, resText);
    delete[] resText;

    /* 後処理 */
    HeapFree(GetProcessHeap(), 0, resBuf);
    InternetCloseHandle(hHttpRequest);
    InternetCloseHandle(hHttpSession);
    InternetCloseHandle(hInternet);
}

void run_capturetg(HWND hWnd, LPARAM lParam)
{
    bmp2png_buf_desc bmpbuf;
    bmpbuf.require(1024*768*3);

    // キャプチャ
    run_capture_target_window(&bmpbuf);

    // pngへ
    bmp2png_buf_desc pngbuf;
    pngbuf.require(1024*768*3);
    run_bmp2png(&pngbuf, &bmpbuf);

    // 保存
    run_filesave(&pngbuf);
    // 送信
    run_httpsend(&pngbuf);
}

void run_get_latestbmp(TCHAR* fileName){
    HKEY hkey;
    LONG regRes = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Microsoft Games\\Age of Empires Expansion\\1.0"),
      0, KEY_READ, &hkey);

    DWORD regType;
    TCHAR regData[MAX_PATH];
    DWORD regDataSize = MAX_PATH*sizeof(TCHAR);
    regRes =  RegQueryValueEx(hkey, _T("EXE Path"), NULL, &regType, (BYTE*) regData, &regDataSize);
    _tcscpy_s(fileName, MAX_PATH, regData);
    _tcscat_s(regData, MAX_PATH, _T("\\AoE*.bmp"));
    regRes = RegCloseKey(hkey);

    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(regData, &fd);

    if(hFind == INVALID_HANDLE_VALUE) {
        printf ("Invalid File Handle. Get Last Error reports %d\n", GetLastError ());
        assert(false);
        return;
    } 
    while(1){
        if(!FindNextFile(hFind, &fd)){
		    if( GetLastError() == ERROR_NO_MORE_FILES ){
    			break;
		    }else{
			    break;
		    }
        }
	}
    _tcscat_s(fileName, MAX_PATH, _T("\\"));
    _tcscat_s(fileName, MAX_PATH, fd.cFileName);
    FindClose(hFind);
}

int run_read_bmp(bmp2png_buf_desc* bmpbuf, const TCHAR *file)
{
    HANDLE fh = CreateFile(file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fh == INVALID_HANDLE_VALUE){
        assert(false);
        return -1;
    }
    DWORD fileSize = GetFileSize(fh, NULL);
    BYTE* filebuf = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);
    DWORD readSize;
    ReadFile(fh, filebuf, fileSize, &readSize, NULL);
    CloseHandle(fh);

    BITMAPFILEHEADER *bmpfh = (BITMAPFILEHEADER*) filebuf;
    BITMAPINFO *bmpi = (BITMAPINFO*)(filebuf + sizeof(BITMAPFILEHEADER));
    int bitCount = bmpi->bmiHeader.biBitCount;
    if(bitCount != 8){
        assert(false);
        HeapFree(GetProcessHeap(), 0, filebuf);
        return -2;
    }

    BYTE* pbits = filebuf + bmpfh->bfOffBits;
    int width = bmpi->bmiHeader.biWidth;
    int height = bmpi->bmiHeader.biHeight;
    int rowbytes = width*(bitCount/8);
    bmpbuf->width = width;
    bmpbuf->height = height;
    bmpbuf->require(width*height*3);

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            BYTE *pixels = (BYTE*) bmpbuf->hptr;
            RGBQUAD *colors = bmpi->bmiColors;
            int pos = y*rowbytes + x;
            pixels[3*pos+0] = colors[pbits[pos]].rgbRed;
            pixels[3*pos+1] = colors[pbits[pos]].rgbGreen;
            pixels[3*pos+2] = colors[pbits[pos]].rgbBlue;
        }
    }
    bmpbuf->length = width*height*3;

    HeapFree(GetProcessHeap(), 0, filebuf);
    return 0;
}

void run_filetg(HWND hWnd, LPARAM lParam)
{
    bmp2png_buf_desc bmpbuf;
    bmpbuf.require(1024*768*3);

    // キャプチャ
    TCHAR bmpfile[MAX_PATH];
    run_get_latestbmp(bmpfile);
    run_read_bmp(&bmpbuf, bmpfile);

    // pngへ
    bmp2png_buf_desc pngbuf;
    pngbuf.require(1024*768*3);
    run_bmp2png(&pngbuf, &bmpbuf);

    // 保存
    run_filesave(&pngbuf);
    // 送信
    run_httpsend(&pngbuf);
}
