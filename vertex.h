#ifndef ENGINE_VERTEX_H
#define ENGINE_VERTEX_H

#include "matrix.h"

namespace engine
{
    struct vertex
    {
        double x;
        double y;

        constexpr vertex()
            : x(0.0)
            , y(0.0)
        {}
        constexpr vertex(const double xarg, const double yarg)
            : x(xarg)
            , y(yarg)
        {}

        template<std::size_t columnCount>
        vertex operator *(const matrix<3, columnCount> &m)
        {
            matrix<1, 3> helper {
                { x, y, 1 }
            };

            auto r = helper * m;

            return {
                r(0, 0),
                r(0, 1)
            };
        }
    };

    using point = vertex;
    using vector2d = vertex;
}

#endif // ENGINE_VERTEX_H
