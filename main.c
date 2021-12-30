#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN // the purposes of these defines is to reduce unnecessary includes via windows.h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <windows.h>
#include <windowsx.h>
#include <gl/glew.h> // used to load function pointers for OpenGL.
#include <gl/wglew.h> // used for creating the modern OpenGL context.

#include "include/shader.h"

const int width = 1600;
const int height = 1200;
bool running = true;
HDC dc; // This tracks the "device context", a handle to the window.
HGLRC rc; // Represents a handle to a OpenGL context (rendering context, not the same as device context).

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

    if (!RegisterClass(&wc)) { // every windows window has to be associated with a window class.
        MessageBoxA(0, "Failed to register class", "ERROR", 0);
        return GetLastError();
    }

    HWND hwnd = createWindow(hInstance, className, width, height);

    if (hwnd == NULL) {
        MessageBoxA(0, "Failed to open window", "ERROR", 0);
        return GetLastError();
    }

    ShowWindow(hwnd, nShowCmd);

    SpirvData vertexShaderData = {0};
    SpirvData fragmentShaderData = {0};
    Shader shader;

    if (!readBinaryFileIntoSpirvData("spirv/triangle_shader.vert.spv", &vertexShaderData)) return -1;
    if (!readBinaryFileIntoSpirvData("spirv/triangle_shader.frag.spv", &fragmentShaderData)) return -1;

    if (!compileShaders(&shader, &vertexShaderData, &fragmentShaderData)) return -1;

    float vertices[] = {
    //  x      y     z      r     g     b
      -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
       0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
       0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    unsigned int vbo, vao;
    glCreateVertexArrays(1, &vao);

    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, 6 * sizeof(float));

    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);

    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, 0);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, false, 3 * sizeof(float));

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);

    while (running) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        glClearColor(0.92f, 0.58f, 0.39f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader.program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SwapBuffers(dc);
    }
	return 0;
}

BOOL createGLContext(HWND hwnd) {
    // if you are curious about these values "mean", they come from here:
    // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-setpixelformat
    PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
            1,                     // version number  
            PFD_DRAW_TO_WINDOW |   // support window  
            PFD_SUPPORT_OPENGL |   // support OpenGL  
            PFD_DOUBLEBUFFER,      // double buffered  
            PFD_TYPE_RGBA,         // RGBA type  
            32,                    // 32-bit color depth  
            0, 0, 0, 0, 0, 0,      // color bits ignored  
            0,                     // no alpha buffer  
            0,                     // shift bit ignored  
            0,                     // no accumulation buffer  
            0, 0, 0, 0,            // accum bits ignored  
            32,                    // 32-bit z-buffer      
            16,                    // 16-bit stencil buffer  
            0,                     // no auxiliary buffer  
            PFD_MAIN_PLANE,        // main layer  
            0,                     // reserved  
            0, 0, 0                // layer masks ignored  
    };

    dc = GetDC(hwnd);
    if (dc == NULL) return false;

    int pf = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, pf, &pfd);

    HGLRC tempContext = wglCreateContext(dc); // temp context which is needed to connect to OpenGL 3.2+ versions
    wglMakeCurrent(dc, tempContext); // temp as well so we can call ->wgl<-createcontext...

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        return false;
    }

    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        4,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        6,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
        0
    };

    rc = wglCreateContextAttribsARB(dc, 0, attribs); // it is at this call you need to have a placeholder context created.

    wglMakeCurrent(NULL, NULL); // we can have a modern context, forget this one.
    wglDeleteContext(tempContext);

    if (!wglMakeCurrent(dc, rc)) return false;

    int numExt = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
    for (int i = 0; i < numExt; i++)
        if (!strcmp((char*)glGetStringi(GL_EXTENSIONS, i), "GL_ARB_compatibility")) {
            MessageBoxA(0, "If this prints ITS COMPATABILITY, NOT WANTED!", "INCORRECT PROFILE", 0);
            return false;
        }

    glViewport(0, 0, width, height); // this fine to call here or before the message loop in main, it does not matter.

    //MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0); // optional to see the version retrieved

    return true;
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
        case WM_ACTIVATE: { // when the window is actived, create the context.
            if (!createGLContext(hwnd)) {
                MessageBoxA(hwnd, L"Failed to initialize OpenGL context", L"ERROR", 0);
                PostQuitMessage(0);
            }
        }

        case WM_CHAR: {
            if (GetAsyncKeyState('Q')) { // destroy the context and close the window when the user presses 'Q'.
                ReleaseDC(hwnd, dc);
                wglDeleteContext(rc);
                PostQuitMessage(0);
            }
        } break;

        case WM_DESTROY: {
            ReleaseDC(hwnd, dc);
            wglDeleteContext(rc);
            PostQuitMessage(0);
        } break;

        default: {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    return 0;
}