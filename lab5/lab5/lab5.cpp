#include <iostream>
#include <cmath>
#include "lab5.h"
#include "framework.h"
#define MAX_LOADSTRING 100

// Глобальные переменные

double cre_start = -2;
double cre_finish = 2;

double cim_start = -2;
double cim_finish = 2;

int scr_y = 600;
int scr_x = 600;

bool Julia = true;
//double cre = -0.7069;
//double cim = 0.1889;
double cre = 0.24123;
double cim = 0.7213;
int filterType = 0;


HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

// Функции:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MSG msg;
    HACCEL hAccelTable;

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_LAB5, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB5));

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_LAB5));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_LAB5);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;

    // Создание окна с новым стилем
    hWnd = CreateWindow(
        szWindowClass, // Имя класса окна
        szTitle, // Заголовок окна
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Стиль окна
        CW_USEDEFAULT, 0, // Позиция окна
        600, 600, // Размер окна (ширина, высота)
        NULL, // Родительское окно
        NULL, // Меню
        hInstance, // Экземпляр приложения
        NULL // Дополнительные параметры
    );

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
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
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            DestroyWindow(hWnd);
            break;
        case 'J':
            Julia = !Julia;
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        double step_x = (cre_finish - cre_start) / scr_x;
        double step_y = (cim_finish - cim_start) / scr_y;

        for (int x = 0; x < scr_x; x++)
        {
            for (int y = 0; y < scr_y; y++)
            {
                double scaled_x = cre_start + x * step_x;
                double scaled_y = cim_start + y * step_y;

                // Инициализация z и c
                double newRe;
                double newIm;
                double oldRe;
                double oldIm;

                if (Julia)
                {
                    newRe = scaled_x;
                    newIm = scaled_y;
                }
                else
                {
                    newRe = 0;
                    newIm = 0;
                    cre = scaled_x;
                    cim = scaled_y;
                }

                // Итерационный процесс
                int i;
                for (i = 0; i < 256; i++)
                {
                    oldRe = newRe;
                    oldIm = newIm;

                    // z = z^3 + c
                    double re_part = pow(oldRe, 3)- 3 * oldRe * pow(oldIm, 2);
                    double im_part = 3 * pow(oldRe,2) * oldIm - pow(oldIm, 3);

                    newRe = re_part + cre;
                    newIm = im_part + cim;

                    // Проверка условия |z| > 2
                    if ((newRe * newRe + newIm * newIm) > 4)
                        break;
                }

                // Определение цвета пикселя
                if (i == 256)
                {
                    SetPixel(hdc, x, y, RGB(0, 0, 0)); // Черный цвет внутри множества
                }
                else
                {
                    int color = i % 256;
                    SetPixel(hdc, x, y, RGB(color * 10, color * 15, color * 3));
                }
            }
        }

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