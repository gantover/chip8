#ifndef H_HARDWARE
#define H_HARDWARE
#include "constants.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>

class Hardware {
public:
  unsigned short opcode;
  unsigned char memory[4096];
  unsigned char V[16];

  unsigned short I; // index
  unsigned short pc;

  unsigned short stack[16];
  unsigned short sp; // stack pointer

  unsigned char key[16];

  unsigned char delay_timer;
  unsigned char sound_timer;

  unsigned char gfx[WIDTH * HEIGHT];

  bool draw_flag;

  Hardware();

  bool load_program(char *);

  void init();

  void incr();

  void fetch();

  void execute();

  void update_timers();
};

#endif
