#include "matrix.h"
#include "vertex.h"

int f()
{
    engine::matrix<3, 3> m {
        {  1, 2, 3 },
        { -1, 3, 1 },
        {  1, 5, 2 }
    };

    auto r = engine::math::determinant(m);
    r = r + 2;
}
