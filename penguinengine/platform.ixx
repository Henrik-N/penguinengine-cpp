module;
#include <cstdio>
#include "defines.h"

#include <vector>

#ifdef PENGUIN_PLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif

export module platform;

#ifdef PENGUIN_PLATFORM_WINDOWS
namespace platform {
	export struct InternalState {
		HINSTANCE instance;
		HWND window;
	};

	struct SystemClock {
		f64 frequency; // a multiplier for the clock cycles received from the OS
		LARGE_INTEGER startTime; // starting time of the app

		explicit 
			SystemClock() OK;
		f64 
			getAbsoluteTime() const OK;
	};

	struct PlatformState {
		InternalState internalState{};

		// This clock doesn't tick and doesn't really have a state. 
		// It queries the state from the OS and keeps track of the clock frequency and the start time of the application
		SystemClock systemClock{};
	};
}
#endif

namespace platform {
	static PlatformState platformState{};
}

export namespace platform {
	struct PlatformInitInfo {
		const char* application_name;
		u32 x;
		u32 y;
		u32 width;
		u32 height;
	};

	void
		init(const PlatformInitInfo& initInfo);
	void
		deinit() OK;
	void
		popMessages() OK;
	void
		writeConsoleMessage(stringv msgTag, stringv msg, u8 logLevel) OK;
	void
		writeConsoleError(stringv msgTag, stringv msg, u8 logLevel) OK;
	f64
		getAbsoluteTime() OK;
	void
		sleep(u64 milliseconds) OK;


#ifdef PENGUIN_RENDERER_VULKAN
	namespace vulkan {
		export const char*
			getRequiredExtensionName() OK;

		export InternalState
			readInternalState() OK;
	}
#endif

}
