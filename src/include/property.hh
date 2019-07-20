#ifndef RML_EVENTS_PROPERTY_HH
#define RML_EVENTS_PROPERTY_HH

#include <type_traits>

#include "signal.hh"

namespace rml
{
    namespace events
    {
        template <typename event_loop_t, typename value_t> class property
        {
        private:
            using decayed_value_t = std::decay_t<value_t>;

        public:
            template <typename... Args>
            property(event_loop_t &event_loop, Args &&... args)
              : changed{ event_loop }, value_{ std::forward<Args>(args)... }
            {
            }

            property(event_loop_t &event_loop, const decayed_value_t &value)
              : changed{ event_loop }, value_{ value }
            {
            }

            property(event_loop_t &event_loop, decayed_value_t &&value)
              : changed{ event_loop }, value_{ std::move(value) }
            {
            }

        public:
            signal<event_loop_t, value_t> changed;

        public:
            auto operator=(const decayed_value_t &value) -> property &
            {
                value_ = value;
                changed.emit(decayed_value_t{ value });

                return *this;
            }

            auto operator=(decayed_value_t &&value) -> property &
            {
                value_ = value;
                changed.emit(std::move(value));

                return *this;
            }

            constexpr operator decayed_value_t() { return value_; }

            constexpr operator const decayed_value_t &() const { return value_; }

        private:
            decayed_value_t value_;
        };
    } // namespace events
} // namespace rml

#endif /* !RML_EVENTS_PROPERTY_HH */
