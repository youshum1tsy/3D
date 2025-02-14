// lab2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "lab2.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
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

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
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
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...


            // Создаем контекст памяти для фона
            HDC memdc = CreateCompatibleDC(hdc);

            // Загружаем фоновое изображение
            HBITMAP bgBitMap = (HBITMAP)LoadImage(NULL, L"..\\assets\\bg-2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            SelectObject(memdc, bgBitMap);

            // Отображаем фон
            BitBlt(hdc, 0, 0, 800, 600, memdc, 0, 0, SRCCOPY);

            // Создаем контекст памяти для спрайтов
            HDC spriteMemdc = CreateCompatibleDC(hdc);

            // Загружаем фазы спрайта персонажа
            std::vector<HBITMAP> personBitMaps;
            for (int i = 0; i < 6; i++) {
                std::wstring personPath = L"..\\assets\\person-dark-0" + std::to_wstring(i + 1) + L".bmp";
                personBitMaps.push_back((HBITMAP)LoadImage(NULL, personPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
            }

            // Координаты персонажа
            int personX = 0;
            int personY = 400;
            int frameIndex = 0;

            // Флаг для остановки анимации
            bool isRunning = true;

            // Основной цикл анимации
            while (isRunning) {
                // Обработка сообщений (включая нажатия клавиш)
                MSG msg;
                if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                    if (msg.message == WM_KEYDOWN && msg.wParam == 'Q') { // Если нажата клавиша 'Q'
                        isRunning = false; // Останавливаем анимацию
                    }
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                // Отображаем фон (очищаем экран)
                BitBlt(hdc, 0, 0, 800, 600, memdc, 0, 0, SRCCOPY);

                // Выбираем текущую фазу спрайта персонажа в контекст памяти для спрайтов
                SelectObject(spriteMemdc, personBitMaps[frameIndex]);

                // Отображаем персонажа
                BitBlt(hdc, personX, personY, 100, 100, spriteMemdc, 0, 0, SRCINVERT);

                // Задержка между кадрами
                Sleep(100);

                // Стираем персонажа (восстанавливаем фон)
                BitBlt(hdc, personX, personY, 100, 100, spriteMemdc, 0, 0, SRCINVERT);

                // Переходим к следующей фазе
                frameIndex = (frameIndex + 1) % personBitMaps.size();

                // Перемещаем персонажа
                personX += 10;
                if (personX > 800) personX = 0; // Если персонаж вышел за границу экрана, возвращаем его в начало
            }


            // 
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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
