#include <type_traits>

#include "signal.hh"

namespace rml
{
    namespace events
    {
        template <typename event_loop_t, typename value_t>
        class property
        {
        public:
            template<typename... Args>
            property(event_loop_t &event_loop, Args &&...args)
            : changed{ event_loop }
            , value_{std::forward<Args>(args)...}
            {}

            explicit property(event_loop_t &event_loop, const std::decay_t<value_t> &value)
            : value_{ value }
            {}

            explicit property(event_loop_t &event_loop, std::decay_t<value_t> &&value)
            : value_{ std::move(value) }
            {}

        public:
            signal<event_loop_t, value_t> changed;

        public:
            auto operator =(const std::decay_t<value_t> &value) -> property &
            {
                value_ = value;
                changed.emit(value_);

                return *this;
            }

            auto operator =(std::decay_t<value_t> &&value) -> property &
            {
                value_ = std::move(value);
                changed.emit(value_);

                return *this;
            }

            constexpr operator std::decay_t<value_t>()
            {
                return value_;
            }

            constexpr operator const std::decay_t<value_t> &() const
            {
                return value_;
            }

        private:
            std::decay_t<value_t> value_;
        };
    }
}