#include "matrix.h"

int f()
{
    engine::matrix<2, 1> m1;
    engine::matrix<1, 2> m2;

//    auto m3 = m1 + m2;

    auto m4 = m1 * m2;
}
