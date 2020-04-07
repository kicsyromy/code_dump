#pragma once

#include "device.hh"

#include <array>
#include <cstdint>
#include <functional>
#include <tuple>

template<typename... Devices> class DataBus
{
private:
    template<size_t Index = 0,
        typename Tuple,
        size_t Size = std::tuple_size_v<std::remove_reference_t<Tuple>>, // tuple size
        typename Callable,
        typename... Args>
    void for_each(Tuple &&tuple, Callable &&callable, Args &&... args)
    {
        if constexpr (Index < Size)
        {
            if (!std::invoke(callable, args..., std::get<Index>(tuple))) return;

            if constexpr (Index + 1 < Size)
                for_each<Index + 1>(std::forward<Tuple>(tuple),
                    std::forward<Callable>(callable),
                    std::forward<Args>(args)...);
        }
    }

private:
    static std::uint8_t read(std::uint16_t address, bool read_only, void *instance) noexcept
    {
        auto self = static_cast<DataBus *>(instance);
        return self->read(address, read_only);
    }
    static void write(std::uint16_t address, std::uint8_t data, void *instance) noexcept
    {
        auto self = static_cast<DataBus *>(instance);
        self->write(address, data);
    };

public:
    DataBus(Devices &... devices) noexcept
      : connected_devices_{ std::addressof(devices)... }
    {
        std::apply(
            [this](auto &... device) {
                (static_cast<Device *>(device)->connect(this, &read, &write), ...);
            },
            connected_devices_);
    }

    ~DataBus() noexcept = default;

private:
    [[nodiscard]] std::uint8_t read(std::uint16_t address, [[maybe_unused]] bool read_only) noexcept
    {
        std::uint8_t result = 0;
        for_each(
            connected_devices_, [&address, &result ](const auto &d) noexcept {
                const auto &device = *static_cast<const Device *>(d);
                if (device.handles_address(address))
                {
                    const auto r = device.read_from(address);
                    if (r.first == true)
                    {
                        result = r.second;
                        return false;
                    }
                }

                return true;
            });

        return result;
    }

    void write(std::uint16_t address, std::uint8_t data) noexcept
    {
        for_each(
            connected_devices_, [&address, &data ](auto &d) noexcept {
                auto &device = *static_cast<Device *>(d);
                if (device.handles_address(address))
                {
                    const auto r = device.write_to(address, data);
                    if (r == true) { return false; }
                }

                return true;
            });
    }

private:
    std::tuple<Devices *...> connected_devices_{};
};
