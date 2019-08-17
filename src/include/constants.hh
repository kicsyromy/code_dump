#ifndef CONSTANTS_HH
#define CONSTANTS_HH

#include <cmath>

constexpr auto PI = static_cast<float>(M_PI);
constexpr auto SCREEN_HEIGHT{ 240ull * 4 };
constexpr auto SCREEN_WIDTH{ 256ull * 4 };
constexpr auto ASPECT_RATIO{ (1.f * SCREEN_HEIGHT) / SCREEN_WIDTH };
constexpr auto Z_NEAR{ 0.1f };
constexpr auto Z_FAR{ 1000.f };
constexpr auto FOV{ 90.f };

#endif /* !CONSTANTS_HH */
