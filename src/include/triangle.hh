#ifndef TRIANGLE_HH
#define TRIANGLE_HH

#include <array>

#include <SFML/Graphics.hpp>

#include "vector.hh"

struct triangle
{
    std::array<vector3f, 3> vertices;
    sf::Color color;
};

#endif /* !TRIANGLE_HH */
