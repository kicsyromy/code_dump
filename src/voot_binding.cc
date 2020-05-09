#include "core/voot_binding.hh"

namespace
{
    voot::binding::BindingList all_bindings;
}

VT_BEGIN_NAMESPACE

binding::BindingList &binding::bindings() noexcept
{
    return all_bindings;
}

VT_END_NAMESPACE
