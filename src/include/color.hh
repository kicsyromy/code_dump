#ifndef COLOR_HH
#define COLOR_HH

#include <cmath>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace color
{
    using triplet = std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>;

    /* This array maps the color code to it's RGB value. It contains */
    /* 248 elements since the last 8 are all black                   */
    constexpr triplet colors[]{
        { 0, 0, 0 },       { 0, 0, 170 },     { 0, 170, 0 },     { 0, 170, 170 },
        { 170, 0, 170 },   { 170, 85, 0 },    { 170, 170, 170 }, { 85, 85, 85 },
        { 85, 85, 255 },   { 85, 255, 85 },   { 85, 255, 255 },  { 255, 85, 85 },
        { 255, 85, 255 },  { 255, 255, 85 },  { 255, 255, 255 }, { 0, 0, 0 },
        { 20, 20, 20 },    { 32, 32, 32 },    { 44, 44, 44 },    { 56, 56, 56 },
        { 69, 69, 69 },    { 81, 81, 81 },    { 97, 97, 97 },    { 97, 97, 97 },
        { 113, 113, 113 }, { 130, 130, 130 }, { 146, 146, 146 }, { 162, 162, 162 },
        { 182, 182, 182 }, { 203, 203, 203 }, { 227, 227, 227 }, { 255, 255, 255 },
        { 0, 0, 255 },     { 65, 0, 255 },    { 125, 0, 255 },   { 190, 0, 255 },
        { 255, 0, 255 },   { 255, 0, 190 },   { 255, 0, 125 },   { 255, 0, 65 },
        { 255, 0, 0 },     { 255, 65, 0 },    { 255, 125, 0 },   { 255, 190, 0 },
        { 255, 255, 0 },   { 190, 255, 0 },   { 125, 255, 0 },   { 65, 255, 0 },
        { 0, 255, 0 },     { 0, 255, 65 },    { 0, 255, 125 },   { 0, 255, 190 },
        { 0, 255, 255 },   { 0, 190, 255 },   { 0, 125, 255 },   { 0, 65, 255 },
        { 125, 125, 255 }, { 158, 125, 255 }, { 190, 125, 255 }, { 223, 125, 255 },
        { 255, 125, 255 }, { 255, 125, 223 }, { 255, 125, 190 }, { 255, 125, 158 },
        { 255, 125, 125 }, { 255, 158, 125 }, { 255, 190, 125 }, { 255, 223, 125 },
        { 255, 255, 125 }, { 223, 255, 125 }, { 190, 255, 125 }, { 158, 255, 125 },
        { 125, 255, 125 }, { 125, 255, 158 }, { 125, 255, 190 }, { 125, 255, 223 },
        { 125, 255, 255 }, { 125, 223, 255 }, { 125, 190, 255 }, { 125, 158, 255 },
        { 182, 182, 255 }, { 199, 182, 255 }, { 219, 182, 255 }, { 235, 182, 255 },
        { 255, 182, 255 }, { 255, 182, 235 }, { 255, 182, 219 }, { 255, 182, 199 },
        { 255, 182, 182 }, { 255, 199, 182 }, { 255, 219, 182 }, { 255, 235, 182 },
        { 255, 255, 182 }, { 235, 255, 182 }, { 219, 255, 182 }, { 199, 255, 182 },
        { 182, 255, 182 }, { 182, 255, 199 }, { 182, 255, 219 }, { 182, 255, 235 },
        { 182, 255, 255 }, { 182, 235, 255 }, { 182, 219, 255 }, { 182, 199, 255 },
        { 0, 0, 113 },     { 28, 0, 113 },    { 56, 0, 113 },    { 85, 0, 113 },
        { 113, 0, 113 },   { 113, 0, 85 },    { 113, 0, 56 },    { 113, 0, 28 },
        { 113, 0, 0 },     { 113, 28, 0 },    { 113, 56, 0 },    { 113, 85, 0 },
        { 113, 113, 0 },   { 85, 113, 0 },    { 56, 113, 0 },    { 28, 113, 0 },
        { 0, 113, 0 },     { 0, 113, 28 },    { 0, 113, 56 },    { 0, 113, 85 },
        { 0, 113, 113 },   { 0, 85, 113 },    { 0, 56, 113 },    { 0, 28, 113 },
        { 56, 56, 113 },   { 69, 56, 113 },   { 85, 56, 113 },   { 97, 56, 113 },
        { 113, 56, 113 },  { 113, 56, 97 },   { 113, 56, 85 },   { 113, 56, 69 },
        { 113, 56, 56 },   { 113, 69, 56 },   { 113, 85, 56 },   { 113, 97, 56 },
        { 113, 113, 56 },  { 97, 113, 56 },   { 85, 113, 56 },   { 69, 113, 56 },
        { 56, 113, 56 },   { 56, 113, 69 },   { 56, 113, 85 },   { 56, 113, 97 },
        { 56, 113, 113 },  { 56, 97, 113 },   { 56, 85, 113 },   { 56, 69, 113 },
        { 81, 81, 113 },   { 89, 81, 113 },   { 97, 81, 113 },   { 105, 81, 113 },
        { 113, 81, 113 },  { 113, 81, 105 },  { 113, 81, 97 },   { 113, 81, 89 },
        { 113, 81, 81 },   { 113, 89, 81 },   { 113, 97, 81 },   { 113, 105, 81 },
        { 113, 113, 81 },  { 105, 113, 81 },  { 97, 113, 81 },   { 89, 113, 81 },
        { 81, 113, 81 },   { 81, 113, 89 },   { 81, 113, 97 },   { 81, 113, 105 },
        { 81, 113, 113 },  { 81, 105, 113 },  { 81, 97, 113 },   { 81, 89, 113 },
        { 0, 0, 65 },      { 16, 0, 65 },     { 32, 0, 65 },     { 48, 0, 65 },
        { 65, 0, 65 },     { 65, 0, 48 },     { 65, 0, 32 },     { 65, 0, 16 },
        { 65, 0, 0 },      { 65, 16, 0 },     { 65, 32, 0 },     { 65, 48, 0 },
        { 65, 65, 0 },     { 48, 65, 0 },     { 32, 65, 0 },     { 16, 65, 0 },
        { 0, 65, 0 },      { 0, 65, 16 },     { 0, 65, 32 },     { 0, 65, 48 },
        { 0, 65, 65 },     { 0, 48, 65 },     { 0, 32, 65 },     { 0, 16, 65 },
        { 32, 32, 65 },    { 40, 32, 65 },    { 48, 32, 65 },    { 56, 32, 65 },
        { 65, 32, 65 },    { 65, 32, 56 },    { 65, 32, 48 },    { 65, 32, 40 },
        { 65, 32, 32 },    { 65, 40, 32 },    { 65, 48, 32 },    { 65, 56, 32 },
        { 65, 65, 32 },    { 56, 65, 32 },    { 48, 65, 32 },    { 40, 65, 32 },
        { 32, 65, 32 },    { 32, 65, 40 },    { 32, 65, 48 },    { 32, 65, 56 },
        { 32, 65, 65 },    { 32, 56, 65 },    { 32, 48, 65 },    { 32, 40, 65 },
        { 44, 44, 65 },    { 48, 44, 65 },    { 52, 44, 65 },    { 60, 44, 65 },
        { 65, 44, 65 },    { 65, 44, 60 },    { 65, 44, 52 },    { 65, 44, 48 },
        { 65, 44, 44 },    { 65, 48, 44 },    { 65, 52, 44 },    { 65, 60, 44 },
        { 65, 65, 44 },    { 60, 65, 44 },    { 52, 65, 44 },    { 48, 65, 44 },
        { 44, 65, 44 },    { 44, 65, 48 },    { 44, 65, 52 },    { 44, 65, 60 },
        { 44, 65, 65 },    { 44, 60, 65 },    { 44, 52, 65 },    { 44, 48, 65 },
    };

    template <typename T> static constexpr auto square(T t) { return (t * t); }

    template <std::uint8_t r, std::uint8_t g, std::uint8_t b>
    static constexpr bool compare_colors(const triplet &lhs, const triplet rhs)
    {
        return ((square(std::get<0>(lhs) - r) + square(std::get<1>(lhs) - g) +
                 square(std::get<2>(lhs) - b)) <
                (square(std::get<0>(rhs) - r) + square(std::get<1>(rhs) - g) +
                 square(std::get<2>(rhs) - b)));
    }

    template <std::uint8_t r, std::uint8_t g, std::uint8_t b> static constexpr auto from_rgb()
    {
        return std::min_element(std::begin(colors), std::end(colors), &compare_colors<r, g, b>) -
               std::begin(colors);
    }

} // namespace color

template <std::uint8_t rr, std::uint8_t gg, std::uint8_t bb> class Color
{
public:
    constexpr Color() : code_(color::from_rgb<rr, gg, bb>()), r_(rr), g_(gg), b_(bb) {}
    ~Color() = default;

public:
    constexpr auto code() const { return code_; }
    constexpr auto r() const { return r_; }
    constexpr auto g() const { return g_; }
    constexpr auto b() const { return b_; }

private:
    const std::uint8_t code_ = 0;
    const std::uint8_t r_ = 0;
    const std::uint8_t g_ = 0;
    const std::uint8_t b_ = 0;

public:
    Color(const Color &) = delete;
    Color(Color &&) = default;
    Color &operator=(const Color) = delete;
    Color &operator=(Color &&) = default;
};

struct color_map
{
    static constexpr std::uint32_t color_base_offset{ 4278190080 };

    std::uint8_t from_32bit(std::uint32_t rgba_value) noexcept
    {
        const auto color_offset = rgba_value - color_base_offset;

        const auto it = map_.find(color_offset);
        if (it == map_.cend())
        {
            std::uint32_t rgb_index;
            std::uint8_t dos_index;

            std::fseek(color_map_file_.get(),
                       sizeof(std::uint32_t) +
                           color_offset * (sizeof(std::uint32_t) + sizeof(std::uint8_t)),
                       SEEK_SET);

            std::fread(&rgb_index, sizeof(std::uint32_t), 1, color_map_file_.get());
            std::fread(&dos_index, sizeof(std::uint8_t), 1, color_map_file_.get());
            map_.emplace(rgb_index, dos_index);

            return dos_index;
        }
        else
        {
            return it->second;
        }
    }

private:
    std::unique_ptr<std::FILE, decltype(&std::fclose)> color_map_file_{
        std::fopen("cmap.bin", "rb"), &std::fclose
    };
    std::uint32_t color_count_{ std::fread(
        &color_count_, sizeof(std::uint32_t), 1, color_map_file_.get()) };

    struct identity_hash
    {
        constexpr std::size_t operator()(std::uint32_t v) const noexcept { return v; }
    };
    std::unordered_map<std::uint32_t, std::uint8_t, identity_hash> map_;
};

namespace color
{
    constexpr auto black() noexcept { return Color<0, 0, 0>{}; }
    constexpr auto white() noexcept { return Color<255, 255, 255>{}; }
    constexpr auto red() noexcept { return Color<255, 0, 0>{}; }
    constexpr auto green() noexcept { return Color<0, 255, 0>{}; }
    constexpr auto blue() noexcept { return Color<0, 0, 255>{}; }

    constexpr double color_distance_complex(const triplet &c1, const triplet &c2)
    {
        const auto &[c1r, c1g, c1b] = c1;
        const auto &[c2r, c2g, c2b] = c2;

        const auto rmean = (static_cast<long>(c1r) + static_cast<long>(c2r)) / 2;
        const auto r = static_cast<long>(c1r) - static_cast<long>(c2r);
        const auto g = static_cast<long>(c1g) - static_cast<long>(c2g);
        const auto b = static_cast<long>(c1b) - static_cast<long>(c2b);
        return std::sqrt((((512 + rmean) * r * r) >> 8) + 4 * g * g +
                         (((767 - rmean) * b * b) >> 8));
    }
} // namespace color

#endif /* !COLOR_HH */
