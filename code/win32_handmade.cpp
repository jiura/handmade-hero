#include <windows.h>

#define internal static
#define local_persist static
#define global static

// TODO: This is a global for now
global bool _running;

global BITMAPINFO _bitmapInfo;
global void      *_bitmapMemory;
global HBITMAP    _bitmapHandle;
global HDC        _bitmapDeviceContext;

internal void
win32ResizeDIBSection(int width, int height) {
    // TODO: Bulletproof this
    // Maybe don't free first. Free after, then free first if that fails.

    if (_bitmapHandle) {
        DeleteObject(_bitmapHandle);
    }

    if (!_bitmapDeviceContext) {
        // TODO: Should we recreate these under certain special circumstances?
        _bitmapDeviceContext = CreateCompatibleDC(0);
    }

    _bitmapInfo.bmiHeader.biSize        = sizeof(_bitmapInfo.bmiHeader);
    _bitmapInfo.bmiHeader.biWidth       = width;
    _bitmapInfo.bmiHeader.biHeight      = height;
    _bitmapInfo.bmiHeader.biPlanes      = 1;
    _bitmapInfo.bmiHeader.biBitCount    = 32;
    _bitmapInfo.bmiHeader.biCompression = BI_RGB;

    _bitmapDeviceContext = CreateCompatibleDC(0);

    _bitmapHandle = CreateDIBSection(_bitmapDeviceContext,
                                     &_bitmapInfo,
                                     DIB_RGB_COLORS,
                                     &_bitmapMemory,
                                     0, 0);
}

internal void win32UpdateWindow(HDC deviceContext,
                                int x,
                                int y,
                                int width,
                                int height) {
    StretchDIBits(deviceContext,
                  x, y, width, height,
                  x, y, width, height,
                  _bitmapMemory,
                  &_bitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
win32MainWindowCallback(HWND   window,
                        UINT   msg,
                        WPARAM wParam,
                        LPARAM lParam) {
    LRESULT result = 0;

    switch (msg) {
        case WM_SIZE: {
            RECT clientRect;
            GetClientRect(window, &clientRect);

            int width  = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;

            win32ResizeDIBSection(width, height);
        } break;

        case WM_ACTIVATEAPP: {
            OutputDebugStringA("WM_ACTIVATEAPP");
        } break;

        case WM_CLOSE: {
            // TODO: Handle this with a message to the user?
            _running = false;
        } break;

        case WM_DESTROY: {
            // TODO: Handle this as an error - recreate window?
            _running = false;
        } break;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC         deviceContext = BeginPaint(window, &paint);

            int x      = paint.rcPaint.left;
            int y      = paint.rcPaint.top;
            int width  = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;

            win32UpdateWindow(deviceContext, x, y, width, height);

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
    windowClass.lpfnWndProc   = win32MainWindowCallback;
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
            _running = true;

            while (_running) {
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
