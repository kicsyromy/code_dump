#pragma once

#include "voot_global.hh"

#include <memory>

#define VT_ADD_LIFELINE                                                                 \
private:                                                                                \
    voot::Lifeline lifeline_{ &voot::lifeline_data, [](void *) constexpr noexcept {} }; \
                                                                                        \
public:                                                                                 \
    inline voot::Weakline get_lifeline()                                                \
    {                                                                                   \
        return lifeline_;                                                               \
    }

VT_BEGIN_NAMESPACE

extern char lifeline_data;

using Lifeline = std::shared_ptr<void>;
using Weakline = std::weak_ptr<void>;

namespace lifeline
{
    template<typename T> class HasLifeline
    {
        typedef char one;
        struct two
        {
            char x[2];
        };

        template<typename C> static one test(decltype(&C::lifeline_) = {});
        template<typename C> static two test(...);

    public:
        enum
        {
            value = sizeof(test<T>(0)) == sizeof(char)
        };
    };
} // namespace lifeline

inline Lifeline get_lifeline(const Weakline &object) noexcept
{
    return object.lock();
}

template<typename T> constexpr bool has_lifeline() noexcept
{
    return lifeline::HasLifeline<T>::value;
}

VT_END_NAMESPACE
