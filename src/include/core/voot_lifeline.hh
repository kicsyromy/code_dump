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

inline Lifeline get_lifeline(const Weakline &object) noexcept
{
    return object.lock();
}

VT_END_NAMESPACE
