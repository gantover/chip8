# CHIP8

A simple CHIP8 interpreter

```sh
cd build
cmake ..
make
./main games/pong2.c8
```

### Useful ressources

- https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- https://en.wikipedia.org/wiki/CHIP-8
- http://www.codeslinger.co.uk/pages/projects/chip8/mychip8.html

### What should you expect to learn from this project

- C shift operator
- C memory and reading unknown lenght files
    - On the stack with the computer's memory and its initialisation to 0 values (`memset` or `= {0}`)
    - On the heap with a buffer to read the byte-code file
- How a basic console/computer works with 
    - How does the computer act on memory, what, which and where data is storred.
    - What does a key press do to the computer.
    - Common instructions (jumping, entering a function (stack pointer, stack depth, ...), arithmetic)

