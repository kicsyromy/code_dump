#ifndef HARBOR_UTILITIES_UTILITIES_HH
#define HARBOR_UTILITIES_UTILITIES_HH

#include <array>
#include <memory>
#include <type_traits>

namespace harbor::utilities
{
    template <typename... Types> constexpr auto make_array(Types &&... elements) noexcept
    {
        return std::array<std::common_type_t<Types...>, sizeof...(Types)>{ { std::forward<Types>(
            elements)... } };
    }

    template <typename Target, typename Source> constexpr Target force_cast(Source source) noexcept
    {
        static_assert(std::is_pointer_v<Target> && std::is_pointer_v<Source>,
                      "The source and target types must be pointers");
        return static_cast<Target>(static_cast<void *>(source));
    }

    template <typename T> using shared_ptr_t = std::__shared_ptr<T, __gnu_cxx::_S_single>;
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_UTILITIES_HH */
