# Copyright © 2024 Stephan Lachnit <stephanlachnit@debian.org>
# SPDX-License-Identifier: MIT

project('SerializableFSM', 'cpp',
  version: '0.9.0',
  default_options: ['cpp_std=c++20', 'warning_level=3', 'werror=true'])

magic_enum_dep = dependency('magic_enum')

SerializableFSM_inc = include_directories('.')

SerializableFSM_dep = declare_dependency(
  include_directories: SerializableFSM_inc,
  dependencies: magic_enum_dep,
)

install_headers('SerializableFSM.hpp')

# examples
subdir('examples')
