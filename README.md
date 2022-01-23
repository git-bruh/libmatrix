# libmatrix

C library for interacting with the federated Matrix communications protocol.

# Pre-requisites

* C11 Compiler

* Meson

* [cJSON](https://github.com/DaveGamble/cJSON)

* [cURL](https://github.com/curl/curl)

# Building

Run `meson . build` and `ninja -C build`. The library will be located at `build/libmatrix.a`.

# Usage

...

# Contributing

Contributions are always welcome, the following points should be kept in mind:

* Pass `-Db_sanitize=address,undefined` to `meson` to enable sanitizers which help in finding memory leaks or undefined behaviour.

* Before submitting a PR, format the code with `ninja -C build clang-format` which runs `clang-format`.
