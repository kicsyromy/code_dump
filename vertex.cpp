#include "vertex.h"

using namespace engine;

vertex vertex::transform(const matrix<3, 3> &m)
{
    matrix<3, 1> helper {
        { x },
        { y },
        { 1 }
    };

    auto r = std::move(m * helper);

    const auto zCoord = r(2,0);
    if (zCoord != 1)
    {
        r /= zCoord;
    }

    return {
        r(0, 0),
        r(1, 0)
    };
}
