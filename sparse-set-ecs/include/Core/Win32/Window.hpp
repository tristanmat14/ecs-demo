#pragma once

#include <windows.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <cstring>

#include "Color.hpp"

class Window {
private:
    HINSTANCE hInstance;
    HWND hWnd;
    int width;
    int height;
    const std::string class_name;
    std::vector<uint32_t> pixelBuffer;

    // cached bitmap resources
    HBITMAP hCachedBitmap;
    HDC hCachedDC;
    void* pBitmapBits;

    void destroyBitmap() {
        if (hCachedBitmap) {
            DeleteObject(hCachedBitmap);
            hCachedBitmap = nullptr;
        }
        if (hCachedDC) {
            DeleteDC(hCachedDC);
            hCachedDC = nullptr;
        }
        pBitmapBits = nullptr;
    }
    
    void createBitmap(HDC hdc) {
        destroyBitmap();

        hCachedDC = CreateCompatibleDC(hdc);

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height;  // Negative for top-down
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        
        hCachedBitmap = CreateDIBSection(hCachedDC, &bmi, DIB_RGB_COLORS, &pBitmapBits, nullptr, 0);
        
        if (hCachedBitmap) {
            SelectObject(hCachedDC, hCachedBitmap);
        }
    }
    
    void resize(int newWidth, int newHeight) {
        if (newWidth <= 0 || newHeight <= 0) return;

        width = newWidth;
        height = newHeight;

        pixelBuffer.resize(width * height, 0);
    }

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        Window* window = nullptr;

        // gets the window pointer
        if (uMsg == WM_CREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        } else {
            window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        switch(uMsg) {
            case WM_CLOSE:
                DestroyWindow(hWnd);
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_PAINT: {
                if (!window) break;

                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);

                if (window->hCachedBitmap && window->pBitmapBits) {
                    // copy pixel buffer to bitmap memory
                    std::memcpy(
                        window->pBitmapBits, window->pixelBuffer.data(),
                        window->pixelBuffer.size() * sizeof(uint32_t)
                    );

                    // Blit from cached DC to screen
                    BitBlt(hdc, 0, 0, window->width, window->height, window->hCachedDC, 0, 0, SRCCOPY);
                }

                EndPaint(hWnd, &ps);
                return 0;
            }
            case WM_SIZE: {
                if (!window) break;

                UINT newWidth = LOWORD(lParam);
                UINT newHeight = HIWORD(lParam);
                
                if (newWidth > 0 && newHeight > 0) {
                    window->resize(newWidth, newHeight);

                    // recreate bitmap for new size
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hWnd, &ps);
                    window->createBitmap(hdc);
                    EndPaint(hWnd, &ps);
                }

                return 0;
            }
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

public:
    Window(int w, int h, std::string title)
        : width(w), height(h), 
          hInstance(GetModuleHandle(nullptr)),
          class_name(title + "Class"),
          pixelBuffer(w * h, 0),
          hCachedBitmap(nullptr),
          hCachedDC(nullptr),
          pBitmapBits(nullptr)
    {
        WNDCLASS wndClass = {};
        wndClass.lpszClassName = class_name.c_str();
        wndClass.hInstance = hInstance;
        wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.lpfnWndProc = Window::WindowProc;
        wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

        if (!RegisterClass(&wndClass)) {
            throw std::runtime_error("Failed to register window class");
        }

        DWORD style = WS_OVERLAPPEDWINDOW; 

        // calculate width and height to include border and title
        RECT rect = {0, 0, width, height};
        AdjustWindowRect(&rect, style, FALSE);
        int windowWidth = rect.right - rect.left;
        int windowHeight = rect.bottom - rect.top;

        hWnd = CreateWindowEx(
            0,
            class_name.c_str(),
            title.c_str(),
            style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            windowWidth, windowHeight,
            nullptr, nullptr,
            hInstance, this 
        );

        if (!hWnd) {
            throw std::runtime_error("Failed to create window");
        }

        ShowWindow(hWnd, SW_SHOW);

        // create initial bitmap
        HDC hdc = GetDC(hWnd);
        createBitmap(hdc);
        ReleaseDC(hWnd, hdc);
    }

    ~Window() {
        destroyBitmap();

        if (hWnd) {
            DestroyWindow(hWnd);
        }
        UnregisterClass(class_name.c_str(), hInstance);
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool processMessages() {
        MSG msg = {};

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return false;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return true;
    }
   
    std::vector<uint32_t>& getPixelBuffer() {
        return pixelBuffer;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    /**
     * Clear the screen, a.k.a the pixel buffer with a single color
     */
    void clearScreen(Color color) {
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), RGB(color.b, color.g, color.r));
    }

    /**
     * Change a single pixels color in the pixel buffer
     */
    void drawPixel(int x, int y, Color color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixelBuffer[y * width + x] = RGB(color.b, color.g, color.r);
        }
    }

    /**
     * Draw a circle to the pixel buffer
     */
    void drawCircle(int centerX, int centerY, float radius, Color color) {
        int minY = centerY - radius;
        int maxY = centerY + radius;
        int minX = centerX - radius;
        int maxX = centerX + radius;

        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                float dx = x - centerX;
                float dy = y - centerY;
                float distSquared = dx * dx + dy * dy;

                if (distSquared > radius * radius) continue;
                drawPixel(x, y, color); 
            }
        }
    }

    /**
     * Draw a rectangle the pixel buffer
     * @note rectangle edges alligned with screen edges
     */
    void drawRectangle(int minX, int maxX, int minY, int maxY, Color color) {
        if (maxX <= minX || maxY <= minY) return;

        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                drawPixel(x, y, color);
            }
        }
    }

    /**
     * Draw a line to the pixel buffer using Bresenham's line algorithm
     */
    void drawLine(int x1, int y1, int x2, int y2, Color color) {
        float dx = std::abs(x2 - x1);
        float dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (x1 != x2 || y1 != y2) {
            drawPixel(x1, y1, color);

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    } 

    /**
     * Draws the pixel buffer to the window
     */
    void redraw() {
        InvalidateRect(hWnd, nullptr, FALSE);
        UpdateWindow(hWnd);
    }

};
