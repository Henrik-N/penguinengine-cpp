#pragma once

#include "defines.h"


struct RendererBackend {
    void
        init(stringv appName);
    void
        deinit();
    void
        onWindowResize(u32 width, u32 height);
};

