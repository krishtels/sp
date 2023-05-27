// lab05.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lab05.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING] = L"First";                // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING] = L"FIRST";               // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

UINT WM_WINDOW_INTERCONNECTION;

enum color {
    Red,
    Green,
    Blue
};

enum figure {
    Romb,
    Square,
    Circle,
    Star
};

typedef struct _drawprops {
    color cl;
    figure fg;
    BOOL draw;
}DRAWING_PROPERTIES;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB05));

    MSG msg;

    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB05));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 228, 240, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   WM_WINDOW_INTERCONNECTION = RegisterWindowMessage(L"WM_WINDOW_INTERCOONECTION");

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    COPYDATASTRUCT myDS;

    DRAWING_PROPERTIES dp;

    HWND hServer;

    static HWND rbRed;
    static HWND rbGreen;
    static HWND rbBlue;

    static HWND rbRomb;
    static HWND rbSquare;
    static HWND rbCircle;
    static HWND rbStar;

    static HWND cbDraw;

    if (message == WM_WINDOW_INTERCONNECTION) {
        hServer = FindWindow(NULL, L"Second");
        if (!hServer) {
            MessageBox(hWnd, L"Can't locate another window", L"First", MB_OK);
            return 0;
        }
        if (SendMessage(rbRed, BM_GETCHECK, 0, 0L) == BST_CHECKED) {
            dp.cl = color::Red;
        }
        else if (SendMessage(rbGreen, BM_GETCHECK, 0, 0L) == BST_CHECKED) {
            dp.cl = color::Green;
        }
        else if (SendMessage(rbBlue, BM_GETCHECK, 0, 0L) == BST_CHECKED) {
            dp.cl = color::Blue;
        }
        if (SendMessage(rbRomb, BM_GETCHECK, 0, 0L) == BST_CHECKED) {
            dp.fg = figure::Romb;
        }
        else if (SendMessage(rbSquare, BM_GETCHECK, 0, 0L) == BST_CHECKED) {
            dp.fg = figure::Square;
        }
        else if (SendMessage(rbCircle, BM_GETCHECK, 0, 0L) == BST_CHECKED) {
            dp.fg = figure::Circle;
        }
        else if (SendMessage(rbStar, BM_GETCHECK, 0, 0L) == BST_CHECKED) {
            dp.fg = figure::Star;
        }
        if (SendMessage(cbDraw, BM_GETCHECK, 0, 0L) == BST_CHECKED)
            dp.draw = TRUE;
        else dp.draw = FALSE;

        myDS.dwData = 1;
        myDS.cbData = sizeof(DRAWING_PROPERTIES);
        myDS.lpData = &dp;
        SendMessage(hServer, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)(LPVOID)&myDS);
        return 0;
    }

    switch (message)
    {

    case WM_CREATE:
    {
        rbRed = CreateWindow(L"button", L"RED", WS_CHILD |
            WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFTTEXT
            | WS_GROUP | WS_TABSTOP,
            0, 0, 100, 50, hWnd, (HMENU)RB_COLOR_RED, hInst, NULL);
        rbGreen = CreateWindow(L"button", L"GREEN", WS_CHILD |
            WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFTTEXT,
            0, 50, 100, 50, hWnd, (HMENU)RB_COLOR_GREEN, hInst, NULL);
        rbBlue = CreateWindow(L"button", L"BLUE", WS_CHILD |
            WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFTTEXT,
            0, 100, 100, 50, hWnd, (HMENU)RB_COLOR_BLUE, hInst, NULL);

        rbRomb = CreateWindow(L"button", L"ROMB", WS_CHILD |
            WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFTTEXT |
            WS_GROUP | WS_TABSTOP,
            110, 0, 100, 50, hWnd, (HMENU)RB_FIGURE_ROMB, hInst, NULL);
        rbSquare = CreateWindow(L"button", L"SQUARE", WS_CHILD |
            WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFTTEXT,
            110, 50, 100, 50, hWnd, (HMENU)RB_FIGURE_SQUARE, hInst, NULL);
        rbCircle = CreateWindow(L"button", L"CIRCLE", WS_CHILD |
            WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFTTEXT,
            110, 100, 100, 50, hWnd, (HMENU)RB_FIGURE_CIRCLE, hInst, NULL);
        rbStar = CreateWindow(L"button", L"STAR", WS_CHILD |
            WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFTTEXT,
            110, 150, 100, 50, hWnd, (HMENU)RB_FIGURE_STAR, hInst, NULL);

        cbDraw = CreateWindow(L"button", L"DRAW", WS_CHILD |
            WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT,
            0, 150, 100, 50, hWnd, (HMENU)CB_DRAW, hInst, NULL);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        switch (wmId)
        {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

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
