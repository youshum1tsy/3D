// lab1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "lab1.h"

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
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1);
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

void scale(float& x, float& y, float& z, float scaleX, float scaleY, float scaleZ) {
    x = x * scaleX;
    y = y * scaleY;
    z = z * scaleZ;
}

void mirror(float& x, float& y, float& z) {
    x = x;
    y = y * -1;
    z = z;
}

void rotate(float& x, float& y, float& z, const int m, const int n ,int angleDegrees) {
    float angleRadians = angleDegrees * M_PI / 180.0f;
    float oldX = x;
    float oldY = y;
    x = (oldX * cos(angleRadians)) - (oldY * sin(angleRadians)) - (m * (cos(angleRadians) - 1)) + (n * sin(angleRadians));
    y = (oldX * sin(angleRadians)) + (oldY * cos(angleRadians)) - (n * (cos(angleRadians) - 1)) - (m * sin(angleRadians));
    z = z;
};


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

            const int sizeParam = 4;
            float matrix[][sizeParam] = {
                {800, 200, 1, 0}, // head
                {850, 150, 1, 1},
                {800, 100, 1, 1},
                {750, 150, 1, 1},
                {800, 200, 1, 1},
                {800, 250, 1, 1}, // neck     
                {950, 325, 1, 1}, // arm right
                {800, 250, 1, 0}, // neck     
                {650, 325, 1, 1}, // arm left
                {800, 250, 1, 0}, // neck     
                {800, 400, 1, 1}, // body
                {875, 500, 1, 1}, // leg right     
                {900, 500, 1, 1},
                {800, 400, 1, 0}, // body     
                {725, 500, 1, 1}, // leg left
                {700, 500, 1, 1},
                {700, 510, 1, 1},
            };
            const int sizeMove = sizeof(matrix);
            
            SetViewportOrgEx(hdc, 500, 500, NULL);

            int targetAngleDegrees = 45;

            float scaled_matrix[sizeMove][sizeParam];
            memcpy(scaled_matrix, matrix, sizeof(matrix));

            float mirrored_matrix[sizeMove][sizeParam];
            memcpy(mirrored_matrix, matrix, sizeof(matrix));

            float rotated_matrix[sizeMove][sizeParam];
            memcpy(rotated_matrix, matrix, sizeof(matrix));

            for (size_t i = 0; i < sizeMove; i++) {
                scale(scaled_matrix[i][0], scaled_matrix[i][1], scaled_matrix[i][2], 1, 0.5, 1);
                mirror(mirrored_matrix[i][0], mirrored_matrix[i][1], mirrored_matrix[i][2]);
                rotate(rotated_matrix[i][0], rotated_matrix[i][1], rotated_matrix[i][2], 800, 500, targetAngleDegrees);
                
            }

            for (size_t i = 0; i < sizeMove; i++) {
                if (scaled_matrix[i][3] == 0) {
                    MoveToEx(hdc, scaled_matrix[i][0], scaled_matrix[i][1], NULL);
                }
                else if (scaled_matrix[i][3] == 1) {
                    LineTo(hdc, scaled_matrix[i][0], scaled_matrix[i][1]);
                }
            }

            for (size_t i = 0; i < sizeMove; i++) {
                if (mirrored_matrix[i][3] == 0) {
                    MoveToEx(hdc, mirrored_matrix[i][0], mirrored_matrix[i][1], NULL);
                }
                else if (mirrored_matrix[i][3] == 1) {
                    LineTo(hdc, mirrored_matrix[i][0], mirrored_matrix[i][1]);
                }
            }

            for (size_t i = 0; i < sizeMove; i++) {
                if (rotated_matrix[i][3] == 0) {
                    MoveToEx(hdc, rotated_matrix[i][0], rotated_matrix[i][1], NULL);
                }
                else if (rotated_matrix[i][3] == 1) {
                    LineTo(hdc, rotated_matrix[i][0], rotated_matrix[i][1]);
                }
            }
			
            
            // TODO: Add any drawing code that uses hdc here...
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
