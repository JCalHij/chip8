# CHIP-8 Emulator Application

This document defines the high-level requirements that a CHIP-8 Emulator Application needs in order to be properly considered an emulator, as well as how these requirements can be met.


## List of Requirements

- The emulator shall load ROMs from memory using a user interface.
- The emulation throttle (instructions per frame/seconds/...) shall be a configurable parameter.
- The emulator shall provide a ROM contents visualizer in binary and hex.
- The emulator shall provide an instruction disassembly view.
- The emulator shall provide a CHIP-8 data view (registers, stack, ...).
- The emulator shall provide different running modes: normal run, run instruction by instruction.
- The emulator shall provide a window for rendering the CHIP-8 VRAM.


## Requirements Implementation

Most of the requirements are related to User Interface. For that, the library `ImGui` will be chosen to generate the user interface.

To load ROMs, one can simply drag and drop the file into the window, and capture it using a `SDL_DROPFILE` event. Also, a file browser [such as this one](https://github.com/AirGuanZ/imgui-filebrowser) can be used to select a file.

The throttling can be a slider done with `ImGui`.

The ROM visualizer can be [this ImGui library](https://github.com/ocornut/imgui_club).

The instsruction disassembly view and the CHIP-8 data view can be custom-made windows with `ImGui`.

The running modes can be easily implemented in code, and configurable with `ImGui`.

The CHIP-8 VRAM window can be easily created with `ImGui` as well.