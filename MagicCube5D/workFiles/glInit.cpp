#include "glInit.h"

#include <stdafx.h>

#pragma managed(push, off)

#include <assert.h>

#include <string>

// A helper to get and display error information.
static void displayErrorMessage(std::string methodName) {
    assert(false);

    LPVOID lpMsgBuf = NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, GetLastError(),
                  0,  // Default language
                  (LPTSTR)&lpMsgBuf, 0, NULL);
    std::string tempString = (LPCSTR)lpMsgBuf;
    LocalFree(lpMsgBuf);

    tempString = "Method '" + methodName + "' failed.\n\nError was:" + tempString;
    MessageBoxA(NULL, tempString.c_str(), "Failed to initialize GL.", MB_ICONERROR);
}

CGlInit::CGlInit() {}

HGLRC
CGlInit::initializeGL(HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        displayErrorMessage("GetDC");
        return NULL;
    }

    return (initializeGL(hdc));
}

HGLRC CGlInit::initializeGL(HDC hContext) {
    if (!setupPixelFormat(hContext)) return NULL;

    HGLRC hrc = wglCreateContext(hContext);
    if (!hrc) {
        displayErrorMessage("wglCreateContext");
        return NULL;
    }

    if (!wglMakeCurrent(hContext, hrc)) displayErrorMessage("wglMakeCurrent");

    return hrc;
}

BOOL CGlInit::setupPixelFormat(HDC hContext) {

    // Fill in the Pixel Format Descriptor
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;                   // Version number
    pfd.dwFlags = PFD_DOUBLEBUFFER |    // Use double buffer
                  PFD_SUPPORT_OPENGL |  // Use OpenGL
                  PFD_DRAW_TO_WINDOW |  // Pixel format is for a window.
                  PFD_GENERIC_ACCELERATED;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;  // 24-bit color
    pfd.cDepthBits = 24;  // 24-bit z-buffer

    // Choose the pixel format.
    int nPixelFormatChosen = -1;
    nPixelFormatChosen = ChoosePixelFormat(hContext, &pfd);
    if (0 == nPixelFormatChosen) {
        displayErrorMessage("ChoosePixelFormat");
        return FALSE;
    }

    // Do some checking.
    {
        PIXELFORMATDESCRIPTOR temp;
        ZeroMemory(&temp, sizeof(temp));
        temp.nSize = sizeof(temp);
        DescribePixelFormat(hContext, nPixelFormatChosen, sizeof(temp), &temp);

        // Some boolean representation of these flags.
        bool acceleratedFlag =
                0 == (temp.dwFlags & PFD_GENERIC_ACCELERATED) ? false : true;
        bool softwareFlag = 0 == (temp.dwFlags & PFD_GENERIC_FORMAT) ? false : true;

        // Are we accelerated?
        if (!acceleratedFlag) {

            // Only log at this point if the software flag is set.
            if (softwareFlag) {
                assert(false);
                MessageBoxA(NULL,
                            "WARNING! The program may not be running in graphics "
                            "accelerated mode.",
                            "MagicCube5D", MB_ICONWARNING);
            }
        }

        // Did we get the right number of depth buffer bits?
        assert(temp.cDepthBits >= 24);
    }

    // Set it.
    if (!SetPixelFormat(hContext, nPixelFormatChosen, &pfd)) {
        displayErrorMessage("SetPixelFormat");
        return FALSE;
    }

    return TRUE;
}
#pragma managed(pop)
