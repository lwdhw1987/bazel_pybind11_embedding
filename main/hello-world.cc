#include <iostream>
#include <string>
#include "hello-time.h"
#include "main/hello-greet.h"
#include "pybind11/embed.h"

namespace py = pybind11;

int main(int argc, char** argv) {
  std::string who = "world";
  if (argc > 1) {
    who = argv[1];
  }
  std::cout << get_greet(who) << std::endl;
  print_localtime();

  int py_argc = 9;
  py::scoped_interpreter guard{};
  wchar_t** wargv = PyMem_New(wchar_t*, py_argc);
  std::string py_argv[py_argc] = {"convert.py",
                                  "--input",
                                  "resnet_v1_50_inference.pb",
                                  "--inputs",
                                  "input:0",
                                  "--outputs",
                                  "resnet_v1_50/predictions/Reshape_1:0",
                                  "--output",
                                  "resnet50.onnx"};
  for (int i = 0; i < py_argc; i++) {
    wargv[i] = Py_DecodeLocale(py_argv[i].c_str(), nullptr);
    if (wargv[i] == nullptr) {
      return EXIT_FAILURE;
    }
  }

  PySys_SetArgv(py_argc, wargv);

  py::module_ sys = py::module_::import("sys");
  // py::print(sys.attr("path"));
  py::object convert = py::module_::import("tf2onnx.convert");
  py::object do_convert = convert.attr("main");
  do_convert();
  py::print(py::module::import("sys").attr("argv"));

  for (int i = 0; i < py_argc; i++) {
    PyMem_Del(wargv[i]);
    wargv[i] = nullptr;
  }
  return 0;
}
