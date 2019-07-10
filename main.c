#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
int width, height;
int head, tail;

int blocks[7][4][4] = {
  { // o
          {0, 0, 0, 0},
          {0, 1, 1, 0},
	  {0, 1, 1, 0},
	  {0, 0, 0, 0}
  },
  { // i
          {0, 0, 1, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0}
  },
  { // l
          {0, 1, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 1, 0}
  },
  { // j
          {0, 0, 1, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0}, 
	  {0, 1, 1, 0}
  },
  { // s
          {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {1, 1, 0, 0},
	  {0, 0, 0, 0}
  },
  { // z
          {0, 0, 0, 0},
	  {1, 1, 0, 0},
	  {0, 1, 1, 0},
	  {0, 0, 0, 0}
  },
  { // t
          {0, 0, 0, 0},
	  {1, 1, 1, 0},
	  {0, 1, 0, 0},
	  {0, 0, 0, 0}
  }
};

struct Block { int x, y, shape; };
struct Block *blocksInPlay[100];
struct Block *newBlock(int i, int j)
{
  struct Block *p = malloc(sizeof(struct Block));
  p->x = i;
  p->y = j;
  p->shape = rand() % 7;
  return p;
}

void fill(int x1, int y1, int x2, int y2) {
  for (int i = x1; i < x2; i++) {
          for (int j = y1; j < y2; j++) {
	          SDL_RenderDrawPoint(renderer, i, j);
	  }
  }
}

void init()
{
  srand(time(0));
  width = height = 1000;
  head = tail = 0;
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
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  fill(290, 100, 300, height - 100);
  fill(width - 300, 100, width - 290, height - 100); 
  fill(290, height - 100, width - 290, height - 90);

  // grid
  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
  for (int i = 340; i < width - 300; i += 40) {
          fill(i, 100, i + 1, height - 100);
  }
  for (int j = 100; j < height - 100; j += 40) {
          fill(300, j, width - 300, j + 1);
  }

  // hold
  SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
  fill(50, 100, 250, 101);
  fill(50, 300, 250, 301);
  fill(50, 100, 51, 301);
  fill(250, 100, 251, 301);
}

void setColor(int shape)
{
  int r, g, b;
  r = g = b = 0;
  switch (shape) {
  case 0: // o
          r = g = 255;
          break;
  case 1: // i
          g = b = 255;
	  break;
  case 2: // l
          r = 255;
	  g = 128;
          break;
  case 3: // j
          b = 255;
          break;
  case 4: // s
          g = 255;
	  break;
  case 5: // z
          r = 255;
          break;
  case 6: // t
          r = b = 150;
          break;
  }
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

void drawBlock(struct Block *block)
{
  int shape = block->shape;
  setColor(shape);

  int x = block->x;
  int y = block->y;
  x = 300 + (x * 40);
  y = 100 + (y * 40);
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          if (blocks[shape][i][j]) {
		          int x1 = x + (i * 40);
			  int y1 = y + (j * 40);
			  int x2 = x1 + 40;
			  int y2 = y1 + 40;
			  fill(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
		  }
	  }
  }
}

void renderBlocks()
{
  int i = head;
  while (i != tail) {
          drawBlock(blocksInPlay[i]);
	  i = (i + 1) % 100;
  }
}

void renderImage()
{
  renderBG();
  renderBlocks();
  SDL_RenderPresent(renderer);
}

int main()
{
  init();
  blocksInPlay[tail++] = newBlock(0, 0);
  blocksInPlay[tail++] = newBlock(0, 2);
  for (;;) {
          handleLogic();
          renderImage();
  }
  return 0;
}
