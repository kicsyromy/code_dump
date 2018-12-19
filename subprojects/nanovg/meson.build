library_name = 'nanovg'

headers = [
]

sources = [
    'nanovg.c'
]
sources += headers

public_include_dirs = include_directories(
    '.'
)

nanovg_library = static_library(
    library_name,
    sources,
    include_directories: public_include_dirs,
    dependencies: []
)

nanovg = declare_dependency(
    link_with: nanovg_library,
    include_directories: public_include_dirs
)

