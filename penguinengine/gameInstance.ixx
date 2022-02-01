module;
#include "defines.h"
#include <cstdio>


export module gameInstance;


export {
    struct PENGUIN_API GameConfig {
        // displayed in the window title
        const char* applicationName = "penguin engine";
        u32 windowStartX = 100;
        u32 windowStartY = 800;
        u32 windowStartWidth = 800;
        u32 windowStartHeight = 600;
    };

    ///  Game instance to be imported by the client (from the engine dll/so).
    struct PENGUIN_API GameInstance {
        // has a default, but can optionally be changed by the child
        virtual GameConfig
            config() = 0;
        virtual void
            init() = 0;
        virtual void
            update(f32 dt) = 0;
        virtual void
            render(f32 dt) = 0;
        virtual void
            onWindowResize(u32 width, u32 height) = 0;
    };
}
