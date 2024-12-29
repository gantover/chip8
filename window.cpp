#include "window.h"
#include "constants.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <stdlib.h>

Window::Window(Hardware &refHw) : hw(refHw) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WIDTH * SCALE, HEIGHT * SCALE, 0, &window,
                              &renderer);
  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      hw.gfx[y * WIDTH + x] = 0;
    }
  }
}

int Window::drawBuffer() {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (hw.gfx[i * WIDTH + j] == 1) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      } else {
        SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR);
      }
      SDL_RenderDrawPoint(renderer, j, i);
    }
  }
  SDL_RenderPresent(renderer);
  return EXIT_SUCCESS;
}

int Window::resetScreen() {
  SDL_SetRenderDrawColor(renderer, DEFAULT_COLOR);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  return EXIT_SUCCESS;
}

/*  Keyboard mapping
Keypad                   Keyboard
+-+-+-+-+                +-+-+-+-+
|1|2|3|C|                |1|2|3|4|
+-+-+-+-+                +-+-+-+-+
|4|5|6|D|                |Q|W|E|R|
+-+-+-+-+       =>       +-+-+-+-+
|7|8|9|E|                |A|S|D|F|
+-+-+-+-+                +-+-+-+-+
|A|0|B|F|                |Z|X|C|V|
+-+-+-+-+                +-+-+-+-+
 */
bool Window::handleEvents() {
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      return false;
    }
    case SDL_KEYDOWN: {
      switch (event.key.keysym.sym) {
      case SDLK_1:
        hw.key[0x1] = 1;
        break;
      case SDLK_2:
        hw.key[0x2] = 1;
        break;
      case SDLK_3:
        hw.key[0x3] = 1;
        break;
      case SDLK_4:
        hw.key[0xC] = 1;
        break;
      case SDLK_q:
        hw.key[0x4] = 1;
        break;
      case SDLK_w:
        hw.key[0x5] = 1;
        break;
      case SDLK_e:
        hw.key[0x6] = 1;
        break;
      case SDLK_r:
        hw.key[0xD] = 1;
        break;
      case SDLK_a:
        hw.key[0x7] = 1;
        break;
      case SDLK_s:
        hw.key[0x8] = 1;
        break;
      case SDLK_d:
        hw.key[0x9] = 1;
        break;
      case SDLK_f:
        hw.key[0xE] = 1;
        break;
      case SDLK_z:
        hw.key[0xA] = 1;
        break;
      case SDLK_x:
        hw.key[0x0] = 1;
        break;
      case SDLK_c:
        hw.key[0xB] = 1;
        break;
      case SDLK_v:
        hw.key[0xF] = 1;
        break;
      }
      break;
    }
    case SDL_KEYUP: {
      switch (event.key.keysym.sym) {
      case SDLK_1:
        hw.key[0x1] = 0;
        break;
      case SDLK_2:
        hw.key[0x2] = 0;
        break;
      case SDLK_3:
        hw.key[0x3] = 0;
        break;
      case SDLK_4:
        hw.key[0xC] = 0;
        break;
      case SDLK_q:
        hw.key[0x4] = 0;
        break;
      case SDLK_w:
        hw.key[0x5] = 0;
        break;
      case SDLK_e:
        hw.key[0x6] = 0;
        break;
      case SDLK_r:
        hw.key[0xD] = 0;
        break;
      case SDLK_a:
        hw.key[0x7] = 0;
        break;
      case SDLK_s:
        hw.key[0x8] = 0;
        break;
      case SDLK_d:
        hw.key[0x9] = 0;
        break;
      case SDLK_f:
        hw.key[0xE] = 0;
        break;
      case SDLK_z:
        hw.key[0xA] = 0;
        break;
      case SDLK_x:
        hw.key[0x0] = 0;
        break;
      case SDLK_c:
        hw.key[0xB] = 0;
        break;
      case SDLK_v:
        hw.key[0xF] = 0;
        break;
      }
      break;
    }
    }
  }
  return true;
}

Window::~Window() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
