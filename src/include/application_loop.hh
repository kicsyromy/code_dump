#ifndef RML_APPLICATION_LOOP_HH
#define RML_APPLICATION_LOOP_HH

#include <cstdio>

namespace rml
{
    namespace events
    {
        template<typename event_loop_t>
        class application_loop
        {
        public:
            void post(typename event_loop_t::event_type &&event)
            {
                event_loop_.post(std::move(event));
            }

            template<typename ...Args>
            void post(Args &&...args)
            {
                event_loop_.post(std::forward<Args>(args)...);
            }

            void run()
            {
                event_loop_.run([](auto &&event){
                    event();
                });
            }

        private:
            event_loop_t event_loop_;
        };

        template<typename event_loop_t>
        application_loop<event_loop_t> *main_loop{ nullptr };
    }
}

#endif /* RML_APPLICATION_LOOP_HH */