#ifndef H_WINDOW
#define H_WINDOW

#include "hardware.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

class Window {
public:
  Hardware &hw;
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Window *window;

  ~Window();
  Window(Hardware &);
  bool handleEvents();
  int resetScreen();
  int drawBuffer();
  int close();
};

#endif
