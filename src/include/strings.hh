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

            /*
                %d : Scan an integer as a signed decimal number.
                %i : Scan an integer as a signed number. Similar to %d, but interprets the number as hexadecimal when preceded by 0x and octal when preceded by 0. For example, the string 031 would be read as 31 using %d, and 25 using %i. The flag h in %hi indicates conversion to a short and hh conversion to a char.
                %u : Scan for decimal unsigned int (Note that in the C99 standard the input value minus sign is optional, so if a minus sign is read, no errors will arise and the result will be the two's complement of a negative number, likely a very large value. See strtoul().[failed verification]) Correspondingly, %hu scans for an unsigned short and %hhu for an unsigned char.
                %f : Scan a floating-point number in normal (fixed-point) notation.
                %g, %G : Scan a floating-point number in either normal or exponential notation. %g uses lower-case letters and %G uses upper-case.
                %x, %X : Scan an integer as an unsigned hexadecimal number.
                %o : Scan an integer as an octal number.
                %s : Scan a character string. The scan terminates at whitespace. A null character is stored at the end of the string, which means that the buffer supplied must be at least one character longer than the specified input length.
                %c : Scan a character (char). No null character is added.
                whitespace: Any whitespace characters trigger a scan for zero or more whitespace characters. The number and type of whitespace characters do not need to match in either direction.
                %lf : Scan as a double floating-point number.
                %Lf : Scan as a long double floating-point number.
             */

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
