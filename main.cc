#include <string>
#include <vector>

#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <array>
#include <type_traits>
#include <tuple>
#include <string>
#include <vector>
#include <utility>

#include <experimental/filesystem>

constexpr auto WINEARCH{ "WINEARCH=win64" };
constexpr auto WINEDEBUG{ "WINEDEBUG=-all" };

extern "C" char **environ;

namespace process
{
    template <typename ...Args>
    struct args_t
    {
        constexpr args_t(Args &&...a) noexcept
            : args_{ std::forward<Args>(a)... }
        {}

        void add_args(const char *arg) noexcept
        {
            args_.push_back(arg);
        }

        inline std::vector<const char *> args() const
        {
            return args_;
        }

    private:
        std::vector<const char *> args_;
    };
    template <typename... T> args_t(T &&...) -> args_t<T...>;

    template<typename ...Args, typename ...Environment>
    int run(const char *executable, const args_t<Args...> *args = nullptr, const args_t<Environment...> *env = nullptr) noexcept
    {
        int exit_status = 0;

        const auto child_pid = fork();
        if (child_pid == -1)
        {
            return -1;
        }

        if (child_pid == 0) /* Code executed by child */
        {
            auto arguments = [args]() noexcept -> std::vector<const char *> {
                    if (args != nullptr) { return args->args(); } else { return {}; }
        }();

            arguments.insert(arguments.begin(), executable);
            arguments.push_back(static_cast<const char *>(nullptr));

            auto environment = [env]() noexcept -> std::vector<const char *> {
                    if (env != nullptr) { return env->args(); } else { return {}; }
        }();
            environment.reserve(64);
            for (auto system_environment = environ; *system_environment != nullptr; ++system_environment)
            {
                environment.push_back(*system_environment);
            }
            environment.push_back(static_cast<const char *>(nullptr));

            if (execvpe(executable, (char *const *)arguments.data(), (char *const *)environment.data()) == -1)
            {
                _exit(127);
            }
        }
        else /* Code executed by parent */
        {
            do
            {
                const auto w = waitpid(child_pid, &exit_status, WUNTRACED | WCONTINUED);
                if (w == -1)
                {
                    perror("waitpid");
                }

                if (WIFEXITED(exit_status))
                {
                    printf("exited, exit_status=%d\n", WEXITSTATUS(exit_status));
                }
                else if (WIFSIGNALED(exit_status))
                {
                    printf("killed by signal %d\n", WTERMSIG(exit_status));
                }
                else if (WIFSTOPPED(exit_status))
                {
                    printf("stopped by signal %d\n", WSTOPSIG(exit_status));
                }
                else if (WIFCONTINUED(exit_status))
                {
                    printf("continued\n");
                }
            }
            while (!WIFEXITED(exit_status) && !WIFSIGNALED(exit_status));
        }

        return exit_status;
    }
}

namespace strings
{
    template<typename string_collection_t>
    std::string join(const string_collection_t &string_collection, std::string_view delimiter) noexcept
    {
        std::string result;

        for (auto it = string_collection.cbegin(); it != string_collection.cend(); ++it)
        {
            if (it != string_collection.cbegin())
            {
                result += delimiter;
            }

            result += *it;
        }

        return result;
    }
}

int main(int argc, char *argv[])
{
    std::string_view program_name = std::string_view(argv[0]);
    program_name.remove_prefix(program_name.rfind('/') + 1);

    std::vector<std::string> process_arguments;
    std::string program_arg;// = ROOT_DIRECTORY"/14.21.27702/bin/Hostx64/x64/";
    program_arg += program_name;
    process_arguments.reserve(static_cast<std::size_t>(argc + 1));
    process_arguments.push_back(std::move(program_arg));

    for (int i = 1; i < argc; ++i)
    {
        std::error_code error;
        const auto file_exists = std::experimental::filesystem::exists(argv[i], error);
        const auto is_regular_file_or_directory = file_exists &&
                (std::experimental::filesystem::is_regular_file(argv[i], error) ||
                 std::experimental::filesystem::is_directory(argv[i], error));

        /* filter out params that are also linux directories */
        const bool is_not_special_linux_dir = strcmp(argv[i], "/lib") != 0;

        if (file_exists && is_regular_file_or_directory && is_not_special_linux_dir)
        {
            std::string arg = argv[i];
            const auto argument_length = arg.size();
            for (std::size_t character_index = 0; character_index < argument_length; ++character_index)
            {
                if (arg[character_index] == '/') arg[character_index] = '\\';
            }

            if (argv[i][0] == '/')
            {
                arg.insert(0, "Z:");
            }

            process_arguments.push_back(std::move(arg));
        }
        else if (std::string_view arg_view = argv[i]; arg_view.substr(0, 9) == "/LIBPATH:")
        {
            std::string arg{ arg_view };

            const auto argument_length = arg.size();
            for (std::size_t character_index = 1; character_index < argument_length; ++character_index)
            {
                if (arg[character_index] == '/') arg[character_index] = '\\';
            }

            if (argv[i][9] == '/')
            {
                arg.insert(9, "Z:");
            }

            process_arguments.push_back(std::move(arg));
        }
        else
        {
            process_arguments.emplace_back(argv[i]);
        }
    }

    process::args_t env(WINEARCH, WINEDEBUG);
    process::args_t args;

    for (std::size_t i = 0; i < process_arguments.size(); ++i)
    {
        args.add_args(process_arguments[i].data());
    }

    std::fprintf(stderr, "Executing: wine %s\n", strings::join(process_arguments, " ").data());

    process::run("wine", &args, &env);

    return 0;
}

