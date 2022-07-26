// main.cc
#include "../pybind11/embed.h"
#include <dlfcn.h>
namespace py = pybind11;

extern "C" {
void * python;

int create() {
  printf("dddd");
  python = dlopen("libpython3.so", RTLD_NOW | RTLD_GLOBAL);
  return 0;
}

int destroy() {
  dlclose(python);
  return 0;
}

int main() {
  py::scoped_interpreter guard{};
  auto py_module = py::module::import("numpy");
  auto version   = py_module.attr("__version__");
  py::print(version);
  return 0;
}
}
