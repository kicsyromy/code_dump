#ifndef ENGINE_VERTEX_H
#define ENGINE_VERTEX_H

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
    };

    using point = vertex;
    using vector2d = vertex;
}

#endif // ENGINE_VERTEX_H
