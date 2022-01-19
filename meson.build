project('libmatrix', 'c')

src = [
  'api.c',
  'linked_list.c',
  'matrix.c',
  'sync.c'
]

c_args = [
  '-O3',
  '-std=c11',
  '-D_GNU_SOURCE',
  '-D_FORTIFY_SOURCE=2',
  '-flto',
  '-fstack-protector-strong',
  '--param=ssp-buffer-size=4',
  '-Wall',
  '-Wextra',
  '-Wpedantic',
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
  '-Werror=incompatible-pointer-types'
]

cc = meson.get_compiler('c')

libcurl_dep = dependency('libcurl', required: true, version: '>= 7.68.0')
libcjson_dep = dependency('cjson', required: false, version: '>= 1.7.13')

if not libcjson_dep.found()
  libcjson_proj = subproject('cjson', default_options: 'tests=false')
  libcjson_dep = libcjson_proj.get_variable('libcjson_dep')
endif

incdir = include_directories('include')

libmatrix = static_library('matrix',
  src, 
  c_args: c_args,
  dependencies: [libcurl_dep, libcjson_dep],
  include_directories: incdir,
  link_args: ['-lpthread'],
)

libmatrix_dep = declare_dependency(include_directories: incdir, link_with: libmatrix)
