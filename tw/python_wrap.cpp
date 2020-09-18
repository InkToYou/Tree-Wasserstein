#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "treewasserstein.hpp"

PYBIND11_MODULE(tw, m) {
  m.doc() =
      "A python module for fast calculation of the wasserstein distance on "
      "tree metrics implemented by C++.";
  m.def("distance", &distance, "Tree Wasserstein");
}
