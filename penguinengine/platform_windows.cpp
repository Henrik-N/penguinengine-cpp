module;
#include "defines.h"

#ifdef PENGUIN_PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif

#include <iostream>
#include <format>

#include <vector>

module platform;


#ifdef PENGUIN_PLATFORM_WINDOWS

#ifdef PENGUIN_RENDERER_VULKAN
namespace platform {
    namespace vulkan {
        const char* getRequiredExtensionName() OK {
            return "VK_KHR_win32_surface";
        }
        InternalState readInternalState() OK {
            return platformState.internalState;
        }
    }
}
#endif


namespace platform {

    SystemClock::SystemClock() OK {
        // get processor clock frequency
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);

        // LARGE_INTEGER is a union and the QuadPart is used to store a 64-bit integer on compilers with built-in support for 64-bit. 
        // Converting it into a floating point value
        this->frequency = 1.0 / static_cast<f64>(frequency.QuadPart);

        // set the current time for setting 
        QueryPerformanceCounter(&this->startTime);
    }

    f64 SystemClock::getAbsoluteTime() const OK {
        // get number of cycles since the app started 
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        // 
        return static_cast<f64>(currentTime.QuadPart) * this->frequency;
    }

    f64 getAbsoluteTime() OK {
        return platformState.systemClock.getAbsoluteTime();
    }


    LRESULT CALLBACK win32ProcessMessage(HWND window, u32 msg, WPARAM wParam, LPARAM lParam);

    struct WindowRect {
        u32 xPos;
        u32 yPos;
        u32 width;
        u32 height;

        void pushBordersOut(RECT adjustmentRect) OK {
            // The adjustment rectangle is a is a measurement of how different the client rect
            // is compared to the outer window rect.
            // 
            // The adjustment border rect will be negative, suggesting
            //   that the client rect is smaller than the window rect.

            // We expand the outer window rect, moving the borders out.
            //  This way, the contents of the entire client rect will be displayed, 
            //  and then the borders, will be displayed outside of it (rather than 
            //  the borders being inside the client rect).

            xPos += adjustmentRect.left;
            yPos += adjustmentRect.top;
            width += adjustmentRect.right - adjustmentRect.left;
            height += adjustmentRect.bottom - adjustmentRect.top;
        }
    };

    constexpr LPCSTR windowClassName = "PenguinWindowClass";

    void tryCreateHWindow(HWND& hWindow, const PlatformInitInfo& initInfo, HINSTANCE hInstance) {
        const u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION
            | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
        const u32 window_ex_style = WS_EX_APPWINDOW;

        // client window ("inner" window, without borders etc)
        WindowRect window{
            .xPos = initInfo.x,
            .yPos = initInfo.y,
            .width = initInfo.width,
            .height = initInfo.height,
        };

        // get border size
        RECT adjustment_rect = { 0, 0, 0, 0 };
        AdjustWindowRectEx(&adjustment_rect, window_style, 0, window_ex_style);

        window.pushBordersOut(adjustment_rect);

        hWindow = CreateWindowExA(
            window_ex_style, windowClassName, initInfo.application_name,
            window_style, window.xPos, window.yPos, window.width, window.height,
            0, 0, hInstance, 0
        );

        if (!hWindow) {
            const char* errMsg = "Platform: failed to create window!";
            MessageBoxA(nullptr, errMsg, "Error!", MB_ICONEXCLAMATION | MB_OK);

            printf("%s", errMsg);

            throw new std::runtime_error(errMsg);
        }
    }

    void init(const PlatformInitInfo& initInfo) {
        const HINSTANCE hInstance = GetModuleHandleA(0);

        const HICON icon = LoadIcon(hInstance, IDI_APPLICATION);

        const char* wClassName = windowClassName;

        WNDCLASSA wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_DBLCLKS; // capture double clicks
        // Pointer to the window procedure.
        //  Events that need to be handled
        wc.lpfnWndProc = win32ProcessMessage;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = icon;
        // we'll manage the cursor by ourselves
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszClassName = windowClassName;

        if (!RegisterClassA(&wc)) {
            const char* errMsg = "Windows: failed to register window";
            MessageBoxA(0, errMsg, "Error", MB_ICONEXCLAMATION | MB_OK);
            throw new std::runtime_error(errMsg);
        }

        const u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION
            | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
        const u32 window_ex_style = WS_EX_APPWINDOW;


        HWND hWindow;
        tryCreateHWindow(hWindow, initInfo, hInstance);

        // window creation successful
        const InternalState internal_state = {
            .instance = hInstance,
            .window = hWindow,
        };

        // display window
        const bool shouldActivate = true; // window should accept input
        const i32 shouldShowWindowFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
        ShowWindow(internal_state.window, shouldShowWindowFlags);

        platformState = platform::PlatformState{
            .internalState = internal_state,
            .systemClock = SystemClock(),
        };
    }

    void deinit() OK {
        const auto window = platformState.internalState.window;

        if (window != nullptr) {
            DestroyWindow(window);
        } else {
            ERROR("platform: failed to deinit window");
        }
    }

    void popMessages() OK {
        // while there is a message in the message stack, pop it
       //  from the stack and put it in the MSG struct
        MSG msg;
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg); // dispatch to win32ProcessMessage
        }
    }

    enum Color : u8 {
        Black = 0x0,
        Blue = 0x1,
        Green = 0x2,
        Aqua = 0x3,
        Red = 0x4,
        Purple = 0x5,
        Yellow = 0x6,
        White = 0x7,
        Gray = 0x8,
        LightBlue = 0x9,
        LightGreen = 0xA,
        LightAqua = 0xB,
        LightRed = 0xC,
        LightPurple = 0xD,
        LightYellow = 0xE,
        BrightWhite = 0xF,
    };

    consteval u8 colorCode(Color foreground, Color background) OK {
        const u8 foreground_u8 = static_cast<u8>(foreground);
        const u8 background_u8 = static_cast<u8>(background);
        // The first half of the byte is the background, the second is the foreground.
        // Left shift the background value half a byte and combine it with the foreground.
        return (background_u8 << 4) | foreground_u8;
    }

    constexpr u8 colorFromLogLevel(u8 logLevel) {
        switch (logLevel) {
            // fatal
            case 0: return colorCode(Black, Red);
                // error
            case 1: return colorCode(Red, Black);
                // warning
            case 2: return colorCode(Yellow, Black);
                // info
            case 3: return colorCode(LightGreen, Black);
                //debug
            case 4: return colorCode(LightPurple, Black);
                //trace
            default: return colorCode(Gray, Black);
        }
    }

    void writeConsoleMessage(const stringv msgTag, const stringv msg, u8 logLevel) OK {
        const u8 color = colorFromLogLevel(logLevel);
        const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
        std::cout << msgTag << " " << msg << "\n";
    }

    void writeConsoleError(const stringv msgTag, const stringv msg, u8 logLevel) OK {
        const u8 color = colorFromLogLevel(logLevel);
        const HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
        std::cerr << msgTag << " " << msg << "\n";
    }

    void sleep(u64 milliseconds) OK {
        Sleep(static_cast<DWORD>(milliseconds));
    }


    LRESULT CALLBACK win32ProcessMessage(HWND window, u32 msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case WM_ERASEBKGND: {
                // Notify Windows to ignore this; clearing the background (screen) will be handled by the application.
                // This is to avoid flicker.
                return 1;
            }
            case WM_CLOSE: {
                // TODO notify app to exit
                return 0;
            }
            case WM_DESTROY: {
                PostQuitMessage(0); // exit code 0 => posts WM_QUIT
                return 0;
            }
            case WM_SIZE: {
                // window resized message

                // get updated size
                // RECT rect;
                // GetClientRect(window, &rect);
                // u32 width = rect.right - rect.left;
                // u32 height = rect.bottom - rect.top;

                // TODO window resize event
            } break;

                // TODO handle input
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP: {
                //const bool is_keydown = (msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN);
            } break;
            case WM_MOUSEMOVE: {
                // const i32 x_pos = GET_X_LPARAM(l_param);
                // const i32 y_pos = GET_Y_LPARAM(l_param);
            } break;
            case WM_MOUSEWHEEL: {
                // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
                // if (z_delta != 0) {
                //     // set to simply up or down
                //     z_delta = (z_delta < 0) ? -1 : 1;
                // }
            } break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
                //
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
                //
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP: {

            } break;
        }

        // default handle all other events
        return DefWindowProcA(window, msg, wParam, lParam);
    }
}
#endif

