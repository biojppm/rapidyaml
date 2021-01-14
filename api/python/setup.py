#!/usr/bin/env python
from pathlib import Path
from setuptools import setup
from cmake_build_extension import BuildExtension, CMakeExtension

# define a CMake package
cmake_args = dict(
    name='ryml.ryml',
    install_prefix='',
    source_dir=str(Path("../../").absolute()),
    cmake_component='python',
    cmake_configure_options=[
        "-DRYML_BUILD_API:BOOL=ON",
        ])

try:
    ext = CMakeExtension(**cmake_args)
except TypeError:
    # FIXME: cmake_build_extension may not support cmake_component.
    del cmake_args['cmake_component']
    ext = CMakeExtension(**cmake_args)

setup(name='rapidyaml',
    version='1.0',
    packages=['ryml'],
    ext_modules=[ext],
    cmdclass=dict(build_ext=BuildExtension),
    author='Joao Paulo Magalhaes',
    url='https://github.com/biojppm/rapidyaml',
    description='This is a test',
    license='MIT',
    )
