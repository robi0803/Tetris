#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
int width, height;

const int FPS = 60;
const int MaxFPS = 1000 / FPS;
int pressedKeys[SDL_NUM_SCANCODES];
int releasedKeys[SDL_NUM_SCANCODES];
int timeElapsed, fallTime;

int bgInPlay[21][12];
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
	  {0, 0, 1, 1},
	  {0, 1, 1, 0},
	  {0, 0, 0, 0}
  },
  { // z
          {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 0, 1, 1},
	  {0, 0, 0, 0}
  },
  { // t
          {0, 0, 0, 0},
	  {0, 1, 1, 1},
	  {0, 0, 1, 0},
	  {0, 0, 0, 0}
  }
};

int currentBlock;
struct Block { int x, y, color, shape[4][4]; };
struct Block *blocksInPlay[50];
struct Block *newBlock(int i, int j)
{
  struct Block *p = malloc(sizeof(struct Block));
  p->x = i;
  p->y = j;
  p->color = rand() % 7;
  memcpy(p->shape, blocks[p->color], sizeof(int) * 16);
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
  fallTime = 300;
  memcpy(bgInPlay, bg, sizeof(int) * 21 * 12);

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

int legalMove(int xpos, int ypos, int shape[4][4])
{
  int bgx, bgy;
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          bgy = j + ypos;
		  bgx = i + xpos + 1;
	          if (shape[j][i] && bgInPlay[bgy][bgx]) {
		          return 0;
		  }
	  }
  }
  return 1;
}

void rotate(int shape[4][4], int direction)
{
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          if (direction = 0) {
		          temp[j][3 - i] = shape[i][j];
		  } else {
		          temp[i][j] = shape[j][3 - i];
		  }
	  }
  }
  memcpy(shape, temp, sizeof(int) * 16);
}

void freezeBlock()
{
  int bgx, bgy;
  struct Block *b = blocksInPlay[currentBlock];
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          bgy = j + b->y;
		  bgx = i + b->x + 1;
	          bgInPlay[bgy][bgx] |= b->shape[j][i];
	  }
  }
}

void gameOver()
{
  for (;;) {
	    if (SDL_PollEvent(&event) && 
		event.type == SDL_QUIT) {
	              SDL_Quit();
	    }
  }
}

void dropBlock()
{
  int i = -1;
  while (blocksInPlay[++i] != NULL);
  struct Block *b = newBlock(4, -1);
  if (legalMove(b->x, b->y, b->shape)) {
          currentBlock = i;
          blocksInPlay[currentBlock] = b;
  } else {
          free(b);
          gameOver();
  }
}

void reset()
{
  memcpy(bgInPlay, bg, sizeof(int) * 21 * 12);
  fallTime = 300;
  for (int i = 0; i < 50; i++) {
          if (blocksInPlay[i] != NULL) {
                  free(blocksInPlay[i]);
		  blocksInPlay[i] = NULL;
	  }
  }
  dropBlock();
}

void swap(struct Block *b)
{
}

void shiftRow3(struct Block *b, int row)
{
}

void shiftRow2(struct Block *b, int row)
{
  if (b->y > row + 4) {
          return;
  } else if (b->y < row - 4) {
          b->y++;
  } else {
          shiftRow3(b, row);
  }
}

void shiftRow(int row)
{
  for (int i = 0; i < 50; i++) {
            if (blocksInPlay[i] != NULL) {
	              shiftRow2(blocksInPlay[i], row); 
	    }
  }
}

void checkRows()
{
  for (int i = 0; i < 20; i++) {
            if (bgInPlay[i] == bgInPlay[20]) {
	            shiftRow(i);
	    }
  }
}

void handleLogic()
{
  if (SDL_PollEvent(&event)) {
          switch(event.type) {
	  case SDL_KEYDOWN:
	          pressedKeys[event.key.keysym.scancode] = 1;
		  break;
	  case SDL_KEYUP:
	          releasedKeys[event.key.keysym.scancode] = 1;
		  break;
	  case SDL_QUIT:
	          SDL_Quit();
		  break;
	  }
  }
  
  struct Block *b = blocksInPlay[currentBlock];
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
          rotate(b->shape, 0);
	  if (!legalMove(b->x, b->y, b->shape)) {
	          rotate(b->shape, 1);
	  }
	  pressedKeys[SDL_SCANCODE_W] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_S]) {
          rotate(b->shape, 1);
	  if (!legalMove(b->x, b->y, b->shape)) {
	          rotate(b->shape, 0);
	  }
	  pressedKeys[SDL_SCANCODE_S] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_F]) {
          swap(b);
	  if (!legalMove(b->x, b->y, b->shape)) {
	          swap(b);
	  }
	  pressedKeys[SDL_SCANCODE_F] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_SPACE]) {
	  fallTime = 50;
	  pressedKeys[SDL_SCANCODE_SPACE] = 0;
  }
  if (releasedKeys[SDL_SCANCODE_SPACE]) {
          fallTime = 300;
	  releasedKeys[SDL_SCANCODE_SPACE] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_ESCAPE]) {
          reset();
	  pressedKeys[SDL_SCANCODE_ESCAPE] = 0;
  }
  if (timeElapsed > fallTime && legalMove(b->x, b->y + 1, b->shape)) {
          timeElapsed = 0;
          b->y++;
  } else if (timeElapsed > fallTime) {
          freezeBlock(b);
	  checkRows();
	  dropBlock();
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

void setColor(int color)
{
  int r, g, b;
  r = g = b = 0;
  switch (color) {
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
  setColor(block->color);
  int x = block->x;
  int y = block->y;
  x = 300 + (x * 40);
  y = 100 + (y * 40);
  int x1, y1, x2, y2;
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          if (block->shape[j][i]) {
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
  for (int i = 0; i < 50; i++) {
          if (blocksInPlay[i] != NULL) {
	          drawBlock(blocksInPlay[i]);
	  }
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
  int startTime = SDL_GetTicks(), endTime, elapsed;
  dropBlock();
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
