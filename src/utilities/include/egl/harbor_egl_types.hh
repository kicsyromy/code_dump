#ifndef HARBOR_UTILITIES_EGL_TYPES_HH
#define HARBOR_UTILITIES_EGL_TYPES_HH

#include <array>

namespace harbor::utilities
{
    namespace egl
    {
        struct rectangle
        {
            enum indices
            {
                X,
                Y,
                Width,
                Height
            };
            std::array<int, 4> data;
        };
    } // namespace egl
} // namespace harbor::utilities

#endif /* !HARBOR_UTILITIES_EGL_TYPES_HH */
