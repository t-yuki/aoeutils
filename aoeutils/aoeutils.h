#pragma once

#include "resource.h"
void run_foreground(HWND hWnd, LPARAM lParam);
void run_capturetg(HWND hWnd, LPARAM lParam);
void run_filetg(HWND hWnd, LPARAM lParam);

#define TARGET_WINDOW_NAME _T("Age of Empires Expansion")
#define WM_APP_RUN1 (WM_APP+0x40+0x01)
#define WM_APP_RUN2 (WM_APP+0x40+0x02)
