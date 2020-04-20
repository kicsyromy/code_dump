#pragma once

#include <cassert>
#include <vector>

template<typename... Args> class Signal
{
public:
    template<typename user_data_t>
    void connect(user_data_t *user_data, void (*callback)(Args..., void *)) noexcept
    {
        assert(user_data != nullptr);
        connections_.emplace_back(callback, user_data);
    }

    void connect(void (*callback)(Args...)) noexcept
    {
        auto fn = reinterpret_cast<void *>(callback);
        connections_.emplace_back(reinterpret_cast<void (*)(Args..., void *)>(fn), nullptr);
    }

    void emit(Args... args) const
    {
        for (const auto &connection : connections_)
        { connection.first(std::forward<Args>(args)..., connection.second); }
    }

private:
    std::vector<std::pair<void (*)(Args..., void *), void *>> connections_{};
};
