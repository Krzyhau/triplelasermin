#include "window.h"

LRESULT CALLBACK window_process(HWND, UINT, WPARAM, LPARAM);
void window_update(struct WindowHandler* window);
void window_draw(struct WindowHandler* window);

void window_init(struct WindowHandler* window)
{
    window->totalTime = 0.0f;
    window->deltaTime = 0.0f;
    window->lastClock = clock();

    WNDCLASSW wc = { 
        .lpszClassName = window->info.className,
        .hInstance = window->data.instance,
        .lpfnWndProc = window_process,
        .hCursor = LoadCursor(0, IDC_ARROW),
        .cbWndExtra = sizeof(struct WindowHandler*),
    };

    RegisterClassW(&wc);
    window->handle = CreateWindowW(
        window->info.className, window->info.titleName,
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        (GetSystemMetrics(SM_CXSCREEN) - window->info.width) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - window->info.height) / 2,
        window->info.width, window->info.height,
        NULL, NULL, window->data.instance, (LPVOID)window
    );

    //SetWindowLongPtr(window->handle, GWLP_USERDATA, (long*)window);

    window->input = malloc(sizeof(struct InputState));
    input_state_init(window->input);

    window->display = malloc(sizeof(struct Display));
    display_init(window->display, window->handle, window->info.pixelSize);

    window->data.startCallback(window);
}

int window_run(struct WindowHandler* window)
{
    MSG msg;

    while (GetMessage(&msg, window->handle, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void window_set_min_max(struct WindowHandler* window, LPMINMAXINFO lpMMI) {
    int minWidth = window->info.minWidth;
    int minHeight = window->info.minHeight;

    RECT rcClient, rcWindow;
    GetClientRect(window->handle, &rcClient);
    GetWindowRect(window->handle, &rcWindow);
    minWidth += (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
    minHeight += (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);

    lpMMI->ptMinTrackSize.x = minWidth;
    lpMMI->ptMinTrackSize.y = minHeight;
}

LRESULT CALLBACK window_process(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    struct WindowHandler* window = (struct WindowHandler*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_CREATE:
        window = (struct WindowHandler*)((CREATESTRUCT*)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);

        SetTimer(hwnd, 1, window->info.tickTimeMs, 0);

        break;
    case WM_TIMER:
        window_update(window);
        break;
    case WM_PAINT:
        window_draw(window);
        break;
    case WM_CLOSE:
        window->handle = NULL;
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        input_register_key_press(window->input, wParam);
        break;
    case WM_KEYUP:
        input_register_key_release(window->input, wParam);
        break;
    case WM_MOUSEMOVE:
        input_register_mouse_movement(window->input, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_GETMINMAXINFO:
        if(window != NULL){
            window_set_min_max(window, (LPMINMAXINFO)lParam);
        }
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void window_update_clock(struct WindowHandler* window) {
    clock_t currentClock = clock();
    window->deltaTime = (float)(currentClock - window->lastClock) / CLOCKS_PER_SEC;
    window->totalTime += window->deltaTime;
    window->lastClock = currentClock;
}

void window_lock_mouse(struct WindowHandler* window) {
    static int focusedLastTime = 0;

    DWORD threadId = GetWindowThreadProcessId(window->handle, NULL);

    GUITHREADINFO guiThreadInfo;
    ZeroMemory(&guiThreadInfo, sizeof(GUITHREADINFO));
    guiThreadInfo.cbSize = sizeof(GUITHREADINFO);
    GetGUIThreadInfo(threadId, &guiThreadInfo);

    if (window->input->mouseLocked == InputMouseLocked) {
        if (GetFocus() == window->handle && !(guiThreadInfo.flags & GUI_INMOVESIZE)) {
            RECT clientRect;
            GetClientRect(window->handle, &clientRect);
            POINT center = {
                .x = (clientRect.left + clientRect.right) / 2,
                .y = (clientRect.top + clientRect.bottom) / 2,
            };
            window->input->mouseX = center.x;
            window->input->mouseY = center.y;
        
            ClientToScreen(window->handle, &center);
            SetCursorPos(center.x, center.y);
        
            if (!focusedLastTime) {
                ShowCursor(0);
                focusedLastTime = 1;
            }
        }
        else {
            window->input->deltaMouseX = 0.0f;
            window->input->deltaMouseY = 0.0f;
        }
    }
    else {
        if (focusedLastTime) {
            ShowCursor(1);
            focusedLastTime = 0;
        }
    }
}

void window_request_redraw(struct WindowHandler* window) {
    RECT r;
    GetClientRect(window->handle, &r);
    RedrawWindow(window->handle, &r, NULL, RDW_INVALIDATE);
}

void window_update(struct WindowHandler* window) {
    window_update_clock(window);

    input_state_update(window->input);
    window_lock_mouse(window);
    window->data.updateCallback(window);

    window_request_redraw(window);
}

void window_draw(struct WindowHandler* window) {
    display_begin(window->display);
    window->data.drawCallback(window);
    display_end(window->display);
}