#ifndef RML_RML_HH
#define RML_RML_HH

#include "concurrent_queue.hh"
#include "event.hh"
#include "event_loop.hh"
#include "property.hh"
#include "signal.hh"

namespace rml
{
    using event_loop_t =
        events::event_loop<events::event, utilities::concurrent_queue<events::event>>;

    class application_loop
    {
    private:
        inline static application_loop *instance_;

    public:
        inline application_loop() : event_loop_{} { instance_ = this; }

    public:
        void post(typename event_loop_t::event_type &&event) { event_loop_.post(std::move(event)); }

        template <typename... Args> void post(Args &&... args)
        {
            event_loop_.post(std::forward<Args>(args)...);
        }

        void run()
        {
            event_loop_.run([](auto &&event) { event(); });
        }

    public:
        static inline auto instance() noexcept -> application_loop & { return *instance_; }

    private:
        event_loop_t event_loop_;
    };

    template <typename... Args> class signal_t : public events::signal<application_loop, Args...>
    {
        using parent_t = events::signal<application_loop, Args...>;

    public:
        inline signal_t() : parent_t(application_loop::instance()) {}
    };

    template <typename value_t>
    class property_t : public events::property<application_loop, value_t>
    {
        using parent_t = events::property<application_loop, value_t>;

    public:
        inline property_t() : parent_t(application_loop::instance()) {}

        template <typename... Args>
        inline property_t(Args &&... args)
          : parent_t(application_loop::instance(), std::forward<Args>(args)...)
        {
        }

        inline property_t(const std::decay_t<value_t> &value)
          : parent_t(application_loop::instance(), value)
        {
        }

        inline property_t(std::decay_t<value_t> &&value)
          : parent_t(application_loop::instance(), std::move(value))
        {
        }

        using parent_t::operator=;
    };

} // namespace rml

#endif /* !RML_RML_HH */
