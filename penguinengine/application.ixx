module;
#include <cstdio>

export module application;

import platform;
import game_instance;
import penguin_t;

import logger;

using GameInst = game_instance::GameInstance;

namespace application {
	struct ApplicationState {
		// platform state: in it's own module
		u32 window_width;
		u32 window_height;
		f64 last_frame_time;
		bool is_running;
	};

	static ApplicationState app;
	static GameInst* game;
}

export namespace application {

	bool
		init(GameInst* game_inst);
	bool
		run();
}

namespace application {
	bool init(GameInst* game_inst) {
		linfo("Initializing application....");

		const auto config = game_inst->config();

		const platform::PlatformInitInfo platform_init_info{
			.application_name = config.application_name,
			.x = config.window_start_x,
			.y = config.window_start_y,
			.width = config.window_start_width,
			.height = config.window_start_height,
		};

		linfo("Initializing platform layer...");
		if (!platform::init(platform_init_info)) {
			return false;
		}

		linfo("Initializing game instance");
		if (!game_inst->init()) {
			return false;
		}

		app = ApplicationState{
			.window_width = config.window_start_width,
			.window_height = config.window_start_height,
			.last_frame_time = 0.f,
			.is_running = true,
		};
		game = game_inst;

		linfo("Application layer & game instance initialized");
		return true;
	}

	bool run() {
		const f32 delta = 0; // TODO 

		while (app.is_running) {

			ltrace("platform poppping messages");
			if (!platform::popMessages())
				break;

			if (!game->update(static_cast<f32>(delta)))
				break;

			if (!game->render(static_cast<f32>(delta)))
				break;
		}

		platform::deinit();

		return true;
	}
}

