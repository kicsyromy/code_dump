#include "core/voot_binding.hh"

namespace
{
    voot::binding::BindingList all_bindings;
}

VOOT_BEGIN_NAMESPACE

binding::BindingList &binding::bindings() noexcept
{
    return all_bindings;
}

VOOT_END_NAMESPACE
