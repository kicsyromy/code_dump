project(
    'nanovg', 'c',
)

sources = [
    'nanovg/src/nanovg.c'
]

public_include_dirs = include_directories(
    'nanovg/src'
)

nanovg_library = static_library(
    'nanovg',
    sources,
    include_directories: public_include_dirs,
    dependencies: []
)

nanovg = declare_dependency(
    link_with: nanovg_library,
    include_directories: public_include_dirs
)
