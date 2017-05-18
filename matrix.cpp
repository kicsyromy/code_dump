#include "matrix.h"

int f()
{
    engine::matrix<3, 3> m1;
    engine::matrix<3, 1> m2;

    auto m3 = m1 * m2;
//    auto m4 = m1 + m2;
}
