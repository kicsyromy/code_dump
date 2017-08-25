#ifndef VGA_H
#define VGA_H

#include <cstdint>

#include "color.h"

class VGA
{
public:
    static constexpr const int SCREEN_WIDTH  { 320 };
    static constexpr const int SCREEN_HEIGHT { 200 };

public:
    VGA();
    ~VGA();

public:
    template <std::uint8_t r, std::uint8_t g, std::uint8_t b>
    void put_pixel(int x, int y, const Color<r, g, b> &c)
    {
        static constexpr const auto multiply_by_320 = [](int value) { return (value << 8) + (value << 6); };
        vga_memory_buffer_[multiply_by_320(y) + x] = c.code();
    }

private:
    std::uint8_t *vga_memory_buffer_;

private:
    VGA(const VGA &) = delete;
    VGA(VGA &&) = delete;
    VGA &operator =(const VGA &) = delete;
    VGA &operator =(VGA &&) = delete;
};

#endif // VGA_H
