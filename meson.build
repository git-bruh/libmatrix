project(
    'libmatrix',
    ['c', 'cpp'],
    default_options: [
        'buildtype=debugoptimized',
        'warning_level=3',
        'b_lto=true',
        'c_std=c11',
    ],
)

cc = meson.get_compiler('c')

src = ['src/api.c', 'src/linked_list.c', 'src/matrix.c', 'src/sync.c']

tests = ['test_parsing']

c_args = [
    '-D_GNU_SOURCE',
    '-D_FORTIFY_SOURCE=2',
    '-fstack-protector-strong',
    '--param=ssp-buffer-size=4',
]

warning_c_args = [
    '-Wshadow',
    '-Wnull-dereference',
    '-Wformat=2',
    '-Wcast-qual',
    '-Wconversion',
    '-Wpointer-arith',
    '-Wunused-macros',
    '-Wredundant-decls',
    '-Wwrite-strings',
    '-Werror=int-conversion',
    '-Werror=implicit-function-declaration',
    '-Werror=incompatible-pointer-types',
]

add_project_arguments(c_args, language: 'c')
add_project_arguments(cc.get_supported_arguments(warning_c_args), language: 'c')

libcurl_dep = dependency('libcurl', required: true, version: '>= 7.68.0')
libcjson_dep = dependency('cjson', required: false, version: '>= 1.7.13')
threads_dep = dependency('threads', required: true)
m_dep = cc.find_library('m', required: false)

if not libcjson_dep.found()
    libcjson_proj = subproject(
        'cjson',
        default_options: ['tests=false', 'default_library=static'],
    )
    libcjson_dep = libcjson_proj.get_variable('libcjson_dep')
endif

incdir = include_directories('include')

libmatrix = static_library(
    'matrix',
    src,
    dependencies: [libcurl_dep, libcjson_dep, threads_dep, m_dep],
    include_directories: incdir,
)

libmatrix_dep = declare_dependency(
    include_directories: incdir,
    link_with: libmatrix,
)

if get_option('tests')
    unity_proj = subproject('unity', default_options: ['werror=false'])
    unity_dep = unity_proj.get_variable('unity_dep')

    foreach test_name : tests
        exe = executable(
            test_name,
            'tests/@0@.cc'.format(test_name),
            dependencies: [libmatrix_dep, unity_dep],
        )
        test(test_name, exe)
    endforeach
endif
