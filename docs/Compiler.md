# CHIP-8 Compiler Application

This document defines the high-level requirements of the CHIP-8 Compiler Application.

## List of Requirements

- The compiler shall receive as input a file.
- The compiler shall generate as output a valid CHIP-8 file.
- The language the files are written in shall be FORTH-like.

## Requirements Implementation

The requirements are a bit vague, but that is Ok. 

For the first one, we will be passing to the compiler the path to the file to compile. Our compiler will then read that file and generate the appropiate binary code CHIP-8 compatible. This binary file will be the output file of the program.

Regarding the language, there are multiple places where we can look for information. Examples:
- High level steps on how to write a FORTH language [Write Yourself a Forth](https://beza1e1.tuxen.de/articles/forth.html)
- [Moving Forth](http://www.bradrodriguez.com/papers/moving1.htm)
- A Github file where FORTH is implemented in assembly [jonesforth.S](https://github.com/nornagon/jonesforth/blob/master/jonesforth.S)

In order to emit valid CHIP-8 code, we need to know about the CHIP-8 instruction set. For that, we can simply go to [Journey](Journey.md)