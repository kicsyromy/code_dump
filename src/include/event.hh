#ifndef CONCORD_EVENT_HH
#define CONCORD_EVENT_HH

namespace rml
{
    namespace events
    {
        class event
        {
        public:
            using callback_t = void (*)(void *data);

        public:
            event() : data_{ nullptr }, callback_{ [](void *) {} } {}

            event(callback_t &&callback, void *data) : data_{ data }, callback_{ callback } {}

        public:
            inline auto data() const noexcept -> void * { return data_; }
            inline void operator()() { callback_(data_); }

        private:
            void *data_;
            callback_t callback_;
        };
    } // namespace events
} // namespace rml

#endif /* !RML_EVENT_HH */
