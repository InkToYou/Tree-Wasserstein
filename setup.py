from pybind11 import get_include
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

ext_modules = [
    Pybind11Extension(
        name="tw",
        sources=["tw/python_wrap.cpp"],
        include_dirs=[get_include()],
        extra_compile_arges=["-std=c++17"],
        language="c++",
    ),
]


setup(
    name="tw",
    version="0.0.1",
    author="cojiro",
    description="A python module for fast calculation of the wasserstein distance on tree metrics implemented by C++.",
    ext_modules=ext_modules,
    setup_requires=["pybind11>=2.6.0"],
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
)
