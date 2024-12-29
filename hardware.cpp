#include "hardware.h"
#include "constants.h"
#include <cstring>

unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Hardware::Hardware() {};

void Hardware::init() {
  opcode = 0x0000;
  memset(memory, 0, sizeof(unsigned char) * 4096);
  memset(V, 0, sizeof(unsigned char) * 16);

  I = 0;
  pc = 0x200;

  memset(stack, 0, sizeof(unsigned short) * 16);
  sp = 0;

  memset(key, 0, sizeof(unsigned char) * 16);

  delay_timer = 0;
  sound_timer = 0;

  memset(gfx, 0, sizeof(unsigned char) * WIDTH * HEIGHT);

  draw_flag = false;

  std::cout << "building a CPU" << std::endl;

  for (int i = 0; i < 80; i++) {
    memory[i] = chip8_fontset[i];
  }
}

bool Hardware::load_program(char *filename) {
  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    fputs("invalid filename", stderr);
    return false;
  }
  fseek(f, 0, SEEK_END);
  long lsize = ftell(f); // current position of the stream
  rewind(f);

  char *buf = (char *)malloc(sizeof(char) * lsize);
  if (buf == NULL) {
    fputs("memory error when creating file", stderr);
    return false;
  }

  size_t result = fread(buf, 1, lsize, f);
  if (result != lsize) {
    fputs("failed to copy file content to buffer", stderr);
  }
  // the program occupies space from 0x200 to 0xFFF
  if (lsize < 4096 - 512) {
    for (int i = 0; i < lsize; i++) {
      memory[i + 512] = buf[i];
      printf("loaded mem(%x) == buf(%x) at %i\n", memory[i + 512], buf[i],
             i + 512);
    }
    printf("program loaded in memory from 512 to 512+%li\n", lsize);
  } else {
    fputs("the program does not fit in memory", stderr);
    return false;
  }
  fclose(f);
  free(buf);
  return true;
}

void Hardware::incr() { pc += 2; }; // each instruction is 16bit

void Hardware::update_timers() {
  if (sound_timer > 0)
    sound_timer--;
  if (delay_timer > 0)
    delay_timer--;
}

void Hardware::fetch() {
  opcode = memory[pc] << 8 | memory[pc + 1];
};

// /!\ some jump statements should not include an increment to the pc
void Hardware::execute() {
  switch (opcode & 0xF000) {
  case 0x0000: {
    switch (opcode & 0x00FF) {
    case 0x00E0: {
      for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
          gfx[y * WIDTH + x] = 0;
        }
      }
      incr();
      break;
    }
    case 0x00EE: { // return from subroutine
      // get the pc of the level above
      sp--;
      pc = stack[sp];
      incr(); // we need it here so we don't go enter the same function again
              // upon exiting it
      break;
    }
    default:
      printf("unknown opcode from %x\n", opcode);
    }
    break;
  }
  case 0x1000: {
    pc = opcode & 0x0FFF;
    break;
  }
  case 0x2000: {
    stack[sp] = pc; // at this depth, we where at pc
    sp++;           // now we go deeper
    pc = opcode & 0x0FFF;
    break;
  }
  case 0x3000: {
    unsigned char Vx = V[(opcode & 0x0F00) >> 8];
    unsigned char Val = opcode & 0x00FF;
    incr();
    if (Vx == Val)
      incr();
    // we already increment automatically to go to the next instruction
    // here we also increment to skip that next instruction
    break;
  }
  case 0x4000: {
    unsigned char Vx = V[(opcode & 0x0F00) >> 8];
    unsigned char Val = opcode & 0x00FF;
    incr();
    if (Vx != Val)
      incr();
    break;
  }
  case 0x5000: {
    unsigned char Vx = V[(opcode & 0x0F00) >> 8];
    unsigned char Vy = V[(opcode & 0x00F0) >> 4];
    incr();
    if (Vx == Vy)
      incr();
    break;
  }
  case 0x6000: {
    unsigned char *Vx = &V[(opcode & 0x0F00) >> 8];
    unsigned char Val = opcode & 0x00FF;
    *Vx = Val;
    incr();
    break;
  }
  case 0x7000: {
    unsigned char *Vx = &V[(opcode & 0x0F00) >> 8];
    unsigned char Val = opcode & 0x00FF;
    *Vx += Val;
    incr();
    break;
  }
  case 0x8000: {
    unsigned char *Vx = &V[(opcode & 0x0F00) >> 8];
    unsigned char Vy = V[(opcode & 0x00F0) >> 4];
    incr();
    switch (opcode & 0x000F) {
    case 0x0000: {
      *Vx = Vy;
      break;
    }
    case 0x0001: {
      *Vx |= Vy;
      break;
    }
    case 0x0002: {
      *Vx &= Vy;
      break;
    }
    case 0x0003: {
      *Vx ^= Vy;
      break;
    }
    case 0x0004: {
      if (Vy > (0xFF - *Vx)) {
        // if what we add is bigger than what is remaining
        V[0xF] = 1; // carry flag
      } else {
        V[0xF] = 0;
      }
      *Vx += Vy;
      break;
    }
    case 0x0005: {
      if (*Vx >= Vy) {
        V[0xF] = 1;
      } else {
        V[0xF] = 0;
      }
      *Vx -= Vy;
      break;
    }
    case 0x0006: {
      V[0xF] = (*Vx) & 0x1; // least significant bit of VX prior to the shift
      *Vx >>= 1;
      break;
    }
    case 0x0007: {
      if (Vy >= *Vx) {
        V[0xF] = 1;
      } else {
        V[0xF] = 0;
      }
      *Vx = Vy - *Vx;
      break;
    }
    case 0x000E: {
      // TODO: was wrong!!! V[0xF] = *Vx & 0x10; // set VF to 1 if most
      // significant bit of Vx was set
      V[0xF] = (*Vx) >> 7; // set VF to 1 if most significant bit of Vx was set
      *Vx <<= 1;
      break;
    }
    }
    break;
  }
  case 0x9000: {
    unsigned char Vx = V[(opcode & 0x0F00) >> 8];
    unsigned char Vy = V[(opcode & 0x00F0) >> 4];
    incr();
    if (Vx != Vy)
      incr();
    break;
  }
  case 0xA000: {
    I = opcode & 0x0FFF;
    incr();
    break;
  }
  case 0xB000: {
    pc = V[0] + (opcode & 0x0FFF);
    // TODO: should we increment?
    break;
  }
  case 0xC000: {
    V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF)) & (opcode & 0x00FF);
    incr();
    break;
  }
  case 0xD000: {
    unsigned short x = V[(opcode & 0x0F00) >> 8];
    unsigned short y = V[(opcode & 0x00F0) >> 4];
    unsigned short h = opcode & 0x000F;
    unsigned short row_sprite;
    V[0xF] = 0;
    // each row is a byte long so 8 bits to extract
    for (int row = 0; row < h; row++) {
      row_sprite = memory[I + row];
      for (int col = 0; col < 8; col++) {
        if ((row_sprite & (0x80 >> col)) != 0) {
          if (gfx[x + col + (y + row) * 64] == 1) {
            V[0xF] = 1;
          };
          gfx[x + col + (y + row) * 64] ^= 1;
        }
      }
    }
    draw_flag = true;
    incr();
    break;
  }
  case 0xE000: {
    unsigned char Vx = V[(opcode & 0x0F00) >> 8];
    Vx = Vx & 0x0F;
    incr();
    switch (opcode & 0x00FF) {
    case 0x009E: {
      if (key[Vx] != 0) {
        incr();
      };
      break;
    }
    case 0x00A1: {
      if (key[Vx] == 0) {
        incr();
      };
      break;
    }
    }
    break;
  }
  case 0xF000: {
    unsigned char *Vx = &V[(opcode & 0x0F00) >> 8];
    switch (opcode & 0x00FF) {
    case 0x0007: {
      *Vx = delay_timer;
      incr();
      break;
    }
    case 0x000A: {
      // we halt the execution of the program until a key is pressed
      for (int i = 0; i < 16; i++) {
        if (key[i] != 0) {
          *Vx = i;
          incr();
          break;
        }
      }
      break;
    }
    case 0x0015: {
      delay_timer = *Vx;
      incr();
      break;
    }
    case 0x0018: {
      sound_timer = *Vx;
      incr();
      break;
    }
    case 0x001E: {
      I += *Vx;
      incr();
      break;
    }
    case 0x0029: {
      I = 0 + *Vx * 5;
      incr();
      break;
    }
    case 0x0033: {
      memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
      memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
      memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
      incr();
      break;
    }
    case 0x0055: { // reg dump
      for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
        memory[i + I] = V[i];
      }
      incr();
      break;
    }
    case 0x0065: { // reg load
      for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
        V[i] = memory[i + I];
      }
      incr();
      break;
    }
    }
    break;
  }
  }
  update_timers();
}
