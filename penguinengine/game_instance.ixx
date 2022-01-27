module;
#include "macros.h"
#include <cstdio>

export module game_instance;

import penguin_t;

export namespace game_instance {

	struct PENGUIN_API GameConfig {
		// displayed in the window title
		const char* application_name = "penguin engine";
		u32 window_start_x = 100;
		u32 window_start_y = 800;
		u32 window_start_width = 800;
		u32 window_start_height = 600;
	};

	struct PENGUIN_API GameInstance {
		// has a default, but can optionally be changed by the child
		constexpr virtual GameConfig
			config() = 0;
		virtual bool
			init() = 0;
		virtual bool
			update(f32 dt) = 0;
		virtual bool
			render(f32 dt) = 0;
		virtual bool
			onWindowResize(u32 width, u32 height) = 0;
	};
}

