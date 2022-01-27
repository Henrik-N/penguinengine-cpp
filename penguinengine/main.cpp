#include <cstdio>

#include "macros.h"

import application;

import game_instance;
import logger;

import penguin_t;

// TODO extern initalizer for this
using namespace application;

static f32 accumulated_time = 0;

struct Game final : public game_instance::GameInstance {
	constexpr game_instance::GameConfig config() override {
		return game_instance::GameConfig{
			.application_name = "penguin engine",
			.window_start_x = 100,
			.window_start_y = 100,
			.window_start_width = 800,
			.window_start_height = 600,
		};
	}

	virtual bool init() override {
		ltrace("game init");
		return true;
	};

	virtual bool update(f32 dt) override {
		ltrace("game update {}", dt);
		return true;
	};

	virtual bool render(f32 dt) override {
		return true;
	}

	virtual bool onWindowResize(u32 width, u32 height) override {
		linfo("window resize event");
		return true;
	}
};


int main() {
	Game* game = new Game();

	if (!application::init(game)) {
		printf("failed to init app!");
		return -1;
	}

	application::run();

	delete game;

	return 0;
}


