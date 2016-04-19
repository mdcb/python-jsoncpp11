#
# GMTO Corp.
#

from distutils.core import setup, Extension
from os import environ as env

version = '0.1'

extra_compile_args=[
  '-Wall',
  '-Wextra',
  '-pedantic',
  '--std=c++11',
  '-pthread',
]

extra_link_args = [
  '-pthread'
]

define_macros = [
]
undef_macros = [
  'NDEBUG',
]

include_dirs=[]
include_dirs.extend([
  '/usr/include/jsoncpp',
  '/home/mdcb/work/webrepos/pybind11/include',
])


setup(name='python3-jsoncpp11',
      version=version,
      description='jsoncpp for python3',
      long_description='jsoncpp for python3.',
      author='Matthieu Bec',
      author_email='mdcb808@gmail.com',
      url='https://github.com/mdcb/emf',
      license='GPL',
      ext_modules=[
         Extension(
            name='jsoncpp11',
            sources = [
              'extension.cpp',
            ],
            include_dirs=include_dirs,
            undef_macros=undef_macros,
            define_macros=define_macros,
            library_dirs=[
            ],
            libraries = [
              'jsoncpp',
            ],
            #runtime_library_dirs=[], # XXX rpath bad for rpm
            extra_compile_args = extra_compile_args,
            extra_link_args = extra_link_args,
            ),
         ],
)

