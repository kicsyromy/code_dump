#ifndef CONSTANTS_HH
#define CONSTANTS_HH

#include <cmath>

constexpr auto PI = static_cast<float>(3.14159f);
constexpr auto SCREEN_HEIGHT{ 240ull };
constexpr auto SCREEN_WIDTH{ 256ull };
constexpr auto ASPECT_RATIO{ (1.f * SCREEN_HEIGHT) / SCREEN_WIDTH };

#endif /* !CONSTANTS_HH */
