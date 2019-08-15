#ifndef MESH_HH
#define MESH_HH

#include <vector>

#include "triangle.hh"
#include "utils.hh"

struct mesh
{
    std::vector<triangle> triangles;

    inline static mesh load_from_object_file(const char *file_path) noexcept
    {
        using namespace utils;
        using namespace utils::file;

        mesh result;

        auto [object_file, error] = file::open(file_path, file::open_mode_t::Read);

        if (error != file::io_error_t::NoError) return {};

        std::vector<vector3f> vertex_cache;

        for (;;)
        {
            const auto opt_line = object_file.read_line();
            if (!opt_line.has_value()) break;

            const std::string_view line{ opt_line.value() };
            if (line[0] == '#' || line[0] == 's') continue;

            const auto split_line = strings::split<4>(line, ' ');

            switch (line[0])
            {
                case 'v':
                {
                    const auto x = strings::to<float>(split_line[1]);
                    const auto y = strings::to<float>(split_line[2]);
                    const auto z = strings::to<float>(split_line[3]);

                    vertex_cache.emplace_back(x, y, z);
                }
                break;
                case 'f':
                {
                    const auto vertex_a = vertex_cache[strings::to<int>(split_line[1]) - 1];
                    const auto vertex_b = vertex_cache[strings::to<int>(split_line[2]) - 1];
                    const auto vertex_c = vertex_cache[strings::to<int>(split_line[3]) - 1];

                    result.triangles.push_back({ vertex_a, vertex_b, vertex_c });
                }
                break;
            }
        }

        return result;
    }
};

#endif /* !MESH_HH */
