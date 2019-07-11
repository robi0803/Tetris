#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
int width, height;
int head, tail;

const int FPS = 60;
const int MaxFPS = 1000 / FPS;
int pressedKeys[SDL_NUM_SCANCODES];
int timeElapsed, fallTime;
int temp_block[4][4];

int bg[21][12] = {
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 1
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 2
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 3
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 4
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 5
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 6
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 7
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 8
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 9
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 10
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 11
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 12
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 13
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 14
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 15
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 16
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 17
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 18
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 19
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 20
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 21
};

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

struct Block { int x, y, shape, position; };
struct Block *blocksInPlay[100];
struct Block *newBlock(int i, int j)
{
  struct Block *p = malloc(sizeof(struct Block));
  p->x = i;
  p->y = j;
  p->shape = rand() % 7;
  p->position = 0;
  return p;
}

void fill(int x1, int y1, int x2, int y2)
{
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

int legalMove(int xpos, int ypos, int shape)
{
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          if (blocks[shape][j][i] & bg[j + ypos][i + xpos + 1]) {
		          return 0;
		  }
	  }
  }
  return 1;
}

void handleLogic()
{
  if (SDL_PollEvent(&event)) {
          switch(event.type) {
	  case SDL_KEYDOWN:
	          pressedKeys[event.key.keysym.scancode] = 1;
		  break;
	  case SDL_QUIT:
	          SDL_Quit();
		  break;
	  }
  }
  
  struct Block *b = blocksInPlay[tail - 1];
  if (pressedKeys[SDL_SCANCODE_A]) {
          if (legalMove(b->x - 1, b->y, b->shape)) {
	          b->x--;
	  }
	  pressedKeys[SDL_SCANCODE_A] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_D]) {
          if (legalMove(b->x + 1, b->y, b->shape)) {
	          b->x++;
	  }
	  pressedKeys[SDL_SCANCODE_D] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_W]) {
          b->position = (b->position + 1) % 4;
	  pressedKeys[SDL_SCANCODE_W] = 0;
  }

  if (timeElapsed > fallTime && legalMove(b->x, b->y + 1, b->shape)) {
          timeElapsed = 0;
          b->y++;
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

void transpose()
{
  int temp;
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          temp = temp_block[i][j];
		  temp_block[i][j] = temp_block[j][i];
		  temp_block[j][i] = temp;
	  }
  } 
}

void rotate(int shape, int position)
{
  memcpy(temp_block, blocks[shape], sizeof(int) * 16);
  for (int i = 0; i < position; i++) {
          transpose();
  }
}

void drawBlock(struct Block *block)
{
  setColor(block->shape);
  rotate(block->shape, block->position);

  int x = block->x;
  int y = block->y;
  x = 300 + (x * 40);
  y = 100 + (y * 40);
  int x1, y1, x2, y2;
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          if (temp_block[j][i]) {
		          x1 = x + (i * 40);
			  y1 = y + (j * 40);
			  x2 = x1 + 40;
			  y2 = y1 + 40;
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
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  renderBG();
  renderBlocks();
  SDL_RenderPresent(renderer);
}

int main()
{
  init();
  blocksInPlay[tail++] = newBlock(0, 0);
  blocksInPlay[tail++] = newBlock(0, 2);
  int startTime = SDL_GetTicks(), endTime, elapsed;
  fallTime = 300;
  for (;;) {
          handleLogic();
          renderImage();

	  endTime = SDL_GetTicks();
	  elapsed = endTime - startTime;
	  elapsed = elapsed < MaxFPS ? elapsed : MaxFPS;
	  startTime = endTime;
	  timeElapsed += elapsed;
  }
  return 0;
}
