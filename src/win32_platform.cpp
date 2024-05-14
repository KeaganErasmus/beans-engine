#include "platform.h"
#include "beans_lib.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "wglext.h"
#include <glcorearb.h>

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

    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

    {
        window = CreateWindowExA(0, title, title, dwstyle, 100, 100, width, height, NULL, NULL, instance, NULL);

        if(window == NULL){
            return false;
        }

        HDC fakeDC = GetDC(window);

        PIXELFORMATDESCRIPTOR pfd = {0};
        pfd.nSize = 1;
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;

        int pixelFormat = ChoosePixelFormat(fakeDC, &pfd);

        if(!SetPixelFormat(fakeDC, pixelFormat, &pfd)) {
            return false;
        }

        HGLRC fakeRC = wglCreateContext(fakeDC);

        if(!wglMakeCurrent(fakeDC, fakeRC)){
            return false;
        }

        wglChoosePixelFormatARB = 
        (PFNWGLCHOOSEPIXELFORMATARBPROC)platform_load_gl_function("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)platform_load_gl_function("wglCreateContextAttribsARB");


        wglMakeCurrent(fakeDC, 0);
        wglDeleteContext(fakeRC);
        ReleaseDC(window, fakeDC);

        DestroyWindow(window);
    }

    // Actual OpenGl init
    {
        {
            RECT borderRect = {};
            AdjustWindowRectEx(&borderRect, dwstyle, 0, 0);

            width += borderRect.right - borderRect.left;
            height += borderRect.bottom - borderRect.bottom;
        }

        window = CreateWindowExA(0, title, title, dwstyle, 100, 100, width, height, NULL, NULL, instance, NULL);
        if(window == NULL){
            return false;
        }

        HDC dc = GetDC(window);
        const int pixelAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_SWAP_METHOD_ARB,    WGL_SWAP_COPY_ARB,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB,     32,
            WGL_ALPHA_BITS_ARB,     8,
            WGL_DEPTH_BITS_ARB,     24,
            0
        };

        UINT numPixelFormats;
        int pixelFormat = 0;
        if(!wglChoosePixelFormatARB(dc, pixelAttribs, 0, 1, &pixelFormat, &numPixelFormats)){
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd = {0};
        DescribePixelFormat(dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        if(!SetPixelFormat(dc, pixelFormat, &pfd)){
            return false;
        }

        const int contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,  4,
            WGL_CONTEXT_MINOR_VERSION_ARB,  6,
            WGL_CONTEXT_PROFILE_MASK_ARB,   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB,          WGL_CONTEXT_DEBUG_BIT_ARB,
            0
        };

        HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
        if(!wglMakeCurrent(dc, rc)){
            return false;
        }

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
