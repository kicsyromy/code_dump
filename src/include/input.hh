#ifndef RACER_INPUT_HH
#define RACER_INPUT_HH

#include <utility>

#include <allegro.h>

namespace input
{
    struct scan_code_t
    {
        int value;
        constexpr operator int() const noexcept { return value; }
    };

    struct modifier_t
    {
        int value;
        constexpr operator int() const noexcept { return value; }
    };

    inline std::pair<scan_code_t, modifier_t> read_key() noexcept
    {
        const auto key_value = readkey();
        if ((key_value & 0xff) == '\0')
        {
            return { scan_code_t{ key_value >> 8 }, modifier_t{ KEY_ALT } };
        }
        else if ((key_value & 0xff) < 32)
        {
            return { scan_code_t{ key_value >> 8 }, modifier_t{ KEY_LCONTROL } };
        }
        else if ((key_value & 0xff) > 64 && (key_value & 0xff) < 91)
        {
            return { scan_code_t{ key_value >> 8 }, modifier_t{ KEY_LSHIFT } };
        }
        else
        {
            return { scan_code_t{ key_value >> 8 }, modifier_t{ KEY_MAX } };
        }
    }
} // namespace input

#endif RACER_INPUT_HH
