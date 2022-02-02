module;
#include <cstdio>
#include "defines.h"

export module application;

import platform;
import gameInstance;
import logger;
import renderer;

import events;

namespace application {

    struct Application {
        // platform state: in it's own module
        u32 windowWidth;
        u32 windowHeight;
        f64 lastFrameTime;
        bool isRunning;
        GameInstance* pGame;

        void
            run();
    };

    export Application init(GameInstance* gameInst);
}



module :private;

namespace application {
    Application init(GameInstance* gameInst) {
        const auto config = gameInst->config();

        const platform::PlatformInitInfo platformInitInfo{
            .application_name = config.applicationName,
            .x = config.windowStartX,
            .y = config.windowStartY,
            .width = config.windowStartWidth,
            .height = config.windowStartHeight,
        };

        events::test();

        platform::init(platformInitInfo);
        INFO("platform layer initialized");

        gameInst->init();
        INFO("game instance initialized");

        renderer::init();
        INFO("renderer initialized");


        return Application {
            .windowWidth = config.windowStartWidth,
            .windowHeight = config.windowStartHeight,
            .lastFrameTime = 0.f,
            .isRunning = true,
            .pGame = gameInst,
        };
    }

    void Application::run() {
        INFO("Starting app run loop...");

        const f32 delta = 0; // TODO 

        while (isRunning) {
            platform::popMessages();

            pGame->update(static_cast<f32>(delta));

            pGame->render(static_cast<f32>(delta));
        }


        renderer::deinit();

        platform::deinit();
    }

}

