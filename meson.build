project(
  'delaunay',
  ['c', 'cpp'],
  default_options: ['cpp_std=c++20']
)

argparse_dep = dependency('argparse')

delaunay_exe = executable(
  'delaunay',
  ['src/main.c++', 'src/quadedge.c++', 'src/predicates.c'],
  dependencies: argparse_dep,
  install : false,
)

test_exe = executable(
  'topology_tests',
  ['tests/test.c++', 'src/quadedge.c++', 'src/predicates.c'],
  install : false,
)

x11_dep = dependency('x11', required : true)

showme_exe = executable(
  'showme',
  'triangle/showme.c',
  dependencies : x11_dep,
  install : false,
)

