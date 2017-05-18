#include "matrix.h"
#include "vertex.h"

int f()
{
    engine::matrix<3, 3> m {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 }
    };
    engine::vertex v { 2, 4 };

    auto r = v * m;
    r * m;
}
