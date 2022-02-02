module;

#include "defines.h"
#include <functional>

export module events;

export {
    //struct Event {
    //    i32 someData;
    //};

    using ShouldConsumeEvent = bool;

    template <typename Func, typename TEvent>
    concept CEventFunc = requires(const Func& func, TEvent ev) {
        { func(static_cast<TEvent&>(ev)) } -> std::convertible_to<ShouldConsumeEvent>;
    };

    template <typename Event>
    struct EventDispatcher {
    public:
        explicit EventDispatcher(Event& ev) : _event(ev) {}

        ShouldConsumeEvent dispatch(CEventFunc<Event> auto func) {
            return func(_event);
        };

    private:
        Event& _event;
    };


    struct SomeEvent {
        u32 someData;
    };

    ShouldConsumeEvent someDispatchFunc(const SomeEvent& ev) {
        return true;
    }

    namespace events {
        void test() {
            SomeEvent ev{ .someData = 2 };
            EventDispatcher<SomeEvent> dispatcher(ev);
            const ShouldConsumeEvent yeet = dispatcher.dispatch(&someDispatchFunc);

        }
    } 
}

