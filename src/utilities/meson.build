library_name = 'utilities'

headers = [
    'include/egl/harbor_egl_config.hh',
    'include/egl/harbor_egl_context.hh',
    'include/egl/harbor_egl_display.hh',
    'include/egl/harbor_egl_surface.hh',
    'include/egl/harbor_egl_types.hh',
    'include/wayland/harbor_wl_registry.hh',
    'include/xdg/harbor_xdg_surface.hh',
    'include/harbor_logger.hh',
    'include/harbor_signal.hh',
    'include/harbor_utilities.hh'
]

sources = [
    'src/harbor_utilities.cc'
]
sources += headers

public_include_dirs = include_directories(
    'include'
)

private_include_dirs = include_directories(
    'src/include'
)

utilities_library = static_library(
    library_name,
    sources,
    include_directories: [
        public_include_dirs,
        private_include_dirs
    ],
    dependencies: [
        wayland_protocols
    ]
)

utilities = declare_dependency(
    link_with: utilities_library,
    include_directories: public_include_dirs
)
