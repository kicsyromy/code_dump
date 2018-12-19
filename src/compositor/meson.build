library_name = 'compositor'

headers = [
    'include/bindings.h',
    'include/config.h',
    'include/cursor.h',
    'include/desktop.h',
    'include/ini.h',
    'include/input.h',
    'include/keyboard.h',
    'include/layers.h',
    'include/output.h',
    'include/seat.h',
    'include/server.h',
    'include/switch.h',
    'include/text_input.h',
    'include/view.h',
    'include/virtual_keyboard.h',
    'include/xcursor.h'
]

sources = [
    'src/bindings.c',
    'src/config.c',
    'src/cursor.c',
    'src/desktop.c',
    'src/ini.c',
    'src/input.c',
    'src/keyboard.c',
    'src/layer_shell.c',
    'src/main.c',
    'src/output.c',
    'src/seat.c',
    'src/switch.c',
    'src/text_input.c',
    'src/virtual_keyboard.c',
    'src/wl_shell.c',
    'src/xdg_shell.c',
    'src/xdg_shell_v6.c',
    'src/xwayland.c',
    'src/test.cc'
]
sources += headers

public_include_dirs = include_directories(
    'include'
)

compositor_library = static_library(
    library_name,
    sources,
    include_directories: public_include_dirs,
    dependencies: [
        panel,
        utilities,
        wayland_protocols,
        nanovg,
        pixman,
        libinput,
        wlroots,
        wlr_protos,
        xkbcommon
    ],
    c_args: [
        '-DWLR_USE_UNSTABLE'
    ]
)

compositor = declare_dependency(
    link_with: compositor_library,
    include_directories: public_include_dirs
)
