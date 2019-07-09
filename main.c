#include "SDL.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
int width, height;

#define fill(x1, y1, x2, y2)                           \
  for (int i = x1; i < x2; i++) {                      \
          for (int j = y1; j < y2; j++) {              \
	          SDL_RenderDrawPoint(renderer, i, j); \
	  }                                            \
  }                                                    

#define fillColor(x1, y1, x2, y2, r, g, b)        \
  SDL_SetRenderDrawColor(renderer, r, g, b, 250); \
  fill(x1, y1, x2, y2);

void init()
{
  width = height = 1000;
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
          printf("SDL Init Error\n");
  } else {
          printf("SDL Init Success\n");
  }
  if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) != 0) {
          printf("SDL Create Window Error\n");
  } else {
          printf("SDL Create Window Success\n");
  }
}

void handleLogic()
{
  if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
          SDL_Quit();
  }
}

void renderBG()
{
  // boundary
  fillColor(290, 100, 300, height - 100, 200, 200, 200);
  fill(width - 300, 100, width - 290, height - 100); 
  fill(290, height - 110, width - 290, height - 100);

  // grid
  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
}

void renderImage()
{
  renderBG();
  SDL_RenderPresent(renderer);
}

int main()
{
  init();
  for (;;) {
          handleLogic();
          renderImage();
  }
  return 0;
}
