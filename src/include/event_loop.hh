#ifndef RML_EVENT_LOOP_HH
#define RML_EVENT_LOOP_HH

#include <cstdio>

#include "concurrent_queue.hh"

namespace rml
{
    namespace events
    {
        template <typename event_t, typename queue_t> class event_loop
        {
        public:
            using event_type = event_t;

        public:
            void post(event_t &&event) { event_queue_.push(std::move(event)); }

            template <typename... Args> void post(Args &&... args)
            {
                event_queue_.emplace(std::forward<Args>(args)...);
            }

            template <typename callable_t> void run(callable_t &&event_processor)
            {
                for (;;)
                {
                    auto event = event_queue_.pop();
                    event_processor(std::move(event));
                }
            }

        private:
            queue_t event_queue_;
        };
    } // namespace events
} // namespace rml

#endif /* RML_EVENT_LOOP_HH */
