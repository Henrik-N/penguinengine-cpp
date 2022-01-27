module;
#include <cstdio>
#include "macros.h"
#include <iostream>

#ifdef PENGUIN_PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif
#include <format>

export module platform;

import penguin_t;

#ifdef PENGUIN_PLATFORM_WINDOWS
namespace platform {
	struct InternalState {
		HINSTANCE instance;
		HWND window;
	};

	struct SystemClock {
		f64 clock_frequency; // a multiplier for the clock cycles received from the OS
		LARGE_INTEGER start_time; // starting time of the app

		explicit 
			SystemClock();
		f64 
			getAbsoluteTime() const;
	};

	struct PlatformState {
		InternalState internal_state{};

		// This clock doesn't tick and doesn't really have a state. 
		// It queries the state from the OS and keeps track of the clock frequency and the start time of the application
		SystemClock system_clock{};
	};

}
#endif

namespace platform {
	static PlatformState platform_state{};
}

export namespace platform {
	struct PlatformInitInfo {
		const char* application_name;
		u32 x;
		u32 y;
		u32 width;
		u32 height;
	};

	bool
		init(const PlatformInitInfo& init_info);
	bool
		deinit();
	bool
		popMessages();
	void
		writeConsoleMessage(stringv msg_tag, stringv msg, u8 log_level);
	void
		writeConsoleError(stringv msg_tag, stringv msg, u8 log_level);
	f64
		getAbsoluteTime();
	void
		sleep(u64 milliseconds);
}


#ifdef PENGUIN_PLATFORM_WINDOWS
namespace platform {
	SystemClock::SystemClock() {
		// get processor clock frequency
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		// LARGE_INTEGER is a union and the QuadPart is used to store a 64-bit integer on compilers with built-in support for 64-bit. 
		// Converting it into a floating point value
		this->clock_frequency = 1.0 / static_cast<f64>(frequency.QuadPart);

		// set the current time for setting 
		QueryPerformanceCounter(&this->start_time);
	}

	f64 SystemClock::getAbsoluteTime() const {
		// get number of cycles since the app started 
		LARGE_INTEGER current_time;
		QueryPerformanceCounter(&current_time);
		// 
		return static_cast<f64>(current_time.QuadPart) * this->clock_frequency;
	}

	f64 getAbsoluteTime() {
		return platform_state.system_clock.getAbsoluteTime();
	}


	LRESULT CALLBACK win32ProcessMessage(HWND window, u32 msg, WPARAM w_param, LPARAM l_param);

	struct WindowRect {
		u32 x_pos;
		u32 y_pos;
		u32 width;
		u32 height;

		void pushBordersOut(RECT adjustment_rect) {
			// The adjustment rectangle is a is a measurement of how different the client rect
			// is compared to the outer window rect.
			// 
			// The adjustment border rect will be negative, suggesting
			//   that the client rect is smaller than the window rect.

			// We expand the outer window rect, moving the borders out.
			//  This way, the contents of the entire client rect will be displayed, 
			//  and then the borders, will be displayed outside of it (rather than 
			//  the borders being inside the client rect).

			x_pos += adjustment_rect.left;
			y_pos += adjustment_rect.top;
			width += adjustment_rect.right - adjustment_rect.left;
			height += adjustment_rect.bottom - adjustment_rect.top;
		}
	};

	constexpr LPCSTR WindowClassName = "PenguinWindowClass";

	bool tryCreateHWindow(HWND& h_window, const PlatformInitInfo& init_info, HINSTANCE h_instance) {
		const u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION
			| WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
		const u32 window_ex_style = WS_EX_APPWINDOW;

		// client window ("inner" window, without borders etc)
		WindowRect window{
			.x_pos = init_info.x,
			.y_pos = init_info.y,
			.width = init_info.width,
			.height = init_info.height,
		};

		// get border size
		RECT adjustment_rect = { 0, 0, 0, 0 };
		AdjustWindowRectEx(&adjustment_rect, window_style, 0, window_ex_style);

		window.pushBordersOut(adjustment_rect);

		h_window = CreateWindowExA(
			window_ex_style, WindowClassName, init_info.application_name,
			window_style, window.x_pos, window.y_pos, window.width, window.height,
			0, 0, h_instance, 0
		);

		if (!h_window) {
			const char* err_msg = "Platform: failed to create window!";
			MessageBoxA(nullptr, err_msg, "Error!", MB_ICONEXCLAMATION | MB_OK);

			printf("%s", err_msg);

			return false;
		}

		return true;
	}


	bool init(const PlatformInitInfo& init_info) {

		HINSTANCE h_instance = GetModuleHandleA(0);

		HICON icon = LoadIcon(h_instance, IDI_APPLICATION);
		WNDCLASSA wc;
		memset(&wc, 0, sizeof(wc));
		wc.style = CS_DBLCLKS; // capture double clicks
		// Pointer to the window procedure.
		//  Events that need to be handled
		wc.lpfnWndProc = win32ProcessMessage;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = h_instance;
		wc.hIcon = icon;
		// we'll manage the cursor by ourselves
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszClassName = WindowClassName;

		if (!RegisterClassA(&wc)) {
			const char* err_msg = "Windows: failed to register window";
			MessageBoxA(0, err_msg, "Error", MB_ICONEXCLAMATION | MB_OK);
			printf("%s", err_msg);
			return false;
		}


		const u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION
			| WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
		const u32 window_ex_style = WS_EX_APPWINDOW;

		// client window ("inner" window, without borders etc)
		WindowRect client_window{
			.x_pos = init_info.x,
			.y_pos = init_info.y,
			.width = init_info.width,
			.height = init_info.height,
		};

		HWND h_window;
		if (!tryCreateHWindow(h_window, init_info, h_instance)) {
			printf("failed to create window");
			return false;
		}

		// window creation successful
		const InternalState internal_state = {
			.instance = h_instance,
			.window = h_window,
		};

		// display window
		bool should_activate = true; // window should accept input
		i32 show_window_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
		ShowWindow(internal_state.window, show_window_flags);

		platform_state = platform::PlatformState{
			.internal_state = internal_state,
			.system_clock = SystemClock(),
		};

		return true;
	}

	bool deinit() {
		const auto window = platform_state.internal_state.window;

		if (window != nullptr) {
			DestroyWindow(window);
		}
		else {
			// logError("platform: failed to deinit window"); // TODO 
		}

		return true;
	}

	bool popMessages() {
		// while there is a message in the message stack, pop it
	   //  from the stack and put it in the MSG struct
		MSG msg;
		while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg); // dispatch to win32ProcessMessage
		}

		return true;
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

	consteval u8 colorCode(Color foreground, Color background) {
		const u8 foreground_u8 = static_cast<u8>(foreground);
		const u8 background_u8 = static_cast<u8>(background);
		// The first half of the byte is the background, the second is the foreground.
		// Left shift the background value half a byte and combine it with the foreground.
		return (background_u8 << 4) | foreground_u8;
	}

	constexpr u8 colorFromLogLevel(u8 log_level) {
		switch (log_level) {
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

	void writeConsoleMessage(const stringv msg_tag, const stringv msg, u8 log_level) {
		const u8 color = colorFromLogLevel(log_level);
		const HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console_handle, color);
		const std::string str(msg);
		std::cout << str << "\n";
	}

	void writeConsoleError(const stringv msg_tag, const stringv msg, u8 log_level) {
		const u8 color = colorFromLogLevel(log_level);
		const HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
		SetConsoleTextAttribute(console_handle, color);
		std::cerr << msg_tag << msg;
	}

	void sleep(u64 milliseconds) {
		Sleep(static_cast<DWORD>(milliseconds));
	}


	LRESULT CALLBACK win32ProcessMessage(HWND window, u32 msg, WPARAM w_param, LPARAM l_param) {
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
		return DefWindowProcA(window, msg, w_param, l_param);
	}
}
#endif
