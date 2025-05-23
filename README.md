<div id="toc">
  <ul style="list-style: none;" align="center">
    <summary>
      <h1> Cpp Archive </h1>
    </summary>
  </ul>
</div>

<p align="center"><i>A personal archive of C++ code snippets, algorithms, and utility programs.</i></p>

## Overview

This repository serves as a collection of reusable C++ code, including:

- Common algorithms and data structures
- Utility classes and functions
- Example programs and problem solutions
- Templates for common C++ patterns

The purpose is to provide a handy reference for C++ development and to encourage code reuse across projects.

## Features

- 100% C++ code
- Well-organized by topic and type
- Self-contained snippets for easy copy-paste
- Includes comments and usage examples where appropriate

## Header Files

Descriptions for files in the `header` directory ([browse all headers](https://github.com/qluana7/Cpp-Archive/tree/main/header)):

- **typedef.h**  
  Provides type aliases for fixed-width integers (`i8`, `u16`, etc.), floating-point types (`f32`, `f64`), and optionally 128-bit types if supported, to simplify type usage across code.

- **containerlib**  
  Utilities for working with STL containers. Includes type traits to detect vectors, conversion utilities, and functions to parse strings into vectors (including nested vectors).

- **enum_helper**  
  Macros and templates for working with enums, including range checks, flag checks, casting, and bitwise operator overloads to make enums easier to use as bitfields or flag sets.

- **fdstream**  
  Defines `ifdstream` and `ofdstream`: C++ input/output stream wrappers for file descriptors, allowing use of standard streams with POSIX file descriptors (useful for advanced I/O and IPC).

- **function_traits**  
  Introspection utilities for `std::function` types, enabling you to extract argument and return types, and the number of arguments, for generic programming.

- **ipc**  
  An experimental header for process management and inter-process communication. Defines `Pipe`, `ProcessStartInfo`, and a `Process` class for launching and managing external processes, with support for I/O redirection.

- **range**  
  Provides a compile-time range class template, allowing you to easily iterate over integer ranges using C++ range-based for loops, similar to Python's `range()`.

- **streamlib**  
  Utilities for stream manipulation, such as ignoring values of specific types or using custom initializers when reading from streams. Useful for flexible and generic input parsing.

- **strlib**  
  Utilities for string manipulation: joining, splitting, trimming, padding, case conversion, and character checks. Includes both modern and deprecated split/join methods for containers and algorithms.

- **threadlib**  
  Utilities for asynchronous programming and event handling. Provides an `event` class template with synchronous and asynchronous policies, handler registration/removal, and a timer class for periodic events (since C++20).

---