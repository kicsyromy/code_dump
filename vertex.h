#ifndef ENGINE_VERTEX_H
#define ENGINE_VERTEX_H

#include "matrix.h"

namespace engine
{
    class vertex
    {
    public:
        constexpr vertex()
            : x(0.0)
            , y(0.0)
        {}
        constexpr vertex(const double xarg, const double yarg)
            : x(xarg)
            , y(yarg)
        {}

    public:
        vertex transform(const matrix<3, 3> &m);

    public:
        double x;
        double y;
    };

    using point = vertex;
    using vector2d = vertex;
}

#endif // ENGINE_VERTEX_H
