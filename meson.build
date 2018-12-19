project(
    'harbor', 'cpp', 'c',
    default_options : [
        'c_std=c11',
        'cpp_std=c++17'
    ]
)

add_global_arguments('-Wno-extra-semi', language : 'cpp')

egl = dependency('egl')
glesv2 = dependency('glesv2')
pixman = dependency('pixman-1')
wayland_client = dependency('wayland-client')
wayland_cursor = dependency('wayland-cursor')
wayland_egl = dependency('wayland-egl')
wayland_server = dependency('wayland-server')
pthread = dependency('threads')
libinput = dependency('libinput')
nanovg = dependency('nanovg', fallback: ['nanovg', 'nanovg'])
wlroots = dependency('wlroots', fallback: ['wlroots/wlroots', 'wlroots'])
wlr_protos = dependency('wlr_protos', fallback: ['wlroots/wlroots', 'wlr_protos'])
xkbcommon = dependency('xkbcommon')

subdir('src')
