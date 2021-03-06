// WindowsRayTracer.cpp : Defines the entry point for the application.
//


#include "header.h"
#include "WindowsRayTracer.h"
#include "Application.h"
#include "Profiler.h"
#include <stdint.h>

#define MAX_LOADSTRING 100

// Global Variables:
HWND	hWnd;									// current window handle
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

Application* pApp = nullptr;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSRAYTRACER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	// Application Initialization
	pApp = new Application();

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSRAYTRACER));

	MSG msg = { 0 };

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	//while (msg.message != WM_QUIT)
	//{
	//	if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//	else
	//	{
	//		pApp->Execute();
	//	}
	//}

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSRAYTRACER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSRAYTRACER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int, Application)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   pApp = new Application();

   RECT rect = { 0, 0, pApp->GetBufferWidth(), pApp->GetBufferHeight() };
   AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, true);

   hWnd = CreateWindow (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, rect.right-rect.left, rect.bottom-rect.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Initialize Application!

#ifdef NDEBUG
   pApp->Initialize(hWnd, true);
#else
   pApp->Initialize(hWnd, false);
#endif // NDEBUG

   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			
			case ID_FILE_SAVEIMAGE:
			{
				pApp->SaveImage();
				break;
			}

			case ID_FILE_DENOISEIMAGE:
			{
				OutputDebugString(L"\nDenoising...");
				pApp->DenoiseImage();
				OutputDebugString(L"\nDenoising Done!");
				break;
			}
		
			case ID_PROFILER_RENDERSTATS:
			{
				std::string debugInfo = Profiler::getInstance().GetProfilerTexts();
			
				wchar_t wBuffer[1024];
				mbstowcs(wBuffer, debugInfo.c_str(), debugInfo.size() + 1);
			
				MessageBox(hWnd, wBuffer, L"====== PROFILER ======", 0);
				break;
			}
			
            case IDM_EXIT:
			{
				DestroyWindow(hWnd);
				break;
			}
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

			pApp->Execute();

			OutputDebugString(L"This is paint!");

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
        
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
