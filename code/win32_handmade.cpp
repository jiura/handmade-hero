#include <windows.h>

LRESULT CALLBACK MainWindowCallback(HWND   window,
                                    UINT   msg,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    LRESULT result = 0;

    switch (msg) {
    case WM_SIZE: {
        OutputDebugStringA("WM_SIZE\n");
    } break;

    case WM_DESTROY: {
        OutputDebugStringA("WM_DESTROY\n");
    } break;

    case WM_CLOSE: {
        PostQuitMessage(0);
        OutputDebugStringA("WM_CLOSE\n");
    } break;

    case WM_PAINT: {
        PAINTSTRUCT paint;
        HDC         deviceContext = BeginPaint(window, &paint);

        int x      = paint.rcPaint.left;
        int y      = paint.rcPaint.top;
        int width  = paint.rcPaint.right - paint.rcPaint.left;
        int height = paint.rcPaint.bottom - paint.rcPaint.top;

        static DWORD operation = WHITENESS;

        PatBlt(deviceContext,
               x, y, width, height,
               operation);

        if (operation == WHITENESS)
            operation = BLACKNESS;
        else
            operation = WHITENESS;

        EndPaint(window, &paint);
    } break;

    default: {
        // OutputDebugStringW("default\n");
        result = DefWindowProc(window, msg, wParam, lParam);
    } break;
    }

    return result;
}

int CALLBACK WinMain(HINSTANCE instance,
                     HINSTANCE prevInstance,
                     LPSTR     cmdLine,
                     int       showCode) {
    WNDCLASS windowClass = {};

    // TODO: Check if HREDRAW/VREDRAW/OWNDC still matter
    windowClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = MainWindowCallback;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = "HandmadeHeroWindowClass";

    if (RegisterClass(&windowClass)) {
        HWND windowHandle =
            CreateWindowEx(0,
                           windowClass.lpszClassName,
                           "Handmade Hero",
                           WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           0,
                           0,
                           instance,
                           0);

        if (windowHandle) {
            for (;;) {
                MSG  msg;
                BOOL msgResult = GetMessageA(&msg, 0, 0, 0);
                if (msgResult > 0) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                } else {
                    break;
                }
            }
        } else {
            // TODO: Logging
        }
    } else {
        // TODO: Logging
    }

    return 0;
}
