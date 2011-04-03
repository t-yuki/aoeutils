#pragma once

#include "resource.h"
void run1(HWND hWnd, LPARAM lParam);
void run2(HWND hWnd, LPARAM lParam);

#define TARGET_WINDOW_NAME _T("Age of Empires Expansion")
#define WM_APP_RUN1 (WM_APP+0x40+0x01)
