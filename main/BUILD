load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

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
