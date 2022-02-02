module;

#include "defines.h"
#include <functional>

export module events;

import logger;

namespace events {
    export {
        struct EventResponse {
            bool consume; // stop event from being propogated further
        };


        struct WindowEvent {
            u32 newWidth;
        };


        struct Window {
            u32 width;
            u32 height;

            using EventCallbackFn = std::function<void(WindowEvent&)>;

            EventCallbackFn _callbackFn;

            void setOnWindowSizeChangeCallback(const EventCallbackFn& callback) {
                _callbackFn = callback;
            }

            void dispatchOnWindowSizeChangeEvent(WindowEvent& ev) {
                width = ev.newWidth;

                _callbackFn(ev);
            }
        };

        struct SomeStruct {
            void onWindowCallback(WindowEvent& ev) {
                INFO("Window callback! new width: {}", ev.newWidth);
            }

            void testEvents() {
                Window wind{ .width = 800, .height = 600 };

                wind.setOnWindowSizeChangeCallback(std::bind(&SomeStruct::onWindowCallback, this, std::placeholders::_1));

                WindowEvent winEvent{ .newWidth = 600 };

                wind.dispatchOnWindowSizeChangeEvent(winEvent);
            }
        };


        void
            init();
        void
            deinit();
    }
}

module :private;

 namespace events {
    void init() {
        //
    }

    void deinit() {
        //
    }
}

