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
libcurl = cc.find_library('libcurl')

cjson = subproject('cjson')
libcjson = cjson.get_variable('libcjson_dep')

matrix = static_library('matrix', 
  src, 
  c_args: c_args,
  dependencies: [libcurl, libcjson],
  link_args: ['-lpthread'],
)

matrix_dep = declare_dependency(include_directories: ['./'], link_with: [matrix])
