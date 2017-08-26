#ifndef SOUND_H
#define SOUND_H

#include <cstdint>

struct SoundBlaster
{
    static constexpr std::uint16_t ADDRESS_PORT { 0x388 }; /* R/W */
    static constexpr std::uint16_t DATA_PORT    { 0x389 }; /* W/O */
};

#endif // SOUND_H
