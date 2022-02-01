module;

#include "renderTypes.h"

//#include "rendererBackend.h"

export module renderer;

import "rendererBackend.h";

namespace renderer {

    static RendererBackend backend;

    export {
        void 
            init();
        void 
            deinit();
        void
            onWindowResize(u32 width, u32 height);
        void
            drawFrame(const RenderPacket& renderPacket);
        void
            createGeometry(const GeometryPacket& geometryPacket);
    }
}

export {

}

module :private;

namespace renderer {

    void init() {
        backend.init("penguin app");
    }

    void deinit() {
        backend.deinit();
    }

    void onWindowResize(u32 width, u32 height) {
        backend.onWindowResize(width, height);
    }

    void drawFrame(const RenderPacket& renderPacket) {
    }

    void createGeometry(const GeometryPacket& geometryPacket) {
    }
}
