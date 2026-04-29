project(
  'delaunay',
  ['c', 'cpp'],
  default_options: ['cpp_std=c++20']
)

# triangle_inc = include_directories('triangle')

# triangle_lib = static_library(
#   'triangle',
#   'triangle/triangle.c',
#   include_directories : triangle_inc,
#   install : false,
# )

argparse_dep = dependency('argparse')

delaunay_exe = executable(
  'delaunay',
  ['src/main.c++', 'src/quadedge.c++', 'src/predicates.c'],
  dependencies: argparse_dep,
  # include_directories : triangle_inc,
  # link_with : triangle_lib,
  install : false,
)

test_exe = executable(
  'topology_tests',
  ['tests/test.c++', 'src/quadedge.c++', 'src/predicates.c'],
  # include_directories : triangle_inc,
  # link_with : triangle_lib,
  install : false,
)

x11_dep = dependency('x11', required : true)

showme_exe = executable(
  'showme',
  'triangle/showme.c',
  dependencies : x11_dep,
  install : false,
)

triangle_exe = executable(
  'triangle',
  'triangle/triangle.c',
  install : false,
)
