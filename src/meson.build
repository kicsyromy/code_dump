subdir('wayland-protocols')
subdir('utilities')
subdir('panel')
subdir('compositor')

sources = [
    'main.cc'
]

executable(
    'harbor',
    sources,
    dependencies : [ 
        compositor,
        panel,
        utilities,
        miral,
        freetype2
    ]
)

