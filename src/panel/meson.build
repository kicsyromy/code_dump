library_name = 'panel'

headers = [
    'include/harbor_panel.hh'
]

sources = [
    'src/harbor_panel.cc'
]
sources += headers

public_include_dirs = include_directories(
    'include'
)

private_include_dirs = include_directories(
    'src/include'
)

panel_library = static_library(
    library_name,
    sources,
    include_directories: [
        public_include_dirs,
        private_include_dirs
    ],
    dependencies: [
        utilities,
        wayland_protocols,
        nanovg,
        egl,
        glesv2,
        pthread,
        wayland_cursor,
        wayland_egl,
        wayland_server
    ]
)

panel = declare_dependency(
    link_with: panel_library,
    include_directories: public_include_dirs
)
