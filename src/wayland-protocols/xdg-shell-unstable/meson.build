library_name = 'xdg-shell-unstable'

headers = [
    'xdg-shell-unstable-v6-protocol.h',
]

sources = [
    'xdg-shell-unstable-v6-protocol.c'
]
sources += headers

public_include_dirs = include_directories(
    '.'
)

xdg_shell_library = static_library(
    library_name,
    sources,
    include_directories: public_include_dirs,
    dependencies: []
)

xdg_shell_unstable = declare_dependency(
    link_with: xdg_shell_library,
    include_directories: public_include_dirs
)

