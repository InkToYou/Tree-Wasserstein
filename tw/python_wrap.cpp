#include <pybind11/chrono.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "treewasserstein.cpp"

PYBIND11_MODULE(tw, m) {
  m.doc() = "pybind11";
  m.def("tw", &tw, "Tree Wasserstein");
}
