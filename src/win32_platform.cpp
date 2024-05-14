#include "platform.h"
#include "beans_lib.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// // globals
static HWND window;

// function implementations
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam){
    LRESULT result = 0;

    switch(msg) {
        case WM_CLOSE:{
            running = false;
            DestroyWindow(window);
            break;
        }

        default:{
            result = DefWindowProcA(window, msg, wParam, lParam);
            break;
        }
    }

    return result;
}
bool platform_create_window(int width, int height, char* title) {
    HINSTANCE instance = GetModuleHandleA(0);
    

    WNDCLASSA wc = {};
    wc.hInstance = instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title;
    wc.lpfnWndProc = windows_window_callback;
    
    if(!RegisterClassA(&wc)){
        return false; 
    }
    
    int dwstyle = WS_OVERLAPPEDWINDOW;
    window = CreateWindowExA(0, title, title, dwstyle, 100, 100, width, height, NULL, NULL, instance, NULL);

    if(window == NULL){
        return false;
    }
    ShowWindow(window, SW_SHOW);
    return true;
}

void platform_update_window(){
    MSG msg;

    while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
}

void* platform_load_gl_function(char* fnName) {
    PROC proc = wglGetProcAddress(fnName);
    if (!proc) {
        static HMODULE openGLDLL = LoadLibraryA("opengl32.dll");

        proc = GetProcAddress(openGLDLL, fnName);
        if(!proc) {
            SM_ASSERT(false, "Failed to load gl proc address");
            return nullptr;
        }
    }

    return (void*)proc;
}
