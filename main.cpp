#include <cmath>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "hardware.h"
#include "window.h"

unsigned char buffer[WIDTH * HEIGHT];

int main(int argc, char *argv[]) {
  Hardware hw = Hardware();
  hw.init();
  Window ww = Window(hw);
  ww.resetScreen();
  if (argc != 2) {
    fputs("format : ./main filename.c8\n", stderr);
    return EXIT_FAILURE;
  } else {
    if (!hw.load_program(argv[1])) {
      fputs("file read failed\n", stderr);
      return EXIT_FAILURE;
    }
  }

  while (ww.handleEvents()) {
    printf("%i\n", hw.pc);
    hw.fetch();
    hw.execute();
    if (hw.draw_flag == true) {
      ww.drawBuffer();
      hw.draw_flag = false;
      usleep((int)((float)(1.0/60.0) * (float)pow(10, 6))); // 1000
    }
    // usleep(1000);
  }

  return EXIT_SUCCESS;
}
