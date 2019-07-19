#ifndef RML_FILE_H
#define RML_FILE_H

#include <array>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <utility>

#include <errno.h>

#ifdef _WIN32
#include <io.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace rml
{
    namespace file
    {
        enum class open_mode_t
        {
            Read,
            Write,
            Append,
            ReadWrite,
            ReadAppend
        };

        enum class io_error_t
        {
            NoError,
            UnknownError
        };

        class file_t
        {
            using file_handle_t = std::unique_ptr<std::FILE, decltype(&std::fclose)>;

            static constexpr const char *open_modes[]{ "r", "w", "a", "rw", "ra" };

        public:
            auto open(const char *path, open_mode_t open_mode) noexcept -> io_error_t
            {
                auto handle = file_handle_t{
                    std::fopen(path, open_modes[static_cast<std::size_t>(open_mode)]), &std::fclose
                };

                if (handle == nullptr)
                {
                    int error = errno;
                    return io_error_t::UnknownError;
                }

                const auto file_descriptor = fileno(handle.get());

#ifdef _WIN32
                const auto length = _filelengthi64(file_descriptor);
                if (length == -1)
                {
                    return io_error_t::UnknownError;
                }
#else
                struct stat stbuf;
                if ((fstat(file_descriptor, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
                {
                    return io_error_t::UnknownError;
                }

                const auto length = stbuf.st_size;
#endif
                handle_ = std::move(handle);
                file_size_ = static_cast<std::size_t>(length);

                return io_error_t::NoError;
            }

            auto close() noexcept -> void { handle_.reset(); }

            auto is_open() noexcept -> bool { return handle_ != nullptr; }

            template <std::size_t buffer_size = 1024>
            auto read_line() noexcept -> std::optional<std::string>
            {
                std::string result;
                result.reserve(buffer_size);

                char buffer[buffer_size];

                for (;;)
                {
                    const auto readResult = std::fgets(buffer, buffer_size, handle_.get());

                    if (readResult != nullptr)
                    {
                        result += readResult;
                        if (result.back() == '\n')
                        {
                            result.erase(result.end() - 1);
                            break;
                        }
                    }
                    else if (!result.empty())
                    {
                        break;
                    }
                    else
                    {
                        return std::nullopt;
                    }
                }

                return result;
            }

            auto read_all() noexcept -> std::string
            {
                std::string result;
                result.resize(file_size_);

                const auto current_position = std::ftell(handle_.get());

                std::fseek(handle_.get(), 0, SEEK_SET);
                std::fread(result.data(), 1, file_size_, handle_.get());

                std::fseek(handle_.get(), current_position, SEEK_SET);

                return result;
            }

            template <typename T, std::size_t count = 1> T read_binary(T * = nullptr) noexcept
            {
                static_assert(std::is_default_constructible_v<T>);

                T value;
                std::fread(&value, sizeof(T), count, handle_.get());

                return value;
            }

        private:
            file_handle_t handle_{ nullptr, nullptr };
            std::size_t file_size_{ 0 };
        };

        auto open(const char *path, open_mode_t open_mode) noexcept -> std::pair<file_t, io_error_t>
        {
            auto file = file_t{};
            const auto error = file.open(path, open_mode);
            return { std::move(file), error };
        }

        auto open(std::string_view path, open_mode_t open_mode) noexcept
            -> std::pair<file_t, io_error_t>
        {
            return open(path.data(), open_mode);
        }
        auto open(const std::string &path, open_mode_t open_mode) noexcept
            -> std::pair<file_t, io_error_t>
        {
            return open(path.c_str(), open_mode);
        }

        auto exists(const char *path) noexcept -> std::pair<bool, io_error_t>
        {
            std::error_code error;
            const auto result = std::filesystem::exists(path, error);

            return { result, error ? io_error_t::UnknownError : io_error_t::NoError };
        }

        auto exists(std::string_view path) noexcept -> std::pair<bool, io_error_t>
        {
            return exists(path.data());
        }

        auto exists(const std::string &path) noexcept -> std::pair<bool, io_error_t>
        {
            return exists(path.c_str());
        }
    } // namespace file
} // namespace rml

#endif /* !RML_FILE_H */
