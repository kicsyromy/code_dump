#include <cmath>
#include <cstdio>
#include <ctime>
#include <sys/timeb.h>
#undef PI

#include <array>
#include <chrono>
#include <memory>

#include <conio.h>

#include "fps.hh"
#include "matrix.hh"
#include "vga.hh"

#include <allegro.h>

#include <GL/glu.h>
#include <GL/osmesa.h>

#include <linalg/linalg.h>

constexpr auto PI = static_cast<float>(M_PI);
constexpr auto RAD(const float d) noexcept { return PI / 180.f * d; }
constexpr auto FOV{ 30 };
constexpr auto FOV_RAD{ RAD(FOV) };

using v3f = linalg::vec<float, 3>;
const std::array cube_mesh{
    // SOUTH
    v3f{ -0.5f, -0.5f, -0.5f }, v3f{ -0.5f, 0.5f, -0.5f }, v3f{ 0.5f, 0.5f, -0.5f },
    v3f{ -0.5f, -0.5f, -0.5f }, v3f{ 0.5f, 0.5f, -0.5f }, v3f{ 0.5f, -0.5f, -0.5f },

    // EAST
    v3f{ 0.5f, -0.5f, -0.5f }, v3f{ 0.5f, 0.5f, -0.5f }, v3f{ 0.5f, 0.5f, 0.5f },
    v3f{ 0.5f, -0.5f, -0.5f }, v3f{ 0.5f, 0.5f, 0.5f }, v3f{ 0.5f, -0.5f, 0.5f },

    // NORTH
    v3f{ 0.5f, -0.5f, 0.5f }, v3f{ 0.5f, 0.5f, 0.5f }, v3f{ -0.5f, 0.5f, 0.5f },
    v3f{ 0.5f, -0.5f, 0.5f }, v3f{ -0.5f, 0.5f, 0.5f }, v3f{ -0.5f, -0.5f, 0.5f },

    // WEST
    v3f{ -0.5f, -0.5f, 0.5f }, v3f{ -0.5f, 0.5f, 0.5f }, v3f{ -0.5f, 0.5f, -0.5f },
    v3f{ -0.5f, -0.5f, 0.5f }, v3f{ -0.5f, 0.5f, -0.5f }, v3f{ -0.5f, -0.5f, -0.5f },

    // TOP
    v3f{ -0.5f, 0.5f, -0.5f }, v3f{ -0.5f, 0.5f, 0.5f }, v3f{ 0.5f, 0.5f, 0.5f },
    v3f{ -0.5f, 0.5f, -0.5f }, v3f{ 0.5f, 0.5f, 0.5f }, v3f{ 0.5f, 0.5f, -0.5f },

    // BOTTOM
    v3f{ 0.5f, -0.5f, 0.5f }, v3f{ -0.5f, -0.5f, 0.5f }, v3f{ -0.5f, -0.5f, -0.5f },
    v3f{ 0.5f, -0.5f, 0.5f }, v3f{ -0.5f, -0.5f, -0.5f }, v3f{ 0.5f, -0.5f, -0.5f }
};

int main()
{
    if (allegro_init() != 0)
    {
        std::puts("Failed to start allegro");
        getch();
        return -1;
    }

    install_timer();
    //    install_keyboard();
    int buttons = install_mouse();
    if (buttons <= 0)
    {
        std::puts("Mouse could not be initialized");
    }

    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, nullptr))
    {
        allegro_message("Sound Error: %s", allegro_error);
    }

    auto screen_buffer = create_bitmap_ex(32, VGA::SCREEN_WIDTH, VGA::SCREEN_HEIGHT);

    auto gl_context = OSMesaCreateContext(OSMESA_RGBA, nullptr);
    if (OSMesaMakeCurrent(gl_context, screen_buffer->dat, GL_UNSIGNED_BYTE, VGA::SCREEN_WIDTH,
                          VGA::SCREEN_HEIGHT) != true)
    {
        printf("Failed to create GL context %d", glGetError());
        getch();
        return -1;
    }

    OSMesaPixelStore(OSMESA_Y_UP, true);

    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT, VGA::SCREEN_WIDTH, VGA::SCREEN_HEIGHT, 0, 0))
    {
        allegro_message("Video Error: %s", allegro_error);
        getch();
        return -1;
    }

    RGB_MAP rgb_table;
    create_rgb_table(&rgb_table, default_palette, nullptr);
    rgb_map = &rgb_table;

    std::array<v3f, 12> surface_normals;
    std::size_t surface_normals_index = 0;
    for (auto i = 0ul; i < cube_mesh.size(); i += 3)
    {
        const auto line_a =
            v3f{ cube_mesh[i + 1].x - cube_mesh[i].x, cube_mesh[i + 1].y - cube_mesh[i].y,
                 cube_mesh[i + 1].z - cube_mesh[i].z };

        const auto line_b =
            v3f{ cube_mesh[i + 2].x - cube_mesh[i].x, cube_mesh[i + 2].y - cube_mesh[i].y,
                 cube_mesh[i + 2].z - cube_mesh[i].z };

        const auto result = linalg::normalize(linalg::cross(line_a, line_b));

        surface_normals[surface_normals_index++] = { result.x, result.y, result.z };
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    linalg::vec<float, 3> camera{ 0, 0, 3 };
    linalg::vec<float, 3> up{ 0, 1, 0 };
    linalg::vec<float, 3> center{ 0, 0, 0 };

    const auto projection_matrix = matrix::perspective_projection(
        90, 0.1f, 1000.f, 1.f * VGA::SCREEN_WIDTH / VGA::SCREEN_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //    glLoadMatrixf(matrix::as_array(projection_matrix));
    gluPerspective(90, 1.0 * VGA::SCREEN_WIDTH / VGA::SCREEN_HEIGHT, 0.1, 1000);
    gluLookAt(camera.x, camera.y, camera.z, center.x, center.y, center.z, up.x, up.y, up.z);

    struct timeb start, end;
    float angle = 0.f;

    ftime(&start);

    fps_counter fps;
    for (;;)
    {
        ftime(&end);
        const auto elapsed_time =
            1000.f * (end.time - start.time) + (end.millitm - start.millitm) / 1000.f;

        if (kbhit())
        {
            auto key = getch();
            if (key == 'q' || key == 'Q') break;
        }

        //            if (key == 0)
        //            {
        //                key = getch();
        //                std::printf("\r%d", key);
        //                if (key == 75)
        //                {
        //                    camera.x += 1.f * elapsed_time;
        //                }
        //                if (key == 77)
        //                {
        //                    camera.x -= 1.f * elapsed_time;
        //                }
        //                if (key == 72)
        //                {
        //                    camera.z += 1.f * elapsed_time;
        //                }
        //                if (key == 80)
        //                {
        //                    camera.z -= 1.f * elapsed_time;
        //                }

        //                glMatrixMode(GL_PROJECTION);
        //                glLoadIdentity();
        //                gluPerspective(90, 1.0 * VGA::SCREEN_WIDTH / VGA::SCREEN_HEIGHT, 0.1, 1000);
        //                gluLookAt(camera.x, camera.y, camera.z, center.x, center.y, center.z, up.x, up.y,
        //                          up.z);
        //            }
        //        }

        angle += 1.f * elapsed_time;

        ftime(&start);

        glClearColor(36.f / 256, 194.f / 256, 38.f / 256, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glColor3f(36.f / 256, 112.f / 256, 194.f / 256);

        const auto rotation_mat_y = matrix::rotation<matrix::axis::Y>(angle);
        const auto rotation_mat_z = matrix::rotation<matrix::axis::Z>(angle * 0.5f);
        const auto rotation_mat = linalg::mul(rotation_mat_y, rotation_mat_z);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(reinterpret_cast<const float *>(&rotation_mat));

        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, reinterpret_cast<const float *>(cube_mesh.data()));
        glNormalPointer(GL_FLOAT, 0, reinterpret_cast<const float *>(surface_normals.data()));

        glDrawArrays(GL_TRIANGLES, 0, cube_mesh.size());

        glDisableClientState(GL_VERTEX_ARRAY);

        glFlush();

        fps.update();
        textprintf_ex(screen_buffer, font, 0, 0, makecol32(255, 0, 0), 0, "%d", fps.get());
        show_mouse(screen_buffer);

        blit(screen_buffer, screen, 0, 0, 0, 0, VGA::SCREEN_WIDTH, VGA::SCREEN_HEIGHT);
    }

    OSMesaDestroyContext(gl_context);
}
