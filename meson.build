# Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
# SPDX-License-Identifier: MIT

project('SerializableFSM', 'cpp',
  version: '0.9.0',
  default_options: ['werror=true', 'warning_level=3'])

magic_enum_dep = dependency('magic_enum')

SerializableFSM_inc = include_directories('.')

SerializableFSM_dep = declare_dependency(
  include_directories: SerializableFSM_inc,
  dependencies: magic_enum_dep
)

install_headers('SerializableFSM.hpp')


# examples
build_examples = get_option('build_examples')

ex_simple_switch_exe = executable('simple_switch', 'examples/simple_switch.cpp',
  dependencies: SerializableFSM_dep,
  build_by_default: build_examples)
