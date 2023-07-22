# Compiler Journey

## Introduction

Same as with the [Emulator Journey](EmulatorJourney.md), I will be writing here all the steps and progress I do for this project.

As background, I created the emulator for the CHIP-8 in summer 2022. Now, one year later, I want to continue my journey writing a compiler for this platform.

I have wanted to create a language for some time, and the idea of compilers fascinates me. I do not have neither the knowledge nor expertise to create my own programming language for a desktop computer, but I think targeting the CHIP-8 might be a really good opportunity to both learn and do something cool. I have chosen the FORTH language as the language I will be developing to generate code for the CHIP-8 just because I want to learn how to create something similar. I already played around with stuff like [jonesforth.S](https://github.com/nornagon/jonesforth/blob/master/jonesforth.S), but I never finished it. Following the tutorial in a different platform was difficult, and I ended up leaving it. This time, however, the target platform is much easier, with around 35 OpCodes.

The programming language for the compiler will be something really easy for me to program, Python. I do not really want to think too much about language stuff, and just focus on the actual problem to solve. If, when I am done, I want to actually compile the compiler to an executable, I might give [Zig](https://ziglang.org/) a go.

[This blog post](https://www.reaktor.com/articles/why-play-games-when-you-can-write-a-game-boy-compiler-instead) was the inspiration for me to try to tackle this challenge. I will be following a similar approach, having tests that will generate the appropriate binary files to check that I am not doing weird and bad stuff.


---

## The Journey

### Day 1 - 22/07/2023

I believe the initial objectives for this project are going to be:

- Select a target ROM. This ROM will be used as an example on how to code stuff. We will try to generate the same binary out of text files.
- Create a simple file that duplicates the contents of the target ROM, where there is only one function available, `emit`, which will directly copy-paste the next token, a number, to a file.
- Check the instruction set of the CHIP-8 and come up with functions for each of the OpCodes.
- Implement little by little the OpCodes and their functions.
- Implement actual language stuff, like functions and variables, in a stack-like way, same as FORTH.

As an initial target ROM, I will select `Breakout (Brix hack) [David Winter, 1997].ch8`. Later, I might move to `Trip8 Demo (2008) [Revival Studios].ch8`, as it is way bigger in size.

The target binary data is the following:

```
$ hexdump -v -C "Breakout (Brix hack) [David Winter, 1997].ch8"

00000000  6e 05 65 00 6b 06 6a 00  a3 0c da b1 7a 04 3a 40  |n.e.k.j.....z.:@|
00000010  12 08 7b 02 3b 12 12 06  6c 20 6d 1f a3 10 dc d1  |..{.;...l m.....|
00000020  22 f6 60 00 61 00 a3 12  d0 11 70 08 a3 0e d0 11  |".`.a.....p.....|
00000030  60 40 f0 15 f0 07 30 00  12 34 c6 0f 67 1e 68 01  |`@....0..4..g.h.|
00000040  69 ff a3 0e d6 71 a3 10  dc d1 60 04 e0 a1 7c fe  |i....q....`...|.|
00000050  60 06 e0 a1 7c 02 60 3f  8c 02 dc d1 a3 0e d6 71  |`...|.`?.......q|
00000060  86 84 87 94 60 3f 86 02  61 1f 87 12 47 1f 12 ac  |....`?..a...G...|
00000070  46 00 68 01 46 3f 68 ff  47 00 69 01 d6 71 3f 01  |F.h.F?h.G.i..q?.|
00000080  12 aa 47 1f 12 aa 60 05  80 75 3f 00 12 aa 60 01  |..G...`..u?...`.|
00000090  f0 18 80 60 61 fc 80 12  a3 0c d0 71 60 fe 89 03  |...`a......q`...|
000000a0  22 f6 75 01 22 f6 45 60  12 de 12 46 69 ff 80 60  |".u.".E`...Fi..`|
000000b0  80 c5 3f 01 12 ca 61 02  80 15 3f 01 12 e0 80 15  |..?...a...?.....|
000000c0  3f 01 12 ee 80 15 3f 01  12 e8 60 20 f0 18 a3 0e  |?.....?...` ....|
000000d0  7e ff 80 e0 80 04 61 00  d0 11 3e 00 12 30 12 de  |~.....a...>..0..|
000000e0  78 ff 48 fe 68 ff 12 ee  78 01 48 02 68 01 60 04  |x.H.h...x.H.h.`.|
000000f0  f0 18 69 ff 12 70 a3 14  f5 33 f2 65 f1 29 63 37  |..i..p...3.e.)c7|
00000100  64 00 d3 45 73 05 f2 29  d3 45 00 ee f0 00 80 00  |d..Es..).E......|
00000110  fc 00 aa 00 00 00 00 00                           |........|
00000118
```

I will create a file that will do the following:

```
EMIT 0x6e
EMIT 0x05
EMIT 0x65
EMIT 0x00
# ...
```

The only thing that is left is to begin writing the compiler, which will simply read the file and emit into the target output file the number at its right. Then, we can simply create a test that compares the output to the original file.

After having done that and seeing that the test passes, I am happy for today. I have also started writing the skeleton of a basic file that can be used as reference on how to code for the new programming language. I will be calling it `Javith`, and the file extension will be `jth`.