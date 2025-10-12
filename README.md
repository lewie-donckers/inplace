# Inplace

**Inplace** is a C++20 header-only library with _inplace_ alternatives to
standard library containers (and container-like wrappers). The term _inplace_
is used as in
[`std::inplace_vector`](https://en.cppreference.com/w/cpp/container/inplace_vector.html);
indicating that these containers store and properly align elements within
themselves. Because of this, they can be used in environments where dynamic
memory allocations are not allowed.

> **Inplace** is in a very early stage of development.

## Plans

Implement alternatives for:

- `std::string`
- `std::vector`
- `std::map` (and standard variants)
- `std::set` (and standard variants)
- `std::function`
- `std::copyable_function`
- `std::move_only_function`
- `std::any`
    - and non-standard move-only variant
- `std::hive` (from C++26)

## Misc

If you want to run the unit tests with thread-sanitizer enabled, you'll need to:

```sh
sudo sysctl vm.mmap_rnd_bits=28
```