# Compiler Journey

## Introduction

Same as with the [Emulator Journey](EmulatorJourney.md), I will be writing here all the steps and progress I do for this project.

As background, I created the emulator for the CHIP-8 in summer 2022. Now, one year later, I want to continue my journey writing a compiler for this platform.

I have wanted to create a language for some time, and the idea of compilers fascinates me. I do not have neither the knowledge nor expertise to create my own programming language for a desktop computer, but I think targeting the CHIP-8 might be a really good opportunity to both learn and do something cool. I have chosen the FORTH language as the language I will be developing to generate code for the CHIP-8 just because I want to learn how to create something similar. I already played around with stuff like [jonesforth.S](https://github.com/nornagon/jonesforth/blob/master/jonesforth.S), but I never finished it. Following the tutorial in a different platform was difficult, and I ended up leaving it. This time, however, the target platform is much easier, with around 35 OpCodes.

The programming language for the compiler will be something really easy for me to program, Python. I do not really want to think too much about language stuff, and just focus on the actual problem to solve. If, when I am done, I want to actually compile the compiler to an executable, I might give [Zig](https://ziglang.org/) a go.

[This blog post](https://www.reaktor.com/articles/why-play-games-when-you-can-write-a-game-boy-compiler-instead) was the inspiration for me to try to tackle this challenge. I will be following a similar approach, having tests that will generate the appropriate binary files to check that I am not doing weird and bad stuff.