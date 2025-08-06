# MarkupCC

`mkcc` is a command-line tool for converting a custom markup language (MKML) into [SFML](https://www.sfml-dev.org/) C++ projects. It provides a complete workflow from template initialization, compilation, to running, helping you quickly create desktop GUI programs with HTML-like syntax.

## Features

- **`mkcc init`**: Generates basic templates such as `.mkcc`, `mkccmake.json`, and `makefile` in the current directory.
- **`mkcc make`**: Parses MKML files according to the configuration in `mkccmake.json`, generates and compiles C++ code.
- **`mkcc run`**: Runs the built binary; if it does not exist, `make` is executed automatically.
- **`mkcc release`**: Reserved packaging command (in development).
- **`mkcc help`**: Displays command help.

The `mkcc_doc.py` script can scan `/*back_start*/ ... /*back_end*/` blocks for `MKML` macros and generate JSON and HTML documentation under the `docs/` directory.

## Build

### Dependencies

- C++17 compiler (e.g., `g++`)
- [CMake](https://cmake.org/)
- [libxml2](http://xmlsoft.org/)
- [nlohmann/json](https://github.com/nlohmann/json) (header-only)
- [SFML](https://www.sfml-dev.org/) (for building the generated project)

### Generate Executable

```bash
cmake -S . -B build
cmake --build build
```

After building, the executable is located at `build/mkcc` (or the corresponding output path for your platform). You can also run `make` using the generated Makefile.

## Usage

In an environment with `mkcc` installed (can be installed to `/usr/bin` by running `setup.sh`), execute in your project directory:

```bash
mkcc init             # Initialize template
mkcc make             # Compile MKML
mkcc run              # Run the program
```

Project configuration is in the generated `mkccmake.json`, which typically includes fields like `name`, `version`, `entry` (entry MKML file), and `output` (build directory).

## Generate Documentation

```bash
python mkcc_doc.py core
```

The script will generate `mkcc_doc.json` and `mkcc_doc.html` under the `docs/` directory, making it easy to view supported `MKML` macros.

## License

This project is released under the [MIT License](LICENSE).
