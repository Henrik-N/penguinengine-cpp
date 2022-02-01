#pragma once
#include "defines.h"


namespace vulkan {

	// called from rendererBackend.cpp
	void
		init(stringv appName);
	void
		deinit();
	void
		onWindowResize(u32 width, u32 height);
}

