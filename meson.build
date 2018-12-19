project(
    'harbor', 'cpp', 'c',
    default_options : ['cpp_std=c++17']
)

cpp = meson.get_compiler('cpp')
cpp_args = []
cpp_link_args = []

add_global_arguments(cpp_args, language : 'cpp')
add_global_link_arguments(cpp_link_args, language : 'cpp')

egl = dependency('egl')
freetype2 = dependency('freetype2')
glesv2 = dependency('glesv2')
miral = dependency('miral')
wayland_client = dependency('wayland-client')
wayland_cursor = dependency('wayland-cursor')
wayland_egl = dependency('wayland-egl')
wayland_server = dependency('wayland-server')
pthread = dependency('threads')
nanovg = dependency('nanovg', fallback: ['nanovg', 'nanovg'])
wlroots = dependency('wlroots', fallback: ['wlroots/wlroots', 'wlroots'])

subdir('src')
