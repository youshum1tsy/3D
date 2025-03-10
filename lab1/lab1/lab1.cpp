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

const WPARAM KEY_Q = 0x51;
const WPARAM KEY_W = 0x57;
const WPARAM KEY_E = 0x45;
const WPARAM KEY_R = 0x52;

enum class Transform {
    ORIGINAL,
    MIRROR,
    ROTATE,
    SCALE
};

void centerMatrix(float matrix[][4], size_t rows, HWND hWnd, float moveTransform[3][3]) {
    float xMin = FLT_MAX;
    float yMin = FLT_MAX;
    float xMax = -FLT_MAX;
    float yMax = -FLT_MAX;

    for (size_t i = 0; i < rows; i++) {
        if (matrix[i][0] < xMin) {
            xMin = matrix[i][0];
        }
        if (matrix[i][0] > xMax) {
            xMax = matrix[i][0];
        }
        if (matrix[i][1] < yMin) {
            yMin = matrix[i][1];
        }
        if (matrix[i][1] > yMax) {
            yMax = matrix[i][1];
        }
    }

    float xCenter = (xMin + xMax) / 2;
    float yCenter = (yMin + yMax) / 2;

    RECT rect;
    GetClientRect(hWnd, &rect);
    float windowCenterX = (rect.right - rect.left) / 2.0f;
    float windowCenterY = (rect.bottom - rect.top) / 2.0f;

    float moveX = windowCenterX - xCenter;
    float moveY = windowCenterY - yCenter;

    moveTransform[0][0] = 1.0f;
    moveTransform[0][1] = 0.0f;
    moveTransform[0][2] = 0.0f;

    moveTransform[1][0] = 0.0f;
    moveTransform[1][1] = 1.0f;
    moveTransform[1][2] = 0.0f;

    moveTransform[2][0] = moveX;
    moveTransform[2][1] = moveY;
    moveTransform[2][2] = 1.0f;
}

void moveMatrix(float matrix[][4], size_t rows, float move[3][3]) {
    for (size_t i = 0; i < rows; i++) {
        matrix[i][0] += move[2][0];
        matrix[i][1] += move[2][1];
    }
}

void scaleMatrix(float matrix[][4], size_t rows, float scale[3][3]) {
    for (size_t i = 0; i < rows; i++) {
        matrix[i][0] *= scale[0][0];
        matrix[i][1] *= scale[1][1];
        matrix[i][2] *= scale[2][2];
    }
}

void mirrorMatrix(float matrix[][4], size_t rows, float mirror[3][3]) {
    for (size_t i = 0; i < rows; i++) {
        matrix[i][0] *= mirror[0][0];
        matrix[i][1] *= mirror[1][1];
        matrix[i][2] *= mirror[2][2];
    }
}

void rotateMatrix(float matrix[][4], size_t rows, float rotate[3][3]) {
    for (size_t i = 0; i < rows; i++) {
        float oldX = matrix[i][0];
        float oldY = matrix[i][1];
        matrix[i][0] = oldX * rotate[0][0] + oldY * rotate[1][0] + rotate[2][0];
        matrix[i][1] = oldX * rotate[0][1] + oldY * rotate[1][1] + rotate[2][1];
    }
}

void paint(HDC& hdc, float matrix[][4], size_t rows) {
    for (size_t i = 0; i < rows; i++) {
        if (matrix[i][3] == 0) {
            MoveToEx(hdc, matrix[i][0], matrix[i][1], NULL);
        }
        else if (matrix[i][3] == 1) {
            LineTo(hdc, matrix[i][0], matrix[i][1]);
        }
    }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static float matrix[][4] = {
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

    static const size_t rows = sizeof(matrix) / sizeof(matrix[0]);
    static float scaledFigure[rows][4];
    static float mirroredFigure[rows][4];
    static float rotatedFigure[rows][4];
    static bool initialized = false;
    static Transform currentFigure = Transform::ORIGINAL;

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

            float angleRadians = 45 * M_PI / 180.0f;

            float scaleTransform[3][3]{
                {1, 0,  0},
                {0, 0.5, 0},
                {0, 0, 1} 
            };

            float mirrorTransform[3][3]{
                {1, 0 ,0},
                {0, -1, 0},
                {0, 0, 1}
            };

            float xMin = FLT_MAX;
            float yMin = FLT_MAX;
            float xMax = -FLT_MAX;
            float yMax = -FLT_MAX;

            for (size_t i = 0; i < rows; i++) {
                if (matrix[i][0] < xMin) {
                    xMin = matrix[i][0];
                }
                if (matrix[i][0] > xMax) {
                    xMax = matrix[i][0];
                }
                if (matrix[i][1] < yMin) {
                    yMin = matrix[i][1];
                }
                if (matrix[i][1] > yMax) {
                    yMax = matrix[i][1];
                }
            }
            float xCenter = (xMin + xMax) / 2;
            float yCenter = (yMin + yMax) / 2;

            float rotateTransform[3][3]{
                {cos(angleRadians), sin(angleRadians), 0},
                {-sin(angleRadians), cos(angleRadians), 0},
                {-xCenter * (cos(angleRadians) - 1) + yCenter * sin(angleRadians), -yCenter * (cos(angleRadians) - 1) - xCenter * sin(angleRadians), 1}
            };

            RECT rect;
            GetClientRect(hWnd, &rect);

            float windowCenterX = (rect.right - rect.left) / 2.0f;
            float windowCenterY = (rect.bottom - rect.top) / 2.0f;

            float moveX = windowCenterX - xCenter;
            float moveY = windowCenterY - yCenter;

            float moveTransform[3][3] = {
                {1, 0, 0},
                {0, 1, 0},
                {moveX, moveY, 1}
            };


            if (!initialized) {

                memcpy(scaledFigure, matrix, sizeof(matrix));
                memcpy(mirroredFigure, matrix, sizeof(matrix));
                memcpy(rotatedFigure, matrix, sizeof(matrix));

                scaleMatrix(scaledFigure, rows, scaleTransform);
                mirrorMatrix(mirroredFigure, rows, mirrorTransform);
                rotateMatrix(rotatedFigure, rows, rotateTransform);

                float moveOriginalTransform[3][3];
                float moveScaledTransform[3][3];
                float moveMirroredTransform[3][3];
                float moveRotatedTransform[3][3];

                centerMatrix(matrix, rows, hWnd, moveOriginalTransform);
                centerMatrix(scaledFigure, rows, hWnd, moveScaledTransform);
                centerMatrix(mirroredFigure, rows, hWnd, moveMirroredTransform);
                centerMatrix(rotatedFigure, rows, hWnd, moveRotatedTransform);

                moveMatrix(matrix, rows, moveOriginalTransform);
                moveMatrix(scaledFigure, rows, moveScaledTransform);
                moveMatrix(mirroredFigure, rows, moveMirroredTransform);
                moveMatrix(rotatedFigure, rows, moveRotatedTransform);

                initialized = true;
            }

            switch (currentFigure)
            {
            case Transform::SCALE: 
                paint(hdc, scaledFigure, rows);
                break;
            case Transform::MIRROR:
                paint(hdc, mirroredFigure, rows);
                break;
            case Transform::ROTATE: 
                paint(hdc, rotatedFigure, rows);
                break;
            default:
                paint(hdc, matrix, rows);
                break;
            }

            EndPaint(hWnd, &ps);
        }
        break;


    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case KEY_Q:
            currentFigure = Transform::ORIGINAL;
            break;
        case KEY_W: 
            currentFigure = Transform::MIRROR;
            break;
        case KEY_E: 
            currentFigure = Transform::ROTATE;
            break;
        case KEY_R: 
            currentFigure = Transform::SCALE;
            break;
        default:
            return 0;
        }
        InvalidateRect(hWnd, NULL, TRUE);
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
