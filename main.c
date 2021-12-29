#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <gl/glew.h>

const int width = 1024;
const int height = 728;
bool running = true;

LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL createGLContext(HWND hwnd);
HWND createWindow(HINSTANCE instance, const wchar_t className[], int width, int height);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nShowCmd) {
    WNDCLASS wc = {0};

    const wchar_t className[] = L"App Class";

    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBoxA(0, "Failed to register class", "ERROR", 0);
        return -1;
    }

    HWND hwnd = createWindow(hInstance, className, width, height);
    ShowWindow(hwnd, nShowCmd);

    if (hwnd == NULL) {
        MessageBoxA(0, "Failed to open window", "ERROR", 0);
        return -1;
    }

    while (running) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	return 0;
}

HWND createWindow(HINSTANCE instance, const wchar_t className[], int width, int height) {
    HWND hwnd = CreateWindowEx(
        0,
        className,
        L"App",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        0,
        0,
        instance,
        0);

    if (!hwnd) {
        return NULL;
    }
    return hwnd;
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CHAR: {
            if (GetAsyncKeyState('Q')) {
                PostQuitMessage(0);
            }
        } break;

        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;

        default: {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    return 0;
}
