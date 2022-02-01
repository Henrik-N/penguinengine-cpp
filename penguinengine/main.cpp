#include "defines.h"
#include <iostream>


import application;

import gameInstance;

struct Game final : public GameInstance {
	GameConfig config() override {
		const GameConfig config{
			.applicationName = "penguin engine",
			.windowStartX = 100,
			.windowStartY = 100,
			.windowStartWidth = 800,
			.windowStartHeight = 600,
		};
		return config;
	}

	virtual void init() override {

	};

	virtual void update(f32 dt) override {

	};

	virtual void render(f32 dt) override {
		
	}

	virtual void onWindowResize(u32 width, u32 height) override {
		//INFO("window resize event");
	}
};


// TODO extern GameInstance* createGame();



int main() {
	Game* game = new Game();

	try {
		auto app = application::init(game);
		app.run();


		delete game;
	}
	catch (const std::runtime_error& err) {
		std::cerr << err.what();
	}

	return 0;
}


