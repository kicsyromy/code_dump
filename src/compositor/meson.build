library_name = 'compositor'

headers = [
    'include/harbor_compositor.hh',
    'include/harbor_window_manager.hh'
]

sources = [
    'src/harbor_compositor.cc',
    'src/harbor_window_manager.cc',
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
        miral,
        freetype2
    ]
)

compositor = declare_dependency(
    link_with: compositor_library,
    include_directories: public_include_dirs
)
