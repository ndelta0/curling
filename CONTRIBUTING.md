# Contributing

## Code style

This project follows the Google Code Style (with some exceptions). Please make sure to follow it when contributing.

Exceptions:
- The maximum line length is 120 characters instead of 80.
- The tab size is 4 spaces instead of 2, and the class access modifiers should be indented in the same way as the class declaration.
- The usage of exceptions is allowed, although it is not certain that they will be used in the future.
- The usage of implicit constructors is allowed, although it is not certain that they will be used in the future and should be used when it makes sense.
- The header files should end with `.hpp` instead of `.h`.
- The source files should end with `.cpp` instead of `.cc`.
- The usage of `using namespace ...;` is allowed, but only in the `.cpp` files, unless it is used to shorten the usage in repeated statements.
  - Example:
    ```c++
    using namespace very::nested;
    switch (type) {
      case Type::kType1:
        // ...
        break;
      case Type::kType2:
        // ...
        break;
    }
    ```
    Instead of:
    ```c++
    switch (type) {
      case very::nested::Type::kType1:
        // ...
        break;
      case very::nested::Type::kType2:
        // ...
        break;
    }
    ```
- The usage of `#pragma once` is allowed, but the usage of traditional header guards is not forbidden, and may be the preferred way in the future.
- The usage of forward declarations should be used only when necessary, and should be avoided if possible.
- Code should be written in a way that it is easy to read and understand, and not in a way that it is easy to write.
- The usage of raw pointers is strictly prohibited, unless it is necessary. Use smart pointers instead, preferring `std::unique_ptr` over `std::shared_ptr`.
  - Example: Use of `CURL*` or `curl_slist*`, since this is the way the library is designed.
