# bazel_pybind11_embedding
this repo origins from [c++ bazel build example](https://github.com/bazelbuild/examples/tree/master/cpp-tutorial/stage3)

in addtion to the original example, I made 3 important changes:
- supporting gtest
- supporting pybind11
- embedding python interpreter in C++ and accessing Python libraries from C++ 

solutions:
- use one of the repository functions in the bazel [WORKSPACE](https://github.com/lwdhw1987/bazel_pybind11_embedding/blob/master/WORKSPACE) file to download Google Test / PyBind11 and make it available in your repository
- depend on these external libraries with cc_* rules in bazel [BUILD](https://github.com/lwdhw1987/bazel_pybind11_embedding/blob/master/main/BUILD) file
- Accessing Python libraries from C++ by python interpreter and Python C++ interface, see hello-world.cc for details

### WORKSPACE
```
http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-master",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/master.zip"],
)

http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11.BUILD",
  strip_prefix = "pybind11-2.6.2",
  urls = ["https://github.com/pybind/pybind11/archive/v2.6.2.tar.gz"],
```

### main/BUILD
```
cc_library(
    name = "hello-greet",
    srcs = ["hello-greet.cc"],
    hdrs = ["hello-greet.h"],
    visibility = ["//test:__pkg__"],
)

cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    copts = ["-Ilib/include"],
    deps = [
        ":hello-greet",
        "//lib:hello-time",
        "@pybind11",
        "@pybind11//:pybind11_embed"
    ],
)
```

### main/hello-world.cc
```cpp
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
```
