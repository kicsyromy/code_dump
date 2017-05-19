#include "matrix.h"
#include "vertex.h"

int f()
{
    engine::matrix<3, 3> m {
        { 1, 0, 4 },
        { 0, 1, 2 },
        { 0, 0, 1 }
    };
    engine::vertex v { 2, 4 };

    auto r = v.transform(m);
}
