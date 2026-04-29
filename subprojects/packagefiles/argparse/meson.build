project(
  'argparse', 
  'cpp',
  version : '3.1',
  default_options : ['cpp_std=c++20']
)

argparse_dep = declare_dependency(
  include_directories : include_directories('include')
)

meson.override_dependency('argparse', argparse_dep)
