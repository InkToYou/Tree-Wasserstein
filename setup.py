from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import os
import setuptools
import tempfile

# This code is based on https://github.com/pybind/python_example/blob/master/setup.py


def get_pybind_includepath():
  import pybind11
  return pybind11.get_include()


def has_flag(compiler, flagname):
  with tempfile.NamedTemporaryFile('w', suffix='.cpp', delete=False) as f:
    f.write('int main (int argc, char **argv) { return 0; }')
    fname = f.name
  try:
    compiler.compile([fname], extra_postargs=[flagname])
  except setuptools.distutils.errors.CompileError:
    return False
  finally:
    try:
      os.remove(fname)
    except OSError:
      pass
  return True


def cpp_flag(compiler):
  flags = ['-std=c++17', '-std=c++14', '-std=c++11']

  for flag in flags:
    if has_flag(compiler, flag):
      return flag

  raise RuntimeError('Unsupported compiler')


ext_modules = [
    Extension(
        name='tw',
        sources=['tw/python_wrap.cpp'],
        include_dirs=[get_pybind_includepath()],
        language='c++'
    ),
]


class BuildExt(build_ext):
  c_opts = {
      'msvc': ['/EHsc'],
      'unix': [],
  }
  l_opts = {
      'msvc': [],
      'unix': [],
  }

  def build_extensions(self):

    if sys.platform == 'darwin':

      if has_flag(self.compiler, '-stdlib=libc++'):
        self.c_opts['unix'].append('-stdlib=libc++')
        self.l_opts['unix'].append('-stdlib=libc++')

      if has_flag(self.compiler, '-mmacosx-version-min=10.7'):
        self.c_opts['unix'].append('-mmacosx-version-min=10.7')
        self.l_opts['unix'].append('-mmacosx-version-min=10.7')

    cplr_type = self.compiler.compiler_type
    opts = self.c_opts.get(cplr_type, [])
    link_opts = self.l_opts.get(cplr_type, [])

    if cplr_type == 'unix':
      opts.append(cpp_flag(self.compiler))
      if has_flag(self.compiler, '-fvisibility=hidden'):
        opts.append('-fvisibility=hidden')

    for ext in self.extensions:
      ext.define_macros = [
          ('VERSION_INFO', f'"{self.distribution.get_version()}"')]
      ext.extra_compile_args = opts
      ext.extra_link_args = link_opts
    build_ext.build_extensions(self)


setup(
    name='tw',
    version="0.0.1",
    author='cojiro',
    description='A python module for fast calculation of the wasserstein distance on tree metrics implemented by C++.',
    ext_modules=ext_modules,
    setup_requires=['pybind11>=2.4.3'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
)
