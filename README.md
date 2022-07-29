# CHIP-8 Emulator

A CHIP-8 emulator written in C.

![CHIP-8 Emulator](docs/res/chip8-emulator.png)


## Contents

This repository contains:

* C code for the CHIP-8 emulator under `chip8`.
* C++ code that uses the emulator plays games on it under `tools`.
* Some documentation I wrote while I was learning how to code the CHIP-8 emulator under `docs`.
* Dependencies under `data` (for games) and `vendor` (for code).
* Ready-to-use VSCode workspace under `misc`.


## Requirements

* Linux machine (might work under Windows)
* `gcc` or some other C/C++ compiler
* `cmake` version 3.13.
* `make`
* `SDL2` development libraries.
* Maybe something related to `GL`.


## Building and running

Tested on Linux. For building:

```sh
# You start at repository path
mkdir build/
cd build/
cmake ..
make
```

For running:

```sh
# You start at repository path
./build/bin/Emulator
```

If you are using VSCode, `Ctrl+Shift+B` to build either in Debug or Release mode, and `F5` to run a Debug build.


## Acknowledgments

This repository has following submodules:

* ImGui -> https://github.com/ocornut/imgui.git
* ImGui Club -> https://github.com/ocornut/imgui_club
* CHIP-8 Roms -> https://github.com/kripod/chip8-roms.git

This repository also uses SDL2.


## References

- Wikipedia Article -> https://en.wikipedia.org/wiki/CHIP-8
- Technical Reference -> https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference
- Instruction Set -> https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
- Emulator 101 -> http://emulator101.com/