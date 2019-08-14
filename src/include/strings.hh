#ifndef UTILS_STRINGS_HH
#define UTILS_STRINGS_HH

#include <array>
#include <charconv>
#include <string>
#include <string_view>
#include <vector>

namespace utils
{
    namespace strings
    {
        template <typename output_iterator_t>
        void split(std::string_view string, char delimiter, output_iterator_t first) noexcept
        {
            for_each_substring(string, delimiter, [&first](std::string_view s) -> bool {
                *first++ = std::move(s);
                return true;
            });
        }

        template <std::size_t count>
        std::array<std::string_view, count> split(
            std::string_view string,
            char delimiter,
            std::array<std::string_view, count> * = nullptr) noexcept
        {
            std::array<std::string_view, count> result;

            std::size_t it = 0;
            for_each_substring(string, delimiter, [&result, &it](std::string_view s) -> bool {
                result[it++] = std::move(s);

                return it < result.size();
            });

            return result;
        }

        template <typename Callback>
        void for_each_substring(std::string_view string,
                                char delimiter,
                                Callback &&callback) noexcept
        {
            while (string.size() > 0)
            {
                const auto last_index = string.size() - 1;

                for (std::size_t i = 0; i < string.size(); ++i)
                {
                    if ((string[i] == delimiter) || (i == last_index))
                    {
                        if (!callback(
                                std::string_view(string.data(), (i < last_index) ? i : i + 1)))
                        {
                            return;
                        }

                        string.remove_prefix(i + 1);
                        break;
                    }
                }
            }
        }

        template <typename T> constexpr T to(const std::string_view &string, T * = nullptr) noexcept
        {
            static_assert(std::is_default_constructible_v<T>);

            constexpr const char *format_string = []() noexcept
            {
                if constexpr (std::is_same_v<std::decay_t<T>, float>)
                {
                    return "%f";
                }

                if constexpr (std::is_same_v<std::decay_t<T>, int>)
                {
                    return "%d";
                }
            }
            ();

            T result;
            std::sscanf(string.data(), format_string, &result);
            return result;
        }
    } // namespace strings
} // namespace utils

#endif /* UTILS_STRINGS_HH */
